//
// Created by omarjarkas on 10/11/23.
//

#include "container.h"
#include "utils.h"
#define YAJL_STR(x) ((const unsigned char *) (x))


enum {
    SYNC_SOCKET_SYNC_MESSAGE,
    SYNC_SOCKET_ERROR_MESSAGE,
    SYNC_SOCKET_WARNING_MESSAGE,
};


struct container_entrypoint_s {
    libcrun_container_t *container;
    libcrun_context_t *context;
    int has_terminal_socket_pair;
    int terminal_socketpair[2];
    int sync_socket;
    int seccomp_fd;
    int seccomp_receiver_fd;
    int console_socket_fd;
    int hooks_out_fd;
    int hooks_err_fd;
    struct custom_handler_instance_s *custom_handler;
};


/** @brief Structure for sync socket messages in crun.
 *
 * This structure represents the format of messages sent over synchronization sockets
 * within the crun container runtime. These messages are used for various types of
 * inter-process communications, such as error reporting and status updates. */
struct sync_socket_message_s {
    int type;
    int error_value;
    char message[512];
};


typedef runtime_spec_schema_defs_hook hook;


/** @brief List of OCI runtime hooks supported by crun.
 *
 * This array of strings represents different types of OCI runtime hooks that can be
 * utilized by crun during the lifecycle of a container. Each element corresponds to a
 * specific stage in the container lifecycle where custom actions can be executed.
 */
char *hooks[] = {
        "prestart",
        "createRuntime",
        "createContainer",
        "startContainer",
        "poststart",
        "poststop"
};


/**  @brief List of Linux namespaces supported by crun.
 *
 * This array of strings represents different types of Linux namespaces that can be
 * utilized by crun to isolate containerized processes. Each element corresponds to a
 * specific namespace type, providing a distinct aspect of isolation. */
static char *namespaces[] = {
        "cgroup",
        "ipc",
        "mount",
        "network",
        "pid",
        "user",
        "uts"
};

/** @brief List of seccomp actions supported by crun.
 *
 * This array of strings represents the different actions that can be taken by the
 * seccomp policy in crun. Each action corresponds to a specific behavior when a
 * system call is made by a containerized process.  */
static char *actions[] = {
        "SCMP_ACT_ALLOW",
        "SCMP_ACT_ERRNO",
        "SCMP_ACT_KILL",
        "SCMP_ACT_KILL_PROCESS",
        "SCMP_ACT_KILL_THREAD",
        "SCMP_ACT_LOG",
        "SCMP_ACT_NOTIFY",
        "SCMP_ACT_TRACE",
        "SCMP_ACT_TRAP"
};

/** @brief Seccomp comparison operators for crun.
 *
 * This file contains the definition of comparison operators used in seccomp rules within
 * the crun container runtime. Seccomp is a Linux kernel feature that allows a process to
 * specify a filter for incoming system calls. These operators are used to build conditions
 * for these filters. */
static char *operators[] = {
        "SCMP_CMP_NE",
        "SCMP_CMP_LT",
        "SCMP_CMP_LE",
        "SCMP_CMP_EQ",
        "SCMP_CMP_GE",
        "SCMP_CMP_GT",
        "SCMP_CMP_MASKED_EQ",
};

/**  @brief List of CPU architectures supported by crun's seccomp configuration.
 *
 * This array of strings represents the different CPU architectures for which crun
 * can define seccomp policies. Each element corresponds to a specific architecture,
 * allowing crun to tailor security policies according to the architecture of the
 * container's environment. */
