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
 * @see fprintf
 */
void _chown_print_usage(const char *const program_name)
{
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}

/**
 * @see strcmp, __chown_print_usage
 */
int _chown_parse_arguments(const int argc, const char *const *const argv)
{
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            // Print the usage and return
            _chown_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}

/**
 * @see __chown_parse_arguments
 */
int our_chown(const int argc, const char *const *const argv)
{
    // Parse the arguments
    int parse_result = _chown_parse_arguments(argc, argv);
    if (parse_result == -1)
        return 0;
    if (parse_result)
        return parse_result;
    if (argc < 3)
    {
        fprintf(stderr, "Error: Not enough arguments\n");
        _chown_print_usage(argv[0]);
        return -1;
    }

    const char *const owner_group = argv[1];
    const char *const owner = strtok((char *)owner_group, ":");
    const char *const group = strtok(NULL, ":");

    struct passwd *pwd = getpwnam(owner);
    if (!pwd)
    {
        fprintf(stderr, "Error: User '%s' not found\n", owner);
        return -1;
    }
    uid_t uid = pwd->pw_uid;
    gid_t gid = -1;

    if (group)
    {
        struct group *grp = getgrnam(group);
        if (!grp)
        {
            fprintf(stderr, "Error: Group '%s' not found\n", group);
            return -1;
        }
        gid = grp->gr_gid;
    }

    for (int i = 2; i < argc; i++)
    {
        const char *const file = argv[i];
        if (chown(file, uid, gid) != 0)
        {
            fprintf(stderr, "Error: Failed to change ownership of '%s': %s\n", file, strerror(errno));
            return -1;
        }
        fprintf(stdout, "Changed ownership of '%s' to '%s:%s'\n", file, owner, group ? group : "none");
    }

    return 0;
}

#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[])
{
    // DO NOT TOUCH
    return our_chown(argc, argv);
}
#endif
