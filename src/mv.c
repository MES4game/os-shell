// CShell Project - New mv command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include "mv.h"
#include "cp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * @see fprintf
 */
void __mv_print_usage(char *program_name)
{
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}

/**
 * @see strcmp, __mv_print_usage
 */
int __mv_parse_arguments(int argc, char *argv[])
{
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            // Print the usage and return
            __mv_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}

/**
 * @see __mv_parse_arguments
 */
int our_mv(int argc, char *argv[])
{
    // Parse the arguments
    int parse_result = __mv_parse_arguments(argc, argv);
    if (parse_result == -1)
        return 0;
    if (parse_result)
        return parse_result;

    // TODO: Implement the mv command
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }

    char *source = argv[1];
    char *destination = argv[2];

    if (rename(source, destination) == 0)
    {
        printf("Moved '%s' to '%s'\n", source, destination);
        return 0;
    }

    // If rename fails, try to copy and then delete the source
    if (our_cp(argc, argv) != 0)
    {
        fprintf(stderr, "Error: Failed to copy '%s' to '%s'\n", source, destination);
        return 1;
    }

    if (unlink(source) != 0)
    {
        fprintf(stderr, "Error: Failed to delete source file '%s'\n", source);
        return 1;
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
    return our_mv(argc, argv);
}
#endif