static char *archs[] = {
        "SCMP_ARCH_AARCH64",      // ARM 64-bit architecture
        "SCMP_ARCH_ARM",          // ARM 32-bit architecture
        "SCMP_ARCH_MIPS",         // MIPS architecture
        "SCMP_ARCH_MIPS64",       // MIPS 64-bit architecture
        "SCMP_ARCH_MIPS64N32",    // MIPS 64-bit with 32-bit pointers
        "SCMP_ARCH_MIPSEL",       // MIPS little-endian architecture
        "SCMP_ARCH_MIPSEL64",     // MIPS 64-bit little-endian architecture
        "SCMP_ARCH_MIPSEL64N32",  // MIPS 64-bit little-endian with 32-bit pointers
        "SCMP_ARCH_PPC",          // PowerPC architecture
        "SCMP_ARCH_PPC64",        // PowerPC 64-bit architecture
        "SCMP_ARCH_PPC64LE",      // PowerPC 64-bit little-endian architecture
        "SCMP_ARCH_RISCV64",      // RISC-V 64-bit architecture
        "SCMP_ARCH_S390",         // IBM S/390 architecture
        "SCMP_ARCH_S390X",        // IBM S/390x architecture
        "SCMP_ARCH_X32",          // x86 32-bit architecture (with 64-bit features)
        "SCMP_ARCH_X86",          // x86 32-bit architecture
        "SCMP_ARCH_X86_64"        // x86 64-bit architecture
};


/** @brief Default OCI runtime specification in JSON format.
 *
 * This constant string defines the default OCI runtime specification for containers
 * created by crun. It includes configuration for various aspects of a container:
 *
 * - "ociVersion": Specifies the OCI runtime spec version.
 * - "process": Configures various process-related parameters like terminal, user, args, env.
 * - "root": Defines the container's root filesystem.
 * - "hostname": Sets the container's hostname.
 * - "mounts": Lists filesystem mounts for the container.
 * - "linux": Specifies Linux-specific configurations like resource limits, namespaces,
 *   masked paths, and readonly paths.
 *
 * Each section is essential for setting up the container environment according to OCI standards.
 */

