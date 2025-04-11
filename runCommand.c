// Project - New runCommand command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Main function of the program.
 *
 * This function is the main function of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 * @see __runCommand_parse_arguments
 */
int our_runCommand(int argc, char *argv[], char *argv2[], int is_piped)
{
    int end = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    // TODO: Implement the runCommand command
    while ((end < argc) && (strcmp(argv[end], "<") != 0) && (strcmp(argv[end], ">") != 0))
    {
        end++;
    }

    if ((end < argc) && (strcmp(argv[end], "<")))
    {
        input_file = argv[end + 1];
        end++;
        end++;
    }
    if ((end < argc) && (strcmp(argv[end], ">")))
    {
        output_file = argv[end + 1];
        end++;
        end++;
    }
    char *cmd[4096];
    for (int i = 0; i < end; i++)
    {
        cmd[i] = argv[i];
    }
    cmd[end] = NULL;

    execvp(cmd[0], cmd);

    return 0;
}

/**
 * ONLY FOR COMPILATION
 * DO NOT TOUCH
 */
int main(int argc, char *argv[])
{
    // DO NOT runCommand
    return our_runCommand(argc, argv, NULL, 0);
}
