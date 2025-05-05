// CShell Project - New cat command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include "cat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * @see fprintf
 */
void _cat_print_usage(const char *const program_name)
{
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}

/**
 * @see strcmp, __cat_print_usage
 */
int _cat_parse_arguments(const int argc, const char *const *const argv)
{
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            // Print the usage and return
            _cat_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}

/**
 * @see __cat_parse_arguments
 */
int our_cat(const int argc, const char *const *const argv)
{
    // Parse the arguments
    int parse_result = _cat_parse_arguments(argc, argv);
    if (parse_result == -1)
        return 0;
    if (parse_result)
        return parse_result;

    // TODO: Implement the cat command

    // Check if there are any files to read else read form stdin
    if (argc == 1)
    {
        int character;
        while ((character = getchar()) != EOF)
        {
            putchar(character);
        }
        return 0;
    }

    // Loop through each file and print its contents
    for (int i = 1; i < argc; i++)
    {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL)
        {
            fprintf(stderr, "Error opening file '%s': %s\n", argv[i], strerror(errno));
            continue;
        }
        int character;
        while ((character = fgetc(file)) != EOF)
        {
            putchar(character);
        }
        fclose(file);
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
    return our_cat(argc, argv);
}
#endif