static const char spec_file[] = "\
{\n\
	\"ociVersion\": \"1.0.0\",\n\
	\"process\": {\n\
		\"terminal\": true,\n\
		\"user\": {\n\
			\"uid\": 0,\n\
			\"gid\": 0\n\
		},\n\
		\"args\": [\n\
			\"sh\"\n\
		],\n\
		\"env\": [\n\
			\"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin\",\n\
			\"TERM=xterm\"\n\
		],\n\
		\"cwd\": \"/\",\n\
		\"capabilities\": {\n\
			\"bounding\": [\n\
				\"CAP_AUDIT_WRITE\",\n\
				\"CAP_KILL\",\n\
				\"CAP_NET_BIND_SERVICE\"\n\
			],\n\
			\"effective\": [\n\
				\"CAP_AUDIT_WRITE\",\n\
				\"CAP_KILL\",\n\
				\"CAP_NET_BIND_SERVICE\"\n\
			],\n\
			\"inheritable\": [\n\
			],\n\
			\"permitted\": [\n\
				\"CAP_AUDIT_WRITE\",\n\
				\"CAP_KILL\",\n\
				\"CAP_NET_BIND_SERVICE\"\n\
			],\n\
			\"ambient\": [\n\
				\"CAP_AUDIT_WRITE\",\n\
				\"CAP_KILL\",\n\
				\"CAP_NET_BIND_SERVICE\"\n\
			]\n\
		},\n\
		\"rlimits\": [\n\
			{\n\
				\"type\": \"RLIMIT_NOFILE\",\n\
				\"hard\": 1024,\n\
				\"soft\": 1024\n\
			}\n\
		],\n\
		\"noNewPrivileges\": true\n\
	},\n\
	\"root\": {\n\
		\"path\": \"rootfs\",\n\
		\"readonly\": true\n\
	},\n\
	\"hostname\": \"crun\",\n\
	\"mounts\": [\n\
		{\n\
			\"destination\": \"/proc\",\n\
			\"type\": \"proc\",\n\
			\"source\": \"proc\"\n\
		},\n\
		{\n\
			\"destination\": \"/dev\",\n\
			\"type\": \"tmpfs\",\n\
			\"source\": \"tmpfs\",\n\
			\"options\": [\n\
				\"nosuid\",\n\
				\"strictatime\",\n\
				\"mode=755\",\n\
				\"size=65536k\"\n\
			]\n\
		},\n\
		{\n\
			\"destination\": \"/dev/pts\",\n\
			\"type\": \"devpts\",\n\
			\"source\": \"devpts\",\n\
			\"options\": [\n\
				\"nosuid\",\n\
				\"noexec\",\n\
				\"newinstance\",\n\
				\"ptmxmode=0666\",\n\
				\"mode=0620\"\
%s\
			]\n\
		},\n\
		{\n\
			\"destination\": \"/dev/shm\",\n\
			\"type\": \"tmpfs\",\n\
			\"source\": \"shm\",\n\
			\"options\": [\n\
				\"nosuid\",\n\
				\"noexec\",\n\
				\"nodev\",\n\
				\"mode=1777\",\n\
				\"size=65536k\"\n\
			]\n\
		},\n\
		{\n\
			\"destination\": \"/dev/mqueue\",\n\
			\"type\": \"mqueue\",\n\
			\"source\": \"mqueue\",\n\
			\"options\": [\n\
				\"nosuid\",\n\
				\"noexec\",\n\
				\"nodev\"\n\
			]\n\
		},\n\
		{\n\
			\"destination\": \"/sys\",\n\
			\"type\": \"sysfs\",\n\
			\"source\": \"sysfs\",\n\
			\"options\": [\n\
				\"nosuid\",\n\
				\"noexec\",\n\
				\"nodev\",\n\
				\"ro\"\n\
			]\n\
		},\n\
		{\n\
			\"destination\": \"/sys/fs/cgroup\",\n\
			\"type\": \"cgroup\",\n\
			\"source\": \"cgroup\",\n\
			\"options\": [\n\
				\"nosuid\",\n\
				\"noexec\",\n\
				\"nodev\",\n\
				\"relatime\",\n\
				\"ro\"\n\
			]\n\
		}\n\
	],\n\
	\"linux\": {\n\
		\"resources\": {\n\
			\"devices\": [\n\
				{\n\
					\"allow\": false,\n\
					\"access\": \"rwm\"\n\
				}\n\
			]\n\
		},\n\
		\"namespaces\": [\n\
			{\n\
				\"type\": \"pid\"\n\
			},\n\
			{\n\
				\"type\": \"network\"\n\
			},\n\
			{\n\
				\"type\": \"ipc\"\n\
			},\n\
			{\n\
				\"type\": \"uts\"\n\
			},\n\
%s\
%s\
			{\n\
				\"type\": \"mount\"\n\
			}\n\
		],\n\
		\"maskedPaths\": [\n\
			\"/proc/acpi\",\n\
			\"/proc/asound\",\n\
			\"/proc/kcore\",\n\
			\"/proc/keys\",\n\
			\"/proc/latency_stats\",\n\
			\"/proc/timer_list\",\n\
			\"/proc/timer_stats\",\n\
			\"/proc/sched_debug\",\n\
			\"/sys/firmware\",\n\
			\"/proc/scsi\"\n\
		],\n\
		\"readonlyPaths\": [\n\
			\"/proc/bus\",\n\
			\"/proc/fs\",\n\
			\"/proc/irq\",\n\
			\"/proc/sys\",\n\
			\"/proc/sysrq-trigger\"\n\
		]\n\
	}\n\
}\n";




/** @brief TTY group setting for the container's pseudoterminal.
 *
 * This string literal defines the group ID (GID) for the pseudoterminal device ('/dev/pts')
 * inside the container. Setting a GID is essential for proper permissions and access control
 * to the TTY device within the container environment.
 *
 * Example usage in JSON:
 * "options": [
 *   "gid=5"
 * ]
 */
static const char *spec_pts_tty_group = ",\n\
				\"gid=5\"\n";


