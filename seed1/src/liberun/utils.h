//
// Created by omarjarkas on 26/11/23.
//

#ifndef SEED_PROJECT_UTILS_H
#define SEED_PROJECT_UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <argp.h>
#include "error.h"
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stddef.h> // For size_t

#define LIKELY(x) __builtin_expect ((x), 1)
#define UNLIKELY(x) __builtin_expect ((x), 0)

static inline void cleanup_freep(void *p) {
    void **pp = (void **) p;
    free(*pp);
}
#define cleanup_free __attribute__ ((cleanup (cleanup_freep)))


__attribute__ ((malloc)) static inline void *xmalloc(size_t size) {
  void *res = malloc(size);
  if (UNLIKELY(res == NULL))
    OOM();
  return res;
}


__attribute__ ((malloc)) static inline void * xmalloc0(size_t size) {
  void *res = calloc(1, size);
  if (UNLIKELY (res == NULL))
    OOM ();
  return res;
}

static inline char *xstrdup(const char *str) {
  char *ret;
  if (str == NULL)
    return NULL;
  ret = strdup (str);
  if (ret == NULL)
    OOM ();
  return ret;
}


/**
 * Reads input from the user.
 * 
 * @param input A pointer to the buffer where input will be stored.
 * @param size The size of the input buffer.
 * @return 0 on success, -1 on failure (EOF or read error).
 */
int read_input(char *input, size_t size);

/**
 * Parses the input string into arguments.
 * 
 * @param input The input string to parse.
 * @param argv An array of strings to store the parsed arguments.
 * @param argc A pointer to an int to store the count of arguments.
 * @return 0 on success.
 */
int parse_input(char *input, char **argv, int *argc);

#endif //SEED_PROJECT_UTILS_H
