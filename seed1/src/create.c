//
// Created by omarjarkas on 26/11/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include <limits.h> 
#include "liberun/error.h"
#include "liberun/utils.h"
#include "container.h"

enum
{
    PATH = 1000,
};

static const char *bundle = NULL;
static const char *path = NULL;
static const char *hostname = NULL;
static const char *config_file = "config.json";

static liberun_context_t erun_context;

static struct argp_option options[] = {{"bundle", 'b', "DIR", 0, "container bundle (default \".\")", 0},
                                       {"config", 'f', "FILE", 0, "override the config file name", 0},
                                       {"hostname", 'h', "HOSTNAME", 0, "container hostname", 0},
                                       {
                                           0,
                                       }};
static char doc[] = "OCI runtime";
static char args_doc[] = "create [OPTION]... CONTAINER";

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key)
    {
    case 'b':
        bundle = erun_context.bundle = argp_mandatory_argument(arg, state);
        break;
    case 'f':
        config_file = argp_mandatory_argument(arg, state);
        break;
    case 'h':
        hostname = argp_mandatory_argument(arg, state);
        break;
    case ARGP_KEY_NO_ARGS:
        liberun_fail_with_error(0, "please specify a ID for the container");
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp run_argp = {options, parse_opt, args_doc, doc, NULL, NULL, NULL};

int erun_command_create(struct erun_global_arguments *global_args, int argc, char **argv, liberun_error_t *err)
{
    int first_arg = 0, ret;
    cleanup_container liberun_container_t *container = NULL;
    cleanup_free char *bundle_cleanup = NULL;
    cleanup_free char *config_file_cleanup = NULL;

    argp_parse(&run_argp, argc, argv, ARGP_IN_ORDER, &first_arg, &erun_context);

    
    /* Make sure the config is an absolute path before changing the directory.  */
    if (strcmp("config.json", config_file) != 0)
    {
        printf("Custom config file specified: %s\n", config_file);
        if (config_file[0] != '/')
        {
            printf("Resolving the absolute path for the config file: %s\n", config_file);
            config_file_cleanup = realpath(config_file, NULL);
            if (config_file_cleanup == NULL)
            {
                int err = errno; // Store errno immediately, as it can be overwritten by subsequent calls
                fprintf(stderr, "Error resolving realpath for `%s`: %s\n", config_file, strerror(err));
                liberun_fail_with_error(err, "realpath `%s` failed", config_file);
            }
            else
            {
                printf("Resolved config file path: %s\n", config_file_cleanup);
                config_file = config_file_cleanup;
            }
        }
        else
        {
            printf("Config file `%s` is an absolute path, no resolution needed.\n", config_file);
        }
    }

    printf("Hello World from inside erun!");

    return 0;
}