/** @brief User namespace configuration for the container.
 *
 * This string segment specifies the use of a user namespace for the container. User namespaces
 * provide isolation of user IDs and group IDs, which is a key aspect of container security. This
 * allows for user ID mappings between the host and the container, enhancing isolation.
 *
 * Example usage in JSON:
 * "namespaces": [
 *   {
 *     "type": "user"
 *   }
 * ]
 */
static const char *spec_user = "\
			{\n\
				\"type\": \"user\"\n\
			},\n";

/** @brief Cgroup namespace configuration for the container.
 *
 * This string segment specifies the use of a cgroup namespace. Cgroup namespaces virtualize the
 * view of a container's cgroup hierarchy. This isolation is crucial for resource management and
 * limiting the container's view of the cgroup hierarchy on the host system.
 *
 * Example usage in JSON:
 * "namespaces": [
 *   {
 *     "type": "cgroup"
 *   }
 * ]
 */
static const char *spec_cgroupns = "\
			{\n\
				\"type\": \"cgroup\"\n\
			},\n";
/** @brief Calculate the byte length of a sync socket message up to a specified length of the message field.
 *
 * This macro calculates the size of a 'sync_socket_message_s' structure up to a given number of bytes
 * in the 'message' field. It is useful for determining the actual size of the structure when only a
 * portion of the 'message' field is used, optimizing memory usage and network transmission.
 *
 * @param x The 'sync_socket_message_s' structure.
 * @param l The length of the 'message' field to consider in the calculation.
 * @return The size of the structure up to the specified point in the 'message' field.
 *
 * Example Usage:
 * size_t message_size = SYNC_SOCKET_MESSAGE_LEN(message_struct, actual_message_length);
 */
#define SYNC_SOCKET_MESSAGE_LEN(x, l) (offsetof (struct sync_socket_message_s, message) + l)


/** @brief Write a message to a synchronization socket.
 *
 * This function sends a message through a given synchronization socket file descriptor.
 * It is used for logging and error reporting from within the containerized process.
 *
 * @param fd The file descriptor of the synchronization socket.
 * @param warning A boolean flag indicating if the message is a warning (true) or an error (false).
 * @param err_value An integer representing an error value or code.
 * @param log_msg The message to be sent through the socket.
 *
 * @return Returns 0 on successful write, -1 on failure, and 0 if the file descriptor is invalid (less than 0).
 *
 * The function sets up a 'sync_socket_message_s' structure with the message type, error value,
 * and the actual log message. It ensures the message length does not exceed the buffer size,
 * and then sends the message via the specified file descriptor. */
static int sync_socket_write_msg(int fd, bool warning, int err_value, const char *log_msg) {
    int ret;
    size_t err_len;
    struct sync_socket_message_s msg;
    msg.type = warning ? SYNC_SOCKET_WARNING_MESSAGE : SYNC_SOCKET_ERROR_MESSAGE;
    msg.error_value = err_value;
    if (fd < 0)
        return 0;
    err_len = strlen (log_msg);
    if (err_len >= sizeof (msg.message))
        err_len = sizeof (msg.message) - 1;
    memcpy (msg.message, log_msg, err_len);
    msg.message[err_len] = '\0';
    ret = TEMP_FAILURE_RETRY(write(fd, &msg, SYNC_SOCKET_MESSAGE_LEN (msg, err_len + 1)));
    if (UNLIKELY (ret < 0))
        return -1;
    return 0;
}

/** @brief Write an error message to a synchronization socket.
 *
 * This function sends an error message, encapsulated within a 'libcrun_error_t' structure,
 * through a synchronization socket file descriptor.
 *
 * @param fd The file descriptor of the synchronization socket.
 * @param out_err Pointer to a 'libcrun_error_t' structure containing the error details.
 *
 * @return Returns 0 if the file descriptor is invalid (less than 0) or on successful write,
 *         and the return value of 'sync_socket_write_msg' on failure.
 *
 * The function delegates the actual writing to the 'sync_socket_write_msg' function after
 * extracting the necessary information from the 'libcrun_error_t' structure. */
