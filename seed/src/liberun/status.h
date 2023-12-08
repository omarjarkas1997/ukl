//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_STATUS_H
#define SEED3_STATUS_H

#include "runtime_spec_schema_config_schema.h"
#include "error.h"
#include "container.h"

struct libcrun_container_list_s {
    struct libcrun_container_list_s *next;
    char *name;
};
typedef struct libcrun_container_list_s libcrun_container_list_t;

struct libcrun_container_status_s {
    pid_t pid;
    unsigned long long process_start_time;
    char *bundle;
    char *rootfs;
    char *cgroup_path;
    char *scope;
    char *intelrdt;
    int systemd_cgroup;
    char *created;
    int detached;
    char *external_descriptors;
    char *owner;
};
typedef struct libcrun_container_status_s libcrun_container_status_t;

void libcrun_free_container_status (libcrun_container_status_t *status);
int libcrun_write_container_status (const char *state_root, const char *id,
                                                   libcrun_container_status_t *status, libcrun_error_t *err);
int libcrun_read_container_status (libcrun_container_status_t *status, const char *state_root,
                                                  const char *id, libcrun_error_t *err);
void libcrun_free_containers_list (libcrun_container_list_t *list);
int libcrun_is_container_running (libcrun_container_status_t *status, libcrun_error_t *err);
char *libcrun_get_state_directory (const char *state_root, const char *id);
int libcrun_container_delete_status (const char *state_root, const char *id, libcrun_error_t *err);
int libcrun_get_containers_list (libcrun_container_list_t **ret, const char *state_root,
                                                libcrun_error_t *err);

int libcrun_status_check_directories (const char *state_root, const char *id, libcrun_error_t *err);
int libcrun_status_create_exec_fifo (const char *state_root, const char *id, libcrun_error_t *err);
int libcrun_status_write_exec_fifo (const char *state_root, const char *id, libcrun_error_t *err);
int libcrun_status_has_read_exec_fifo (const char *state_root, const char *id, libcrun_error_t *err);
int libcrun_check_pid_valid (libcrun_container_status_t *status, libcrun_error_t *err);

static inline void libcrun_free_container_listp(void *p) {
    libcrun_container_list_t **l = (libcrun_container_list_t **) p;
    if (*l != NULL)
        libcrun_free_containers_list (*l);
}

#define cleanup_container_status __attribute__ ((cleanup (libcrun_free_container_status)))
#define cleanup_container_list __attribute__ ((cleanup (libcrun_free_container_listp)))


#endif //SEED3_STATUS_H
