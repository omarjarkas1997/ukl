
#define _XOPEN_SOURCE

#define _GNU_SOURCE

#include "linux.h"
#include "utils.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "terminal.h"

struct terminal_status_s {
    int fd;
    struct termios termios;
};

/** libcrun_new_terminal - Create a new pseudo-terminal
 *
 * Description:
 *   This function creates a new pseudo-terminal and returns its file descriptor.
 *   It opens the master side of a pseudo-terminal, unlocks the slave side, and
 *   returns the name of the slave side. The function is typically used to set up
 *   a terminal for a container process.
 *
 * Parameters:
 *   char **pty - A pointer to a char pointer where the function will store the
 *                path to the slave side of the pseudo-terminal. The caller is
 *                responsible for freeing the memory allocated for this path.
 *
 *   libcrun_error_t *err - A pointer to a libcrun_error_t structure where the
 *                          function will store error information if it fails.
 *                          The structure includes details such as error code
 *                          and error message.
 *
 * Returns:
 *   On success, returns the file descriptor of the master side of the newly
 *   created pseudo-terminal. On failure, returns -1 and sets appropriate
 *   values in the provided `libcrun_error_t` structure.
 *
 *   The function performs error checking using the `UNLIKELY` macro, which
 *   likely hints at branch prediction optimization. If certain operations like
 *   opening the pseudo-terminal, getting its name, or unlocking it fail,
 *   the function will create an error using `crun_make_error` and return -1.
 *
 * Usage:
 *   char *pty_path;
 *   libcrun_error_t error;
 *   int master_fd = libcrun_new_terminal(&pty_path, &error);
 *   if (master_fd < 0) {
 *       // Handle error
 *   }
 *   // Use the master_fd and pty_path as needed
 *   // Free the allocated pty_path after use */
int libcrun_new_terminal(char **pty, libcrun_error_t *err) {
    char buf[64];
    int ret;
    cleanup_close int fd = open ("/dev/ptmx", O_RDWR | O_NOCTTY | O_CLOEXEC);
    if (UNLIKELY (fd < 0))
        return crun_make_error (err, errno, "open `/dev/ptmx`");
    // Calls ptsname_r to get the name of the slave PTY associated with the master PTY fd. The name is stored in buf.
    ret = ptsname_r(fd, buf, sizeof (buf));
    if (UNLIKELY (ret != 0))
        return crun_make_error (err, errno, "ptsname");

    ret = unlockpt(fd);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "unlockpt");

    *pty = xstrdup (buf);

    ret = fd;
    fd = -1;

    return ret;
}

/** libcrun_set_stdio
 *
 * Sets up the standard input/output/error for a container process by duplicating
 * the file descriptor of a specified pseudo-terminal.
 *
 * Parameters:
 * - pty: A pointer to a character array containing the path to the pseudo-terminal
 *        device (typically a slave terminal like /dev/pts/XX). This terminal will
 *        be used for the container's standard streams.
 * - err: A pointer to a libcrun_error_t structure. This structure will be used to
 *        report any errors that occur during the execution of the function.
 *
 * Return:
 * - On success, the function returns 0.
 * - On failure, it returns a negative value and sets the err structure with the
 *   appropriate error information.
 *
 * Description:
 * The function performs the following operations:
 * 1. Opens the pseudo-terminal (pty) with read-write access.
 * 2. Loops through the first three file descriptors (0: stdin, 1: stdout, 2: stderr).
 * 3. Uses dup3 to duplicate the pseudo-terminal file descriptor to stdin, stdout,
 *    and stderr. This effectively redirects the standard streams of the process to
 *    the specified pseudo-terminal.
 * 4. Sets the opened pseudo-terminal as the controlling terminal for the process
 *    using ioctl.
 *
 * Error Handling:
 * - If opening the pseudo-terminal, duplicating the file descriptor, or setting
 *   the controlling terminal fails, the function returns a negative value and
 *   sets the err structure with details about the failure.
 *
 * Note:
 * - The function is specific to the crun runtime and is tailored for setting up
 *   container environments.
 * - The UNLIKELY macro is used to assist the compiler with branch prediction for
 *   error handling paths.  */