static int sync_socket_write_error(int fd, libcrun_error_t *out_err) {
    if (fd < 0)
        return 0;
    return sync_socket_write_msg(fd, false, (*out_err)->status, (*out_err)->msg);
}

/** @brief Write a log message to a synchronization socket.
 *
 * This function is responsible for sending log messages to a synchronization socket associated
 * with a container. If writing to the synchronization socket fails, it writes the log message
 * to standard error instead.
 *
 * @param errno_ The error number associated with the log message.
 * @param msg The log message to be written.
 * @param warning A boolean indicating if the log message is a warning (true) or an error (false).
 * @param arg A pointer to an instance of 'struct container_entrypoint_s' containing the sync socket file descriptor.
 *
 * The function first extracts the file descriptor of the sync socket from the 'container_entrypoint_s' structure.
 * If the file descriptor is valid, it attempts to write the message to the sync socket. In case of a failure
 * in writing to the sync socket, it falls back to writing the message to standard error. */
static void log_write_to_sync_socket (int errno_, const char *msg, bool warning, void *arg) {
    struct container_entrypoint_s *entrypoint_args = arg;
    int fd = entrypoint_args->sync_socket;
    if (fd < 0)
        return;
    if (sync_socket_write_msg (fd, warning, errno_, msg) < 0)
        log_write_to_stderr (errno_, msg, warning, arg);
}

/** @brief Checks if the memory limit of a container is too low.
 *
 * This function examines the memory limit specified in the container's runtime specification
 * and determines whether it is below an acceptable threshold.
 *
 * @param def Pointer to the container's runtime specification structure.
 * @return Returns true if the memory limit is too low, false otherwise.
 *
 * The function defines a threshold for the minimum acceptable memory limit. It then checks
 * if the container's configuration specifies a memory limit below this threshold. The check
 * is performed both for the traditional 'memory' limit setting and the 'unified' cgroup
 * settings, ensuring compatibility with different configuration styles. */
static bool is_memory_limit_too_low (runtime_spec_schema_config_schema *def) {
    const long memory_limit_too_low = 1024 * 1024;

    if (def->linux == NULL || def->linux->resources == NULL)
        return false;

    if (def->linux->resources->memory
        && def->linux->resources->memory->limit_present
        && def->linux->resources->memory->limit < memory_limit_too_low)
        return true;

    if (def->linux->resources->unified)
    {
        size_t i;

        for (i = 0; i < def->linux->resources->unified->len; i++)
            if (strcmp (def->linux->resources->unified->keys[i], "memory.max") == 0) {
                long limit;
                errno = 0;
                limit = strtol (def->linux->resources->unified->values[i], NULL, 10);
                if (errno != 0)
                    return false;
                if (limit < memory_limit_too_low)
                    return true;
            }
    }
    return false;
}




/** @brief Wait for and handle messages from a synchronization socket.
 *
 * This function reads messages from a specified synchronization socket and handles them
 * according to their types. It is primarily used to wait for a specific synchronization
 * message indicating a certain state or to process error and warning messages.
 *
 * @param context The context containing the output handler function and its argument.
 * @param fd The file descriptor of the synchronization socket.
 * @param flush A boolean flag indicating whether to flush the socket (ignore messages).
 * @param err Pointer to a libcrun_error_t structure for error reporting.
 *
 * @return Returns 0 on successful handling of a sync message, -1 on error,
 *         and 0 if the file descriptor is invalid (less than 0).
 *
 * The function continuously reads from the socket in a loop. If it encounters an error
 * or warning message, it handles them appropriately, either logging them or reporting
 * an error. If a synchronization message is received, the function returns successfully. */
