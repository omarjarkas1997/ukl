//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_CGROUP_UTILS_H
#define SEED3_CGROUP_UTILS_H

#include "container.h"
#include "cgroup.h"
#include <unistd.h>

int libcrun_move_process_to_cgroup (pid_t pid, pid_t init_pid, char *path, libcrun_error_t *err);

int libcrun_cgroups_create_symlinks (int dirfd, libcrun_error_t *err);

int libcrun_get_current_unified_cgroup (char **path, bool absolute, libcrun_error_t *err);

int libcrun_get_cgroup_mode (libcrun_error_t *err);

int libcrun_get_cgroup_dirfd (struct libcrun_cgroup_status *status, const char *sub_cgroup, libcrun_error_t *err);

int maybe_make_cgroup_threaded (const char *path, libcrun_error_t *err);

#endif //SEED3_CGROUP_UTILS_H
