// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

// #include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>

#define MAX_HISTORY 100

char *history[MAX_HISTORY];
int history_count = 0;

int DEBUG = 0;

/**
 * @see sizeof, malloc, isspace, strlen, perror, exit, realloc
 */
void parse_line(const char *line, int *argc, char ***argv)
{
    int capacity = 10;
    *argv = malloc(capacity * sizeof(char *));
    *argc = 0;

    const char *ptr = line;
    char *arg;
    int in_quotes, len;
    while (*ptr)
    {
        while (isspace((unsigned char)*ptr))
            ptr++;

        if (*ptr == '\0')
            break;

        in_quotes = 0;
        len = 0;

        arg = malloc(strlen(ptr) + 1);
        if (!arg)
        {
            perror("malloc");
            exit(1);
        }

        while (*ptr)
        {
            if (*ptr == '\"' || *ptr == '\'')
            {
                in_quotes = !in_quotes;
                ptr++;
                continue;
            }

            if (!in_quotes && isspace((unsigned char)*ptr))
                break;

            arg[len++] = *ptr++;
        }

        arg[len] = '\0';

        if (*argc >= capacity)
        {
            capacity *= 2;
            *argv = realloc(*argv, capacity * sizeof(char *));
            if (!*argv)
            {
                perror("realloc");
                exit(1);
            }
        }

        (*argv)[*argc] = arg;
        (*argc)++;
    }
}

/**
 * @see strcmp, execvp
 */
int call_command(int argc, char *argv[], char *argv2[], int *piped_end, int is_piped)
{
    // TODO: remove this when call_command is implemented, it is just for testing parse_commands
    fprintf(stdout, "call_command: command = %s, #args = %d, args:\n", argv[0], argc - 1);
    for (int i = 1; i < argc; i++)
    {
        fprintf(stdout, "    %d. %s\n", i, argv[i]);
    }
    sleep(2);
    fprintf(stdout, "call_command: Finished call of %s\n", argv[0]);
    fflush(stdout);

    int end = 0;
    char *input_file = NULL;
    char *output_file = NULL;

    while ((end < argc) && (strcmp(argv[end], "<") != 0) && (strcmp(argv[end], ">") != 0))
    {
        end++;
    }

    if ((end < argc) && (strcmp(argv[end], "<")))
    {
        end++;
        input_file = argv[end];
        end++;
    }

    if ((end < argc) && (strcmp(argv[end], ">")))
    {
        end++;
        output_file = argv[end];
        end++;
    }

    char *cmd[4096];
    for (int i = 0; i < end; i++)
    {
        cmd[i] = argv[i];
    }
    cmd[end] = NULL;

    // TODO: implement all of our own functions call (cd, cp, rm, ...)
    // TODO: implement input/output redirection
    execvp(cmd[0], cmd);
    // TODO: implement pipe of output
    // e.g.: if is_piped != 0, then get the output into argv2 (i will do the transform from string to argc, argv)

    return 0;
}

/**
 * @see strcmp, call_command, fork, exit, perror
 */
int parse_commands(int argc, char *argv[])
{
    int start = 0, end = 0;
    int return_code;
    char *piped[4096];
    int piped_end = 0;

    while (end < argc)
    {
        if (strcmp(argv[end], ";") == 0)
        {
            call_command(end - start, &argv[start], piped, &piped_end, 0);
            start = end + 1;
        }

        if (strcmp(argv[end], "&&") == 0)
        {
            return_code = call_command(end - start, &argv[start], piped, &piped_end, 0);

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
            return_code = call_command(end - start, &argv[start], piped, &piped_end, 0);

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
            pid_t pid = fork();
            if (pid == 0)
            {
                call_command(end - start, &argv[start], piped, &piped_end, 0);
                exit(0);
            }
            else if (pid < 0)
            {
                perror("fork");
                return 1;
            }
            start = end + 1;
        }

        if (strcmp(argv[end], "|") == 0)
        {
            call_command(end - start, &argv[start], piped, &piped_end, 1);
            start = end + 1;
        }
        else
        {
            // TODO: add reset of piped here
            //! DONE
            piped_end = 0;
            memset(piped, 0, sizeof(piped));
        }

        end++;
    }

    if (start < end)
    {
        call_command(end - start, &argv[start], piped, &piped_end, 0);
    }

    return 0;
}

/**
 * @see fprintf
 */
void print_usage(char *program_name)
{
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
    fprintf(stdout, "    -v             Verbose, debug mode\n");
}

/**
 * @see strcmp, print_usage, exit
 */
void parse_arguments(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            // Print the usage and exit
            print_usage(argv[0]);
            exit(0);
        }

        // Check if the argument is -v
        if (strcmp(argv[i], "-v") == 0)
        {
            // Set DEBUG
            DEBUG = 1;
            continue;
        }
    }
}

/**
 * @see parse_arguments, getuid, getpwuid, sizeof, getcwd, fprintf, parse_line, parse_commands
 */
int main(int argc, char *argv[])
{
    // Parse the arguments
    parse_arguments(argc, argv);

    struct passwd *pw;
    char cwd[1024];
    char command[4096];
    int n_argc;
    char **n_argv;
    while (1)
    {
        // Print shell prompt
        pw = getpwuid(getuid());
        fprintf(stdout, "CShell(%s) - %s > ", (pw ? pw->pw_name : "no user"), getcwd(cwd, sizeof(cwd)) != NULL ? cwd : "no directory");

        // Read the command line
        fgets(command, sizeof(command), stdin);

        if (history_count < MAX_HISTORY)
        {
            history[history_count] = strdup(command);
            history_count++;
        }
        else
        {
            free(history[0]);
            for (int i = 1; i < MAX_HISTORY; i++)
            {
                history[i - 1] = history[i];
            }
            history[MAX_HISTORY - 1] = strdup(command);
        }

        // Parse and call the command
        n_argc = 0;
        parse_line(command, &n_argc, &n_argv);
        parse_commands(n_argc, n_argv);
    }

    return 0;
}
