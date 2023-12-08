
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>  // Include for PATH_MAX
#include <dirent.h> // Include for directory handling
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <linux/capability.h>



// #define CGROUP_FS_TYPE "cgroup2"
// #define MOUNT_POINT "/sys/fs/cgroup/unified"
// #define CGROUP_DIR MOUNT_POINT "/my_cgroup"


// // Function to mount the cgroup filesystem
// int mount_cgroup_fs() {
//     struct stat st = {0};

    
//     if (stat("/sys/fs/cgroup", &st) == -1 || !S_ISDIR(st.st_mode)) {
//         fprintf(stderr, "/sys/fs/cgroup does not exist\n");
//         return -1;
//     }

//     if (stat(MOUNT_POINT, &st) == -1) {
//         if (mkdir(MOUNT_POINT, 0755) != 0) {
//             perror("mkdir");
//             return -1;
//         }
//         if (mount("none", MOUNT_POINT, CGROUP_FS_TYPE, 0, NULL) != 0) {
//             perror("mount cgroup2");
//             return -1;
//         }
//     }
//     return 0;
// }

// int create_cgroup() {
//     if (mkdir(CGROUP_DIR, 0755) != 0) {
//         perror("mkdir cgroup");
//         return -1;
//     }
//     return 0;
// }

// int add_process_to_cgroup() {
//     FILE *fp = fopen(CGROUP_DIR "/cgroup.procs", "w");
//     if (fp == NULL) {
//         perror("fopen");
//         return -1;
//     }

//     fprintf(fp, "%d", getpid());
//     fclose(fp);

//     return 0;
// }


int main() {
    // if (mount_cgroup_fs() != 0) {
    //     fprintf(stderr, "Failed to mount cgroup filesystem\n");
    //     return EXIT_FAILURE;
    // }

    // if (create_cgroup() != 0) {
    //     fprintf(stderr, "Failed to create cgroup\n");
    //     return EXIT_FAILURE;
    // }

    // if (add_process_to_cgroup() != 0) {
    //     fprintf(stderr, "Failed to add process to cgroup\n");
    //     return EXIT_FAILURE;
    // }

    printf("Hello World from inside UKL\n");

    for (;;)
        pause(); // Infinite loop to keep the parent process alive

    exit(EXIT_SUCCESS);
}
