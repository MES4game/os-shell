// Project - New splitCommand command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "runCommand.c"

/**
 * @brief Main function of the program.
 *
 * This function is the main function of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 * @see __splitCommand_parse_arguments
 */
int our_splitCommand(int argc, char *argv[])
{
    // TODO: Implement the splitCommand command
    int start = 0, end = 0;
    int return_code;
    char *piped[4096];

    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    while (end < argc)
    {
        if (strcmp(argv[end], ";") == 0)
        {
            printf("symbol: ';', start: %d, end: %d\n", start, end);
            // redirect to runCommand
            our_runCommand(end - start, &argv[start], piped, 0);
            start = end + 1;
        }
        if (strcmp(argv[end], "&&") == 0)
        {
            printf("symbol: '&&', start: %d, end: %d\n", start, end);
            // redirect to runCommand
            return_code = our_runCommand(end - start, &argv[start], piped, 0);
            if (return_code == 0)
            {
                start = end + 1;
            }
            else
            {
                while (end < argc)
                {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "||") == 0 || strcmp(argv[end], "|") == 0)
                    {
                        start = end + 1;
                        end++;
                        break;
                    }
                    end++;
                }
            }
        }
        if (strcmp(argv[end], "||") == 0)
        {
            printf("symbol: '||', start: %d, end: %d\n", start, end);
            // redirect to runCommand
            return_code = our_runCommand(end - start, &argv[start], piped, 0);

            if (return_code != 0)
            {
                start = end + 1;
            }
            else
            {
                while (end < argc)
                {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&&") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "|") == 0)
                    {
                        start = end + 1;
                        end++;
                        break;
                    }
                    end++;
                }
            }
        }
        if (strcmp(argv[end], "&") == 0)
        {
            // redirect to runCommand
            printf("symbol: '&', start: %d, end: %d\n", start, end);
            pid_t pid = fork();
            if (pid == 0)
            {
                // Child process
                // redirect to runCommand
                our_runCommand(end - start, &argv[start], piped, 0);
                // wait to result
                exit(0);
            }
            else if (pid < 0)
            {
                // Fork failed
                perror("fork");
                exit(EXIT_FAILURE);
            }

            //
            start = end + 1;
        }
        if (strcmp(argv[end], "|") == 0)
        {
            printf("symbol: '|', start: %d, end: %d\n", start, end);
            printf("TO BE IMPLEMENTED\n");
            // TODO
            // redirect to runCommand
            our_runCommand(end - start, &argv[start], piped, 1);

            start = end + 1;
        }

        end++;
    }

    return 0;
}

/**
 * ONLY FOR COMPILATION
 * DO NOT TOUCH
 */
int main(int argc, char *argv[])
{
    // DO NOT TOUCH
    return our_splitCommand(argc, argv);
}
