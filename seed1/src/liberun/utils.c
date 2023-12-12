
#include "utils.h"



// Function to read input from the user
int read_input(char *input, size_t size) {
    printf("erun> ");
    if (!fgets(input, size, stdin)) {
        // Handle EOF or read error
        return -1;
    }
    // Remove newline character from input
    input[strcspn(input, "\n")] = 0;
    return 0;
}

// Function to parse input into arguments
int parse_input(char *input, char **argv, int *argc) {
    *argc = 0;
    char *token = strtok(input, " ");
    while (token != NULL && *argc < 63) {
        argv[(*argc)++] = token;
        token = strtok(NULL, " ");
    }
    argv[*argc] = NULL;
    return 0;
}