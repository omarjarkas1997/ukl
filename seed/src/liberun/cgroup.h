//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_CGROUP_H
#define SEED3_CGROUP_H

#include "container.h"
#include <unistd.h>

#ifndef CGROUP_ROOT
#  define CGROUP_ROOT "/sys/fs/cgroup"
#endif

#ifndef PROC_SELF_CGROUP
#  define PROC_SELF_CGROUP "/proc/self/cgroup"
#endif

enum
{
    CGROUP_MODE_UNIFIED = 1,
    CGROUP_MODE_LEGACY,
    CGROUP_MODE_HYBRID
};

enum
{
    CGROUP_MANAGER_CGROUPFS = 1,
    CGROUP_MANAGER_SYSTEMD,
    CGROUP_MANAGER_DISABLED
};

struct libcrun_cgroup_status;

struct libcrun_cgroup_args
{
    runtime_spec_schema_config_linux_resources *resources;
    json_map_string_string *annotations;
    const char *cgroup_path;
    int manager;
    pid_t pid;
    uid_t root_uid;
    gid_t root_gid;
    const char *id;
    bool joined;
};

/* cgroup life-cycle management.  */
int libcrun_cgroup_preenter (struct libcrun_cgroup_args *args, int *dirfd, libcrun_error_t *err);
int libcrun_cgroup_enter (struct libcrun_cgroup_args *args, struct libcrun_cgroup_status **out, libcrun_error_t *err);
int libcrun_cgroup_enter_finalize (struct libcrun_cgroup_args *args, struct libcrun_cgroup_status *cgroup_status, libcrun_error_t *err);
int libcrun_cgroup_destroy (struct libcrun_cgroup_status *cgroup_status, libcrun_error_t *err);

/* Handle the cgroup status.  */
int libcrun_cgroup_get_status (struct libcrun_cgroup_status *cgroup_status, libcrun_container_status_t *status,
                               libcrun_error_t *err);
void libcrun_cgroup_status_free (struct libcrun_cgroup_status *cgroup_status);

struct libcrun_cgroup_status *libcrun_cgroup_make_status (libcrun_container_status_t *status);

static inline void
cgroup_status_freep (struct libcrun_cgroup_status **p)
{
    struct libcrun_cgroup_status *s = *p;
    if (s)
        libcrun_cgroup_status_free (s);
}
#define cleanup_cgroup_status __attribute__ ((cleanup (cgroup_status_freep)))

/* Operations on the cgroup.  */

int libcrun_cgroup_killall (struct libcrun_cgroup_status *cgroup_status, int signal, libcrun_error_t *err);

int libcrun_cgroup_has_oom (struct libcrun_cgroup_status *status, libcrun_error_t *err);

int libcrun_cgroup_read_pids (struct libcrun_cgroup_status *status, bool recurse, pid_t **pids, libcrun_error_t *err);

int libcrun_update_cgroup_resources (struct libcrun_cgroup_status *status,
                                     runtime_spec_schema_config_linux_resources *resources,
                                     libcrun_error_t *err);

int libcrun_cgroup_is_container_paused (struct libcrun_cgroup_status *status, bool *paused, libcrun_error_t *err);

int libcrun_cgroup_pause_unpause (struct libcrun_cgroup_status *status, const bool pause, libcrun_error_t *err);


#endif //SEED3_CGROUP_H
