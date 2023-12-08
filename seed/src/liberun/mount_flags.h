//
// Created by omarjarkas on 15/11/23.
//

#ifndef SEED3_MOUNT_FLAGS_H
#define SEED3_MOUNT_FLAGS_H

enum
{
    OPTION_TMPCOPYUP = (1 << 0),
    OPTION_RECURSIVE = (1 << 1),
    OPTION_IDMAP = (1 << 2),
    OPTION_COPY_SYMLINK = (1 << 3),
};

struct propagation_flags_s
{
    char *name;
    int clear;
    int flags;
    int extra_flags;
};

const struct propagation_flags_s *libcrun_str2mount_flags (const char *name);
const struct propagation_flags_s *get_mount_flags_from_wordlist ();


#endif //SEED3_MOUNT_FLAGS_H
