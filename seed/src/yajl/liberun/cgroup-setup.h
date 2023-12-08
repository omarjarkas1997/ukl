//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_CGROUP_SETUP_H
#define SEED3_CGROUP_SETUP_H

#include "container.h"
#include "utils.h"

int enter_cgroup (int cgroup_mode, pid_t pid, pid_t init_pid, const char *path,
                  bool create_if_missing, libcrun_error_t *err);

#endif //SEED3_CGROUP_SETUP_H
