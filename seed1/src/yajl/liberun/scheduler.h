//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_SCHEDULER_H
#define SEED3_SCHEDULER_H
#include "error.h"
#include "container.h"
#include "status.h"

int libcrun_set_scheduler (pid_t pid, runtime_spec_schema_config_schema_process *process, libcrun_error_t *err);

#endif //SEED3_SCHEDULER_H
