#ifndef LIBERUN_H
#define LIBERUN_H

#include <stdarg.h>   // For va_list, va_start, va_end
#include <unistd.h>   // For related POSIX functionalities
#include <string.h>   // For string related functions
#include <time.h>     // For time related functions
#include <sys/time.h> // For additional time and date structures
#include <stdio.h>    // For standard I/O
#include <stdbool.h>  // For boolean type
#include <stdlib.h>   // For exit, EXIT_FAILURE


struct liberun_error_s {
    int status;
    char *msg;
};
typedef struct liberun_error_s *liberun_error_t;


void liberun_fail_with_error(int errno_, const char *msg, ...) __attribute__ ((noreturn)) __attribute__ ((format (printf, 2, 3)));

int liberun_make_error(liberun_error_t *err, int status, const char *msg, ...);

#define erun_make_error liberun_make_error

#define OOM()                            \
  do                                     \
    {                                    \
      fprintf (stderr, "out of memory"); \
      _exit (EXIT_FAILURE);              \
  } while (0)


#endif /* LIBERUN_H */
