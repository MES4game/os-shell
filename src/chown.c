// CShell Project - New chown command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "chown.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>


/**
 * @see printf
 */
void _chown_print_usage(const char *const program_name) {
    printf("Usage: %s [Options]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
}


/**
 * @see strcmp, _chown_print_usage
 */
int _chown_parse_arguments(const int argc, const char *const *const argv) {
    if (argc < 3) {
        perror("Error: Not enough arguments");
        _chown_print_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            _chown_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}


/**
 * @see _chown_parse_arguments
 */
int our_chown(const int argc, const char *const *const argv) {
    // Parse the arguments
    int parse_result = _chown_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result) return parse_result;

    const char *const owner_group = argv[1];
    const char *const owner = strtok((char *)owner_group, ":");
    const char *const group = strtok(NULL, ":");

    // Get the provided owner
    struct passwd *pwd = getpwnam(owner);
    if (!pwd) {
        perror("Error: User not found");
        return -1;
    }

    uid_t uid = pwd->pw_uid;
    gid_t gid = -1;

    // Get the provided group
    if (group) {
        struct group *grp = getgrnam(group);
        if (!grp) {
            perror("Error: Group not found");
            return 1;
        }
        gid = grp->gr_gid;
    }

    // Loop through each file and change ownership
    for (int i = 2; i < argc; i++) {
        const char *const file = argv[i];
        if (chown(file, uid, gid) != 0) {
            perror("Error: Failed to change ownership");
            return 1;
        }
        printf("Changed ownership of '%s' to '%s:%s'\n", file, owner, group ? group : "none");
    }

    return 0;
}


#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_chown(argc, argv);
}
#endif
