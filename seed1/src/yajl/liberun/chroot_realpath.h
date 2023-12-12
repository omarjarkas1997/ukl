//
// Created by omarjarkas on 13/11/23.
//

#ifndef SEED3_CHROOT_REALPATH_H
#define SEED3_CHROOT_REALPATH_H

#include <sys/types.h>  // Required for size_t type

char *chroot_realpath(const char *chroot, const char *path, char resolved_path[]);


#endif //SEED3_CHROOT_REALPATH_H