static int set_raw (int fd, void **current_status, libcrun_error_t *err) {
    int ret;
    struct termios termios;
    ret = tcgetattr (fd, &termios);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "tcgetattr");
    if (current_status)
    {
        struct terminal_status_s *s = xmalloc (sizeof (*s));
        s->fd = fd;
        memcpy (&(s->termios), &termios, sizeof (termios));
        *current_status = s;
    }
    cfmakeraw (&termios);
    termios.c_iflag &= OPOST;
    termios.c_oflag &= OPOST;
    ret = tcsetattr (fd, TCSANOW, &termios);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "tcsetattr");
    return 0;
}

/** Sets up the standard I/O (stdin, stdout, stderr) for a container by duplicating the file descriptor of a given pseudo-terminal.
 *
 * @param pty Path to the pseudo-terminal which will be used for standard I/O.
 * @param err Pointer to a libcrun_error_t structure to capture any error information.
 *
 * @return 0 on success, or a negative value on failure. On failure, the `err` structure is filled with details about the error. */
int libcrun_set_stdio (char *pty, libcrun_error_t *err) {
    int ret, i;
    cleanup_close int fd = open (pty, O_RDWR);

    if (UNLIKELY (fd < 0))
        return crun_make_error (err, errno, "open `%s`", pty);

    for (i = 0; i < 3; i++)
    {
        ret = dup3 (fd, i, 0);
        if (UNLIKELY (ret < 0))
            return crun_make_error (err, errno, "dup terminal");
    }

    ret = ioctl (0, TIOCSCTTY, 0);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "ioctl TIOCSCTTY");

    return 0;
}
/** Sets up the terminal associated with a given file descriptor. It essentially prepares the terminal to be used by the container.
 *
 * @param fd File descriptor of the terminal to be setup.
 * @param current_status Pointer to a pointer where the current terminal status will be saved. This can be used later to restore the terminal state.
 * @param err Pointer to a libcrun_error_t structure to capture any error information.
 *
 * @return 0 on success, or a negative value on failure. On failure, the `err` structure is filled with details about the error. */
int libcrun_setup_terminal_ptmx (int fd, void **current_status, libcrun_error_t *err) {
    int ret;
    struct termios termios;
    ret = tcgetattr (fd, &termios);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "tcgetattr");
    ret = tcsetattr (fd, TCSANOW, &termios);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "tcsetattr");
    return set_raw (0, current_status, err);
}
/** Cleans up and restores the terminal status. This function should be called to clean up resources and restore the
 *  terminal to its original state.
 *
 * @param p Terminal status pointer that needs to be cleaned up.
 */

void cleanup_terminalp(void *p) {
    struct terminal_status_s **s = (struct terminal_status_s **) p;
    if (*s)
    {
        tcsetattr ((*s)->fd, TCSANOW, &(*s)->termios);
        free (*s);
    }
}
/** Sets up the size of the terminal associated with a given file descriptor. This function is used to define the dimensions of the terminal.
 *
 * @param fd File descriptor of the terminal to be resized.
 * @param rows Number of rows for the terminal window size.
 * @param cols Number of columns for the terminal window size.
 * @param err Pointer to a libcrun_error_t structure to capture any error information.
 *
 * @return 0 on success, or a negative value on failure. On failure, the `err` structure is filled with details about
 * the error. If the function is unable to detect the terminal type, it may return 0 without changing the size. */
int libcrun_terminal_setup_size (int fd, unsigned short rows, unsigned short cols, libcrun_error_t *err) {
    struct winsize ws = { .ws_row = rows, .ws_col = cols };
    int ret;

    if (ws.ws_row == 0 || ws.ws_col == 0)
    {
        ret = ioctl (0, TIOCGWINSZ, &ws);
        if (UNLIKELY (ret < 0))
        {
            if (errno == ENOTTY)
                return 0;
            return crun_make_error (err, errno, "ioctl TIOCGWINSZ");
        }
    }

    ret = ioctl (fd, TIOCSWINSZ, &ws);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "ioctl TIOCSWINSZ");
    return 0;
}