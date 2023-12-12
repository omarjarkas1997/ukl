//
// Created by omarjarkas on 26/11/23.
//

#ifndef SEED_PROJECT_CONTAINER_H
#define SEED_PROJECT_CONTAINER_H


#include "runtime_spec/runtime_spec_schema_config_schema.h"
#include "liberun/error.h"
#include <stdbool.h>


struct liberun_context_s {
    const char *state_root;
    const char *id;
    const char *bundle;
    const char *path;
    char **argv;
    int argc;
    bool detach;
};


struct liberun_container_s {
  runtime_spec_schema_config_schema *container_def;

  uid_t host_uid;
  gid_t host_gid;

  uid_t container_uid;
  gid_t container_gid;

  char *config_file;
  char *config_file_content;

  struct liberun_container_s *context;
};

typedef struct liberun_context_s liberun_context_t;
typedef struct liberun_container_s liberun_container_t;
void liberun_container_free(liberun_container_t *ctr);
static inline void cleanup_containerp(liberun_container_t **c) {
  liberun_container_t *container = *c;
  liberun_container_free(container);
}



#define cleanup_container __attribute__ ((cleanup (cleanup_containerp)))




liberun_container_t *liberun_container_load_from_file(const char *path, liberun_error_t *err);

#endif //SEED_PROJECT_CONTAINER_H