static int sync_socket_wait_sync (libcrun_context_t *context, int fd, bool flush, libcrun_error_t *err) {
    struct sync_socket_message_s msg;
    if (fd < 0)
        return 0;
    while (true) {
        int ret;
        errno = 0;
        ret = TEMP_FAILURE_RETRY (read (fd, &msg, sizeof (msg)));
        if (UNLIKELY (ret < 0)) {
            if (flush)
                return 0;
            return crun_make_error (err, errno, "read from sync socket");
        }
        if (ret == 0) {
            if (flush)
                return 0;
            return crun_make_error (err, errno, "read from the init process");
        }
        if (! flush && msg.type == SYNC_SOCKET_SYNC_MESSAGE)
            return 0;
        if (msg.type == SYNC_SOCKET_WARNING_MESSAGE) {
            if (context)
                context->output_handler (msg.error_value, msg.message, 1, context->output_handler_arg);
            continue;
        }
        if (msg.type == SYNC_SOCKET_ERROR_MESSAGE)
            return crun_make_error (err, msg.error_value, "%s", msg.message);
    }
}
/**
 * @brief Send a synchronization message through a socket.
 *
 * This function is responsible for sending a synchronization message through the
 * specified file descriptor. It is primarily used for synchronizing states between
 * the crun runtime and container processes.
 *
 * @param fd The file descriptor of the synchronization socket.
 * @param flush_errors A boolean indicating whether to flush errors from the socket.
 * @param err A pointer to a libcrun_error_t structure for error reporting.
 *
 * @return Returns 0 on successful sync message transmission, or a negative value on error.
 *
 * The function initializes a 'sync_socket_message_s' structure for the synchronization message,
 * writes it to the provided file descriptor, and handles any errors that might occur during
 * the write operation. If 'flush_errors' is true and a write error occurs, it attempts to
 * read an error message from the socket and populate the provided 'err' structure.
 */
static int sync_socket_send_sync(int fd, bool flush_errors, libcrun_error_t *err) {
    int ret;
    struct sync_socket_message_s msg = { 0, };
    msg.type = SYNC_SOCKET_SYNC_MESSAGE;

    if (fd < 0) return 0;

    ret = TEMP_FAILURE_RETRY(write(fd, &msg, SYNC_SOCKET_MESSAGE_LEN (msg, 0)));
    if (UNLIKELY (ret < 0)) {
        if (flush_errors) {
            int saved_errno = errno;
            ret = TEMP_FAILURE_RETRY(read (fd, &msg, sizeof (msg)));
            if (ret >= 0 && msg.type == SYNC_SOCKET_ERROR_MESSAGE)
                return crun_make_error(err, msg.error_value, "%s", msg.message);
            errno = saved_errno;
        }
        return crun_make_error(err, errno, "write to sync socket");
    }
    return 0;
}

/** @brief Create and initialize a new container.
 *
 * This function allocates and initializes a new `libcrun_container_t` structure
 * that represents a container. It sets up the container's definition, configuration,
 * and host UID/GID based on the provided parameters.
 *
 * @param container_def A pointer to a `runtime_spec_schema_config_schema` structure
 *        representing the container's OCI runtime specification.
 * @param path The file path to the container's configuration file (optional).
 * @param config The content of the container's configuration (optional).
 *
 * @return A pointer to the newly created `libcrun_container_t` structure.
 *
 * The function allocates memory for the container structure and initializes its fields.
 * It retrieves the effective user ID (UID) and group ID (GID) of the host process
 * and stores them in the container structure. If provided, the paths and contents of
 * the container's configuration are also stored. */
static libcrun_container_t *
make_container (runtime_spec_schema_config_schema *container_def, const char *path, const char *config)
{
    // Allocate memory for the container structure
    libcrun_container_t *container = xmalloc0 (sizeof (*container));

    // Set the container definition
    container->container_def = container_def;

    // Retrieve and set the host process's UID and GID
    container->host_uid = geteuid ();
    container->host_gid = getegid ();

    // Duplicate and set the path to the container's configuration file, if provided
    if (path)
        container->config_file = xstrdup (path);

    // Duplicate and set the content of the container's configuration, if provided
    if (config)
        container->config_file_content = xstrdup (config);

    return container;
}

