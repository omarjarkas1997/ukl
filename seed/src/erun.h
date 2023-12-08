#ifndef SEED_ERUN_H
#define SEED_ERUN_H

#include <stdbool.h>
#include "liberun/container.h"


struct crun_global_arguments {
    char *root;
    char *log;
    char *log_format;
    const char *handler;

    int argc;
    char **argv;

    bool command;
    bool debug;
    bool option_systemd_cgroup;
    bool option_force_no_cgroup;
};

#endif // SEED_ERUN_H
