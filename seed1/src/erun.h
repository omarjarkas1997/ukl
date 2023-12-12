#ifndef SEED_ERUN_H
#define SEED_ERUN_H

#include <stdbool.h>

struct erun_global_arguments {
    int argc;
    char **argv;
    bool command;
};

char *argp_mandatory_argument(char *arg, struct argp_state *state);

#endif // SEED_ERUN_H
