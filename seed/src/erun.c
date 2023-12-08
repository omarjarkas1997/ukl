
#include <stdlib.h>
#include <argp.h>
#include <string.h>

#include "erun.h"
#include "create.h"

static struct crun_global_arguments arguments;

struct commands_s {
    int value;
    const char *name;
    int (*handler) (struct crun_global_arguments *, int, char **, libcrun_error_t *);
};


enum { COMMAND_CREATE = 1000 };

struct commands_s commands[] = { { COMMAND_CREATE, "create", crun_command_create },
                                 { 0, } };

static char doc[] = "\nCOMMANDS:\n"
                    "\tcreate      - create a container\n";

static char args_doc[] = "COMMAND [OPTION...]";

static struct commands_s * get_command (const char *arg) {
    struct commands_s *it;
    for (it = commands; it->value; it++)
        if (strcmp (it->name, arg) == 0)
            return it;
    return NULL;
}

enum {
    OPTION_VERSION = 'v',
    OPTION_VERSION_CAP = 'V',
    OPTION_DEBUG = 1000,
    OPTION_SYSTEMD_CGROUP,
    OPTION_CGROUP_MANAGER,
    OPTION_LOG,
    OPTION_LOG_FORMAT,
    OPTION_ROOT,
    OPTION_ROOTLESS
};


static struct argp_option options[] = { { "debug", OPTION_DEBUG, 0, 0, "produce verbose output", 0 },
                                        { "cgroup-manager", OPTION_CGROUP_MANAGER, "MANAGER", 0, "cgroup manager", 0 },
                                        { "systemd-cgroup", OPTION_SYSTEMD_CGROUP, 0, 0, "use systemd cgroups", 0 },
                                        { "log", OPTION_LOG, "FILE", 0, NULL, 0 },
                                        { "log-format", OPTION_LOG_FORMAT, "FORMAT", 0, NULL, 0 },
                                        { "root", OPTION_ROOT, "DIR", 0, NULL, 0 },
                                        { "rootless", OPTION_ROOT, "VALUE", 0, NULL, 0 },
                                        { "version", OPTION_VERSION, 0, 0, NULL, 0 },
                                        { NULL, OPTION_VERSION_CAP, 0, OPTION_ALIAS, NULL, 0 },
                                        { 0, } };

char *argp_mandatory_argument (char *arg, struct argp_state *state) {
    if (arg)
        return arg;
    return state->argv[state->next++];
}

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    const char *tmp;
    switch (key) {
        case OPTION_DEBUG:
            arguments.debug = true;
            break;
        case OPTION_CGROUP_MANAGER:
            tmp = argp_mandatory_argument (arg, state);
            if (strcmp (tmp, "systemd") == 0)
            {
                arguments.option_force_no_cgroup = false;
                arguments.option_systemd_cgroup = true;
            }
            else if (strcmp (tmp, "cgroupfs") == 0)
            {
                arguments.option_force_no_cgroup = false;
                arguments.option_systemd_cgroup = false;
            }
            else if (strcmp (tmp, "disabled") == 0)
            {
                arguments.option_systemd_cgroup = false;
                arguments.option_force_no_cgroup = true;
            }
            else
            {
                printf("unknown cgroup manager specified");
            }
            break;

        case OPTION_SYSTEMD_CGROUP:
            arguments.option_force_no_cgroup = false;
            arguments.option_systemd_cgroup = true;
            break;

        case OPTION_LOG:
            arguments.log = argp_mandatory_argument (arg, state);
            break;

        case OPTION_LOG_FORMAT:
            arguments.log_format = argp_mandatory_argument (arg, state);
            break;

        case OPTION_ROOT:
            arguments.root = argp_mandatory_argument (arg, state);
            break;

        case OPTION_ROOTLESS:
            /* Ignored.  So that a runc command line won't fail.  */
            break;

        case ARGP_KEY_NO_ARGS:
            printf ("please specify a command");

        case OPTION_VERSION:
        case OPTION_VERSION_CAP:
            printf("1.0.0");
            exit (EXIT_SUCCESS);
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct commands_s *command;

static struct argp argp = { options, parse_opt, args_doc, doc, NULL, NULL, NULL };

int main (int argc, char **argv) {
    char input[1024];
    char *parsed_args[64];
    libcrun_error_t err = NULL;
    int ret, first_argument = 0;

    arguments.argc = 0;
    arguments.argv = NULL;

    while(1) {
        printf("> ");  // Prompt
        if (fgets(input, sizeof(input), stdin) == NULL) break; // Exit on EOF

        // Parse input into arguments
        int argc = 0;
        char *arg = strtok(input, " \n");
        while (arg != NULL && argc < 64) {
            parsed_args[argc++] = arg;
            arg = strtok(NULL, " \n");
        }
        if (argc == 0) continue; // No input

        // Find and execute the command
        command = get_command(parsed_args[0]);
        if (command == NULL) {
            printf("Unknown command: %s\n", parsed_args[0]);
            continue;
        }

        ret = command->handler(&arguments, argc, parsed_args, &err);
        // Handle errors or results

    }
    return ret;
}