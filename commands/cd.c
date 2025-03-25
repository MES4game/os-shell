// Project - New cd command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>

#define MAXPATHLEN 4096

char __cd__cwd[MAXPATHLEN]; // current working directory
char __cd__pwd[MAXPATHLEN]; // previous working directory

/**
 * @brief Print the usage of the program.
 *
 * This function prints the usage of the program.
 *
 * @param program_name The name of the program.
 * @return
 * @see fprintf
 */
void __cd_print_usage(char *program_name)
{
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}

/**
 * @brief Parse the arguments of the program.
 *
 * This function parses the arguments of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return
 * @see strcmp, __cd_print_usage, exit
 */
void __cd_parse_arguments(int argc, char *argv[])
{
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            // Print the usage and exit
            __cd_print_usage(argv[0]);
            exit(0);
        }
    }
}

/**
 * @brief Main function of the program.
 *
 * This function is the main function of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 * @see __cd_parse_arguments
 */
int our_cd(int argc, char *argv[])
{
    // Parse the arguments
    __cd_parse_arguments(argc, argv);

    // TODO: Implement the cd command
    char *path = argv[1];
    if (argc == 1 || ((argc == 2) && (strcmp(argv[1], "~") == 0)))
    {
        path = getenv("HOME");
        if (path == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }

    if ((argc == 2) && strcmp(argv[1], "-") == 0)
    {

        if (__cd__pwd[0] == '\0')
        {
            fprintf(stderr, "cd: no previous directory\n");
            return 1;
        }
        path = __cd__pwd;
    }

    char tmp[MAXPATHLEN];

    if (getcwd(tmp, sizeof(tmp)) == NULL)
    {
        fprintf(stderr, "cd: error getting current working directory\n");
        return 1;
    }

    if (chdir(path) == 0)
    {
        if (getcwd(__cd__cwd, sizeof(__cd__cwd)) != NULL)
        {
            // printf("Current working directory: %s\n", __cd__cwd);
            strcpy(__cd__pwd, tmp);
        }
        else
        {
            fprintf(stderr, "cd: error getting current working directory\n");
            return 1;
        }
    }
    else
    {
        perror("cd error");
    }

    return 0;
}

int test()
{
    printf("=========Test=========\n");

    assert(__cd__cwd[0] == '\0');
    assert(__cd__pwd[0] == '\0');

    printf("cwd: %s\n", __cd__cwd);

    char *argv1[] = {"cd", "test"};
    our_cd(2, argv1);
    printf("cwd: %s\n", __cd__cwd);

    char *argv3[] = {"cd", "-"};
    our_cd(2, argv3);
    printf("cwd: %s\n", __cd__cwd);

    our_cd(2, argv3);
    printf("cwd: %s\n", __cd__cwd);

    char *argv[] = {"cd", ".."};
    our_cd(2, argv);
    printf("cwd: %s\n", __cd__cwd);

    char *argv2[] = {"cd", "~"};
    our_cd(1, argv2);
    printf("cwd: %s\n", __cd__cwd);

    printf("Success!\n");
    return 0;
}

/**
 * ONLY FOR COMPILATION
 * DO NOT TOUCH
 */
int main(int argc, char *argv[])
{
    // DO NOT TOUCH
    return test();
    // return our_cd(argc, argv);
}
