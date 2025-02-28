//
// Created by omarjarkas on 10/11/23.
//

#ifndef SEED3_ERROR_H
#define SEED3_ERROR_H

#include <stdbool.h>
#ifdef HAVE_ERROR_H
#  include <error.h>
#else
#  define error(status, errno, fmt, ...)                      \
    do                                                        \
      {                                                       \
        if (errno == 0)                                       \
          fprintf (stderr, "crun: " fmt "\n", ##__VA_ARGS__); \
        else                                                  \
          {                                                   \
            fprintf (stderr, "crun: " fmt, ##__VA_ARGS__);    \
            fprintf (stderr, ": %s\n", strerror (errno));     \
          }                                                   \
        if (status)                                           \
          exit (status);                                      \
    } while (0)
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <syslog.h>
#include <unistd.h>

struct libcrun_error_s
{
    int status;
    char *msg;
};
typedef struct libcrun_error_s *libcrun_error_t;

#define OOM()                            \
  do                                     \
    {                                    \
      fprintf (stderr, "out of memory"); \
      _exit (EXIT_FAILURE);              \
  } while (0)

typedef void (*crun_output_handler) (int errno_, const char *msg, bool warning, void *arg);

void crun_set_output_handler (crun_output_handler handler, void *arg, bool log_to_stderr);

void log_write_to_journald (int errno_, const char *msg, bool warning, void *arg);

void log_write_to_syslog (int errno_, const char *msg, bool warning, void *arg);

void log_write_to_stream (int errno_, const char *msg, bool warning, void *arg);

void log_write_to_stderr (int errno_, const char *msg, bool warning, void *arg);

int crun_error_wrap (libcrun_error_t *err, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

int crun_error_get_errno (libcrun_error_t *err);

int crun_error_release (libcrun_error_t *err);

void crun_error_write_warning_and_release (FILE *out, libcrun_error_t **err);

void libcrun_warning (const char *msg, ...) __attribute__ ((format (printf, 1, 2)));

void libcrun_error (int errno_, const char *msg, ...) __attribute__ ((format (printf, 2, 3)));

int libcrun_make_error (libcrun_error_t *err, int status, const char *msg, ...) __attribute__ ((format (printf, 3, 4)));

#define crun_make_error libcrun_make_error

void libcrun_error_write_warning_and_release (FILE *out, libcrun_error_t **err);

void libcrun_fail_with_error (int errno_, const char *msg, ...) __attribute__ ((noreturn)) __attribute__ ((format (printf, 2, 3)));

int libcrun_set_log_format (const char *format, libcrun_error_t *err);

int libcrun_init_logging (crun_output_handler *output_handler, void **output_handler_arg, const char *id,
                                         const char *log, libcrun_error_t *err);

int libcrun_error_release (libcrun_error_t *err);

int yajl_error_to_crun_error (int yajl_status, libcrun_error_t *err);

enum
{
    LIBCRUN_VERBOSITY_ERROR,
    LIBCRUN_VERBOSITY_WARNING,
};

void libcrun_set_verbosity (int verbosity);
int libcrun_get_verbosity ();

#endif //SEED3_ERROR_H
