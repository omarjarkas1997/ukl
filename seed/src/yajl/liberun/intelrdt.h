//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_INTELRDT_H
#define SEED3_INTELRDT_H

#include <stdio.h>
#include <stdbool.h>
#include "error.h"

int resctl_create (const char *name, bool explicit_clos_id, bool *created, const char *l3_cache_schema, const char *mem_bw_schema, libcrun_error_t *err);
int resctl_move_task_to (const char *name, pid_t pid, libcrun_error_t *err);
int resctl_update (const char *name, const char *l3_cache_schema, const char *mem_bw_schema, libcrun_error_t *err);
int resctl_destroy (const char *name, libcrun_error_t *err);

#endif //SEED3_INTELRDT_H
