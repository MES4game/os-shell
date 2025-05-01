// CShell Project - New chmod command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include "chmod.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>

/**
 * @see fprintf
 */
void __chmod_print_usage(char *program_name)
{
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}

/**
 * @see strcmp, __chmod_print_usage
 */
int __chmod_parse_arguments(int argc, char *argv[], char **mode_arg, char **file_arg)
{
    *mode_arg = NULL;
    *file_arg = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            __chmod_print_usage(argv[0]);
            return -1;
        }
        else if (argv[i][0] == '-' && strlen(argv[i]) > 1)
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
        else if (*mode_arg == NULL)
        {
            *mode_arg = argv[i];
        }
        else if (*file_arg == NULL)
        {
            *file_arg = argv[i];
        }
        else
        {
            fprintf(stderr, "Too many arguments. Only mode and file are required.\n");
            return 1;
        }
    }

    if (*mode_arg == NULL || *file_arg == NULL)
    {
        fprintf(stderr, "Missing mode or file.\n");
        return 1;
    }

    return 0;
}

/**
 * @see __chmod_parse_arguments
 */
int our_chmod(int argc, char *argv[])
{
    char *mode_str;
    char *filename;

    // Parse the arguments
    int parse_result = __chmod_parse_arguments(argc, argv, &mode_str, &filename);
    if (parse_result == -1)
        return 0;
    if (parse_result)
        return parse_result;

    // TODO: Implement the chmod command
    char *endptr;
    long mode = strtol(mode_str, &endptr, 8);
    if (*endptr != '\0')
    {
        fprintf(stderr, "Invalid mode: %s\n", mode_str);
        return 1;
    }
    if (chmod(filename, (mode_t)mode) != 0)
    {
        fprintf(stderr, "Error changing mode of file '%s': %s\n", filename, strerror(errno));
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
    return our_chmod(argc, argv);
}
#endif
