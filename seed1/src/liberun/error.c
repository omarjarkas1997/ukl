
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "error.h"

void liberun_fail_with_error(int errno_, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
    va_end(args);
    if (errno_ != 0) {
        perror("Error");
    }
    exit(EXIT_FAILURE);
}

int liberun_make_error(liberun_error_t *err, int status, const char *msg, ...) {
  va_list args_list;
  liberun_error_t ptr;

  va_start (args_list, msg);
  *err = xmalloc (sizeof (struct liberun_error_s));
  ptr = *err;
  ptr->status = status;
  if (vasprintf (&(ptr->msg), msg, args_list) < 0)
    OOM ();
  va_end (args_list);

  return -status - 1;
}