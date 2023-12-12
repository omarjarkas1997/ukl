

#ifndef CRIU_H
#define CRIU_H

#include "container.h"
#include "error.h"
#include "utils.h"

#if HAVE_CRIU && HAVE_DLOPEN

int libcrun_container_checkpoint_linux_criu (libcrun_container_status_t *status, libcrun_container_t *container,
                                             libcrun_checkpoint_restore_t *cr_options, libcrun_error_t *err);

int libcrun_container_restore_linux_criu (libcrun_container_status_t *status, libcrun_container_t *container,
                                          libcrun_checkpoint_restore_t *cr_options, libcrun_error_t *err);

#else

static inline int libcrun_container_checkpoint_linux_criu (arg_unused libcrun_container_status_t *status,
                                         arg_unused libcrun_container_t *container,
                                         arg_unused libcrun_checkpoint_restore_t *cr_options, libcrun_error_t *err) {
    return crun_make_error (err, 0, "compiled without CRIU support. Checkpointing not available");
}

static inline int libcrun_container_restore_linux_criu (arg_unused libcrun_container_status_t *status,
                                      arg_unused libcrun_container_t *container,
                                      arg_unused libcrun_checkpoint_restore_t *cr_options, libcrun_error_t *err) {
    return crun_make_error (err, 0, "compiled without CRIU support. Restore not available");
}

#endif
#endif