/** @brief Load a container definition from a JSON string.
 *
 * This function parses a container definition provided as a JSON string and creates
 * a 'libcrun_container_t' object representing the container. It utilizes the OCI runtime
 * specification schema for parsing the JSON data.
 *
 * @param json A string containing the container's JSON definition.
 * @param err A pointer to a 'libcrun_error_t' structure for error handling.
 *
 * @return On success, returns a pointer to the 'libcrun_container_t' object.
 *         On failure, returns NULL and sets the 'err' object with an appropriate error message.
 *
 * The function begins by parsing the JSON string using the OCI runtime spec schema parser.
 * If the parsing fails, an error is recorded and the function returns NULL. Otherwise,
 * it proceeds to create and return a 'libcrun_container_t' object based on the parsed data. */
libcrun_container_t * libcrun_container_load_from_memory (const char *json, libcrun_error_t *err) {
    runtime_spec_schema_config_schema *container_def;
    cleanup_free char *oci_error = NULL;

    // Parse the JSON data into a runtime spec schema structure
    container_def = runtime_spec_schema_config_schema_parse_data (json, NULL, &oci_error);

    // If parsing fails, record the error and return NULL
    if (container_def == NULL)
    {
        crun_make_error (err, 0, "load: `%s`", oci_error);
        return NULL;
    }

    // Create and return a libcrun_container_t object based on the parsed data
    return make_container (container_def, NULL, json);
}

/** @brief Load a container configuration from a file.
 *
 * This function parses a container configuration file (in OCI runtime spec format) and creates
 * a new `libcrun_container_t` instance based on this configuration.
 *
 * @param path The file path of the container configuration file.
 * @param err Pointer to a `libcrun_error_t` structure for error reporting.
 *
 * @return Returns a pointer to a `libcrun_container_t` instance on success, or NULL on failure.
 *
 * The function uses `runtime_spec_schema_config_schema_parse_file` to parse the OCI runtime spec
 * from the given file. In case of failure, it sets the error using `crun_make_error` and returns NULL.
 * On success, it creates and returns a `libcrun_container_t` instance using `make_container`. */
libcrun_container_t * libcrun_container_load_from_file (const char *path, libcrun_error_t *err) {
    runtime_spec_schema_config_schema *container_def;
    cleanup_free char *oci_error = NULL;
    container_def = runtime_spec_schema_config_schema_parse_file (path, NULL, &oci_error);
    if (container_def == NULL) {
        crun_make_error (err, 0, "load `%s`: %s", path, oci_error);
        return NULL;
    }
    return make_container (container_def, path, NULL);
}

/** @brief Frees resources associated with a libcrun_container_t object.
 *
 * This function is responsible for deallocating and freeing all resources associated
 * with a 'libcrun_container_t' object. It performs a series of conditional checks
 * and frees various elements within the structure.
 *
 * @param ctr Pointer to the 'libcrun_container_t' object to be freed.
 *
 * The function first checks if the provided pointer is NULL. If it is not, it proceeds
 * to free resources including any private data (using a cleanup callback if provided),
 * the container definition structure, and strings related to configuration files.
 * Finally, it frees the 'libcrun_container_t' object itself. */
void libcrun_container_free (libcrun_container_t *ctr) {
    // Check if the container pointer is NULL
    if (ctr == NULL)
        return;

    // Clean up private data if a cleanup function is provided
    if (ctr->cleanup_private_data)
        ctr->cleanup_private_data (ctr->private_data);

    // Free the container definition structure
    if (ctr->container_def)
        free_runtime_spec_schema_config_schema (ctr->container_def);

    // Free strings related to configuration files
    free (ctr->config_file_content);
    free (ctr->config_file);

    // Finally, free the container structure itself
    free (ctr);
}


