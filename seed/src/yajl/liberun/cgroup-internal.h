//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_CGROUP_INTERNAL_H
#define SEED3_CGROUP_INTERNAL_H

#include "container.h"
#include "utils.h"

/** identify and manage different aspects of system resources */
enum {
    CGROUP_MEMORY = 1 << 0,
    CGROUP_CPU = 1 << 1,
    CGROUP_HUGETLB = 1 << 2,
    CGROUP_CPUSET = 1 << 3,
    CGROUP_PIDS = 1 << 4,
    CGROUP_IO = 1 << 5,
};

/** Holds the status of a cgroup, including its path, scope, and manager type */
struct libcrun_cgroup_status {
    char *path;
    char *scope;
    int manager;
};

struct libcrun_cgroup_manager {
    /* Create a new cgroup and fill PATH in OUT.  */
    int (*create_cgroup) (struct libcrun_cgroup_args *args, struct libcrun_cgroup_status *out, libcrun_error_t *err);
    int (*precreate_cgroup) (struct libcrun_cgroup_args *args, int *dirfd, libcrun_error_t *err);
    /* Destroy the cgroup and kill any process if needed.  */
    int (*destroy_cgroup) (struct libcrun_cgroup_status *cgroup_status, libcrun_error_t *err);
    /* Additional resources configuration specific to this manager.  */
    int (*update_resources) (struct libcrun_cgroup_status *cgroup_status, runtime_spec_schema_config_linux_resources *resources, libcrun_error_t *err);
};

int move_process_to_cgroup (pid_t pid, const char *subsystem, const char *path, libcrun_error_t *err);
int enter_cgroup_subsystem (pid_t pid, const char *subsystem, const char *path, bool create_if_missing,
                            libcrun_error_t *err);
int enable_controllers (const char *path, libcrun_error_t *err);
int chown_cgroups (const char *path, uid_t uid, gid_t gid, libcrun_error_t *err);
int destroy_cgroup_path (const char *path, int mode, libcrun_error_t *err);
int cgroup_killall_path (const char *path, int signal, libcrun_error_t *err);

int libcrun_cgroup_read_pids_from_path (const char *path, bool recurse, pid_t **pids, libcrun_error_t *err);

bool read_proc_cgroup (char *content, char **saveptr, char **id, char **controller_list, char **path);

static inline int is_rootless(libcrun_error_t *err) {
    if (geteuid ())
        return 1;
    return check_running_in_user_namespace (err);
}

int libcrun_cgroup_pause_unpause_path (const char *cgroup_path, const bool pause, libcrun_error_t *err);

static inline uint64_t convert_shares_to_weight (uint64_t shares) {
    /* convert linearly from 2-262144 to 1-10000.  */
    return (1 + ((shares - 2) * 9999) / 262142);
}

int initialize_cpuset_subsystem (const char *path, libcrun_error_t *err);

int write_cpuset_resources (int dirfd_cpuset, int cgroup2, runtime_spec_schema_config_linux_resources_cpu *cpu, libcrun_error_t *err);

int write_cpu_burst (int cpu_dirfd, bool cgroup2, runtime_spec_schema_config_linux_resources_cpu *cpu, libcrun_error_t *err);

#endif //SEED3_CGROUP_INTERNAL_H
