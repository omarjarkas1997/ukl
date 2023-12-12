//
// Created by omarjarkas on 26/11/23.
//

#include "container.h"

#include "runtime_spec/runtime_spec_schema_config_schema.h"
#include "liberun/utils.h"

void liberun_container_free(liberun_container_t *ctr) {
  if (ctr == NULL)
    return;

  if (ctr->container_def)
    free_runtime_spec_schema_config_schema(ctr->container_def);

  free (ctr->config_file_content);
  free (ctr->config_file);
  free (ctr);
}


static liberun_container_t *make_container(runtime_spec_schema_config_schema *container_def, const char *path, const char *config) {
  liberun_container_t *container = xmalloc0(sizeof(*container));
  container->container_def = container_def;
  container->host_gid = geteuid();
  container->host_gid = getegid();
  if (path) 
    container->config_file = xstrdup(path);
  if (config)
    container->config_file_content = xstrdup(config);
  return container;
}

liberun_container_t *liberun_container_load_from_file(const char *path, liberun_error_t *err) {
  runtime_spec_schema_config_schema *container_def;
  cleanup_free char *oci_error = NULL;
  container_def = runtime_spec_schema_config_schema_parse_file (path, NULL, &oci_error);
  if (container_def == NULL)
    {
      erun_make_error(err, 0, "load `%s`: %s", path, oci_error);
      return NULL;
    }
  return make_container(container_def, path, NULL);
}

static int check_config_file(runtime_spec_schema_config_schema *def, liberun_context_t *context, liberun_container_t *err) {
  if (UNLIKELY(def->oci_version && strstr(def->oci_version, "1.0") == NULL))
    return erun_make_error(err, 0, "unkown version specified");
  if (UNLIKELY(def->root == NULL))
    return erun_make_error(err, 0, "invalid config file, no `root` block specified");
  if (UNLIKELY(def->mounts == NULL))
    return erun_make_error(err, 0, "invalid config file, no `mounts` block specified");
  return 0;
}


int liberun_container_create(liberun_context_t *context, liberun_container_t *container, unsigned int options, liberun_error_t *err) {
  int ret;
  context->detach = 1; // run in the background, detach from terminal
  container->context = context; // set container context

  
  ret = check_config_file(container->container_def, context, err);
  if(UNLIKELY(ret < 0)) return ret;


  return 0;
}
