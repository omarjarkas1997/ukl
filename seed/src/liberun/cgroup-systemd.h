//
// Created by omarjarkas on 15/11/23.
//

#ifndef CGROUP_SYSTEMD_H
#define CGROUP_SYSTEMD_H

#include "container.h"
#include <unistd.h>

#ifdef HAVE_SYSTEMD
extern int parse_sd_array (char *s, char **out, char **next, libcrun_error_t *err);

extern int cpuset_string_to_bitmask (const char *str, char **out, size_t *out_size, libcrun_error_t *err);

extern char *get_cgroup_scope_path (const char *cgroup_path, const char *scope);
#endif

extern struct libcrun_cgroup_manager cgroup_manager_systemd;

#endif