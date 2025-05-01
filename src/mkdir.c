// CShell Project - New mkdir command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

// #include "mkdir.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

/**
 * @see fprintf
 */
void __mkdir_print_usage(char *program_name)
{
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}

/**
 * @see strcmp, __mkdir_print_usage
 */
int __mkdir_parse_arguments(int argc, char *argv[])
{
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            // Print the usage and return
            __mkdir_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}

/**
 * @see __mkdir_parse_arguments
 */
int our_mkdir(int argc, char *argv[])
{
    // Parse the arguments
    int parse_result = __mkdir_parse_arguments(argc, argv);
    if (parse_result == -1)
        return 0;
    if (parse_result)
        return parse_result;

    // TODO: Implement the mkdir command

    const char *dir_name = argv[1];
    int status = mkdir(dir_name, 0755);

    if (status == 0)
    {
        printf("Directory '%s' created successfully.\n", dir_name);
    }
    else
    {
        fprintf(stderr, "Error creating directory '%s': %s\n", dir_name, strerror(errno));
        return 1;
    }

    return 0;
}

#ifndef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[])
{
    // DO NOT TOUCH
    return our_mkdir(argc, argv);
}
#endif
