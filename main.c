#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


// Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


// Debug flag to print debug messages
int DEBUG = 0;

// Hidden flag to allow secret files/folders
int HIDDEN = 0;

/**
 * @brief Parse the line to get the command and the arguments.
 */
void parse_command(char *command, int *argc, char *argv[])
{
    // TODO
}

/**
 * @brief Call the appropriate function with the arguments.
 */
int call_command(int argc, char *argv[])
{
    // TODO
    return 0;
}

/**
 * @brief Print the usage of the program.
 *
 * This function prints the usage of the program.
 *
 * @param program_name The name of the program.
 * @return
 * @see printf
 */
void print_usage(char *program_name)
{
    fprintf(stdout, "Usage: %s [-d] [-a]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -v    Verbose, debug mode\n");
    fprintf(stdout, "    -a    allow secret files/folders\n");
}

/**
 * @brief Parse the arguments of the program.
 *
 * This function parses the arguments of the program and sets the DEBUG and HIDDEN flags.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return
 * @see strcmp, isdigit, atoi, exit, print_usage, fprintf
 */
void parse_arguments(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -v
        if (strcmp(argv[i], "-v") == 0)
        {
            // Set DEBUG
            DEBUG = 1;
            continue;
        }

        // Check if the argument is -a
        if (strcmp(argv[i], "-a") == 0)
        {
            // Set HIDDEN
            HIDDEN = 1;
            continue;
        }
    }
}

/**
 * @brief Main function of the program.
 *
 * This function is the main function of the program.
 * It parses the arguments and do an interactive session as a CShell.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 * @see parse_arguments, fprintf
 */
int main(int argc, char *argv[])
{
    // Parse the arguments
    parse_arguments(argc, argv);

    int res;
    uid_t uid;
    struct passwd *pw;
    char cwd[1024];
    while (1)
    {
        // Print shell prompt
        uid = getuid();
        pw = getpwuid(uid);
        fprintf(stdout, "CShell(%s)-%s> ", (pw ? pw->pw_name : "no user"), getcwd(cwd, sizeof(cwd)) != NULL ? cwd : "no directory");

        // Read the command line
        char command[1024];
        // TODO: Read the command line

        // Parse and call the command
        int n_argc = 0;
        char *n_argv[1024];
        parse_command(command, &n_argc, n_argv);
        res = call_command(n_argc, n_argv);
        if (res != 0)
            break;
    }

    return res;
}
