
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#include "erun.h"


int crun_command_create (struct crun_global_arguments *global_args, int argc, char **argv, libcrun_error_t *err) {
    printf("Hello World1!\n");
    return 0;
}


