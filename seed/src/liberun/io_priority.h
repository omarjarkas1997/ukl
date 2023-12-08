#ifndef SEED3_IO_PRIORITY_H
#define SEED3_IO_PRIORITY_H
#include "error.h"
#include "container.h"
#include "status.h"

int libcrun_set_io_priority(pid_t pid, runtime_spec_schema_config_schema_process *process, libcrun_error_t *err);


#endif //SEED3_IO_PRIORITY_H