/** @brief Block all signals for the calling thread.
 *
 * This function configures the signal mask to block all signals for the thread that calls it.
 * This is typically used to prevent the thread from being interrupted by signals that are meant
 * to be handled elsewhere or at a different time.
 *
 * @param err A pointer to a 'libcrun_error_t' structure for error reporting.
 *
 * @return Returns 0 on success, or a negative value on failure. In case of failure, an error
 *         message is set in the 'err' structure.
 *
 * The function uses 'sigfillset' to fill the signal set with all signals, and then applies
 * this set to the current thread using 'sigprocmask'. If an error occurs, it is captured and
 * reported through the 'err' parameter. */
static int block_signals (libcrun_error_t *err) {
    int ret;
    sigset_t mask;

    // Fill the signal set with all signals
    sigfillset (&mask);

    // Block all signals for the current thread
    ret = sigprocmask (SIG_BLOCK, &mask, NULL);

    // Check for errors in signal blocking
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "sigprocmask");

    return 0;
}

/** @brief Unblock all signals and set their action to the default.
 *
 * This function unblocks all signals for the current process and sets their action
 * to the default behavior. It ensures that the process is responsive to the standard
 * set of signals as defined in the system.
 *
 * @param err Pointer to a 'libcrun_error_t' structure to capture any error information.
 *
 * @return Returns 0 on success, or a negative value on failure. In case of failure,
 *         the 'err' object is populated with error details.
 *
 * The function first unblocks all signals using 'sigprocmask' and then iterates through
 * all signals defined in 'NSIG', resetting their action to the default using 'sigaction'.
 * It handles possible errors and reports them through the 'err' parameter. */
static int unblock_signals (libcrun_error_t *err) {
    int i;
    int ret;
    sigset_t mask;
    struct sigaction act = {};

    // Fill the signal mask with all signals
    sigfillset (&mask);

    // Unblock all signals
    ret = sigprocmask (SIG_UNBLOCK, &mask, NULL);
    if (UNLIKELY (ret < 0))
        return crun_make_error (err, errno, "sigprocmask");

    // Set signal handler to default for all signals
    act.sa_handler = SIG_DFL;
    for (i = 0; i < NSIG; i++) {
        // Reset the action for each signal
        ret = sigaction (i, &act, NULL);
        // Handle possible errors, except for invalid signals
        if (ret < 0 && errno != EINVAL)
            return crun_make_error (err, errno, "sigaction");
    }
    return 0;
}

/** @brief Initialize security settings for a containerized process.
 *
 * This function sets up various security-related aspects for a containerized process
 * managed by crun. It involves initializing AppArmor profiles, SELinux contexts,
 * and capabilities. Each of these components enhances the security isolation of
 * the containerized process.
 *
 * @param proc A pointer to the process structure containing security configuration.
 * @param err A pointer to a 'libcrun_error_t' structure for error reporting.
 *
 * @return Returns 0 on success, or a negative value on failure. If 'proc' is NULL,
 *         the function will return 0 as there is no process to initialize security for.
 *
 * The function sequentially initializes AppArmor (if an AppArmor profile is specified),
 * SELinux, and capabilities. If any of these steps fails, the function returns immediately
 * with the corresponding error code. */
//static int initialize_security(runtime_spec_schema_config_schema_process *proc, libcrun_error_t *err) {
//    int ret;
//
//    // Check for NULL process, which means no initialization is needed
//    if (UNLIKELY (proc == NULL))
//        return 0;
//
//    // Initialize AppArmor if a profile is specified
//    if (proc->apparmor_profile) {
//        ret = libcrun_initialize_apparmor(err);
//        if (UNLIKELY (ret < 0))
//            return ret;
//    }
//
//    // Initialize SELinux
//    ret = libcrun_initialize_selinux(err);
//    if (UNLIKELY (ret < 0))
//        return ret;
//
//    // Initialize capabilities
//    ret = libcrun_init_caps(err);
//    if (UNLIKELY (ret < 0))
//        return ret;
//
//    return 0;
//}
