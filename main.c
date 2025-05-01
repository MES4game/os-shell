// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <fcntl.h> // pour open()

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
    return 0;

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
    if (strcmp(cmd[0], "ls") == 0)
    {
        our_ls(end, cmd);
    }

    if (strcmp(cmd[0], "cd") == 0)
    {
        our_cd(end, cmd);
    }

    if (strcmp(cmd[0], "cp") == 0)
    {
        our_cp(end, cmd);
    }
    if (strcmp(cmd[0], "rm") == 0)
    {
        our_rm(end, cmd);
    }

    if (strcmp(cmd[0], "cat") == 0)
    {
        our_cat(end, cmd);
    }

    if (strcmp(cmd[0], "exit") == 0)
    {
        exit(0); // pour quitter directement le shell
    }
    if (strcmp(cmd[0], "chmod") == 0)
    {
        our_chmod(end, cmd);
    }

    if (strcmp(cmd[0], "chown") == 0)
    {
        our_chown(end, cmd);
    }

    if (strcmp(cmd[0], "mkdir") == 0)
    {
        our_mkdir(end, cmd);
    }

    if (strcmp(cmd[0], "touch") == 0)
    {
        our_touch(end, cmd);
    }

    if (strcmp(cmd[0], "mv") == 0)
    {
        our_mv(end, cmd);
    }

    if (strcmp(cmd[0], "history") == 0)
    {
        for (int i = 0; i < history_count; i++)
        {
            fprintf(stdout, "%d: %s", i + 1, history[i]);
        }
        if (history_count == 0)
        {
            fprintf(stdout, "No history available.\n");
        }
    }

    // TODO: implement input/output redirection

    // Gérer la redirection d'entrée et de sortie
    if (input_file != NULL)
    {
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in < 0)
        {
            perror("Erreur ouverture input_file");
            exit(EXIT_FAILURE);
        }
        if (dup2(fd_in, STDIN_FILENO) < 0)
        {
            perror("Erreur dup2 input_file");
            exit(EXIT_FAILURE);
        }
        close(fd_in);
    }

    if (output_file != NULL)
    {
        int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0)
        {
            perror("Erreur ouverture output_file");
            exit(EXIT_FAILURE);
        }
        if (dup2(fd_out, STDOUT_FILENO) < 0)
        {
            perror("Erreur dup2 output_file");
            exit(EXIT_FAILURE);
        }
        close(fd_out);
    }

    execvp(cmd[0], cmd);

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

    // TODO: make piped editable by call_command, so it can pipe output to the next command
    // TODO: reset piped to NULL after each call_command (if it is not piped to the next command)
    // TODO: add number of args in piped to (end - start) in calls of call_command
    // TODO: make a copy of argv from start to end when calling call_command (so it cannot affect the next command)
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

        // Add commnand to history
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
