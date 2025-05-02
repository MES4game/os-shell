// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <ctype.h>
#include <fcntl.h>  // pour open()

int DEBUG = 0;

/**
 * @see free
 */
void free_string_array(char ***array_ptr)
{
    if (array_ptr == NULL || *array_ptr == NULL) return;

    for (int i = 0; (*array_ptr)[i] != NULL; ++i) free((*array_ptr)[i]);
    free(*array_ptr);

    *array_ptr = NULL;
}

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

    if (*argc >= capacity)
    {
        *argv = realloc(*argv, (capacity++) * sizeof(char *));
        if (!*argv)
        {
            perror("realloc");
            exit(1);
        }
    }

    (*argv)[*argc] = NULL;
}

/**
 * @see strcmp, execvp
 */
int call_command(int argc, char **argv, int *piped_end, char ***piped, int is_piped)
{
    // TODO: remove this when call_command is implemented, it is just for testing parse_commands
    fprintf(stdout, "call_command: command = %s, #args = %d, args:\n", argv[0], argc - 1);
    for (int i = 1; i < argc; i++) fprintf(stdout, "    %d. %s\n", i, argv[i]);

    int end = 0;
    char *input_file = NULL;
    char *output_file = NULL;

    while ((end < argc) && (strcmp(argv[end], "<") != 0) && (strcmp(argv[end], ">") != 0)) end++;

    for (int i = end; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "<") == 0)
        {
            i++;
            input_file = argv[i];
        }
        else if (strcmp(argv[i], ">") == 0)
        {
            i++;
            output_file = argv[i];
        }
    }

    int cmd_argc = end + *piped_end;
    char **cmd_argv = malloc((cmd_argc + 1) * sizeof(char *));
    for (int i = 0; i < end; i++)        cmd_argv[i] = argv[i];
    for (int i = 0; i < *piped_end; i++) cmd_argv[i + end] = (*piped)[i];
    cmd_argv[cmd_argc] = NULL;

    free_string_array(piped);

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

    // TODO: implement all of our own functions call (cd, cp, rm, ...)
    if (strcmp(cmd_argv[0], "exit") == 0) exit(0);
    else if (strcmp(cmd_argv[0], "cat") == 0)   our_cat(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "cd") == 0)    our_cd(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "chmod") == 0) our_chmod(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "chown") == 0) our_chown(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "cp") == 0)    our_cp(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "ls") == 0)    our_ls(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "mkdir") == 0) our_mkdir(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "mv") == 0)    our_mv(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "rm") == 0)    our_rm(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "touch") == 0) our_touch(cmd_argc, cmd_argv);
    else if (strcmp(cmd_argv[0], "history") == 0)
    {
        for (int i = 0; i < history_count; i++) fprintf(stdout, "%d: %s", i + 1, history[i]);
        if (history_count == 0) fprintf(stdout, "No history available.\n");
    }
    else
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            execvp(cmd_argv[0], cmd_argv);
            exit(0);
        }
        else if (pid < 0) perror("fork");
        else wait(NULL); // Wait for all child processes to finish
    }

    return 0;
}

/**
 * @see strcmp, call_command, fork, exit, perror
 */
int parse_commands(int argc, char **argv)
{
    int start = 0, end = 0;
    int return_code;
    char **piped = NULL;
    int piped_end = 0;

    // TODO: make piped editable by call_command, so it can pipe output to the next command
    // TODO: reset piped to NULL after each call_command (if it is not piped to the next command)
    // TODO: add number of args in piped to (end - start) in calls of call_command
    // TODO: make a copy of argv from start to end when calling call_command (so it cannot affect the next command)
    while (end < argc)
    {
        if (strcmp(argv[end], ";") == 0)      return_code = call_command(end - start, &argv[start], &piped_end, &piped, 0);
        else if (strcmp(argv[end], "|") == 0) return_code = call_command(end - start, &argv[start], &piped_end, &piped, 1);
        else if (strcmp(argv[end], "&") == 0)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                return_code = call_command(end - start, &argv[start], &piped_end, &piped, 0);
                exit(0);
            }
            else if (pid < 0) perror("fork");
        }
        else if (strcmp(argv[end], "&&") == 0)
        {
            return_code = call_command(end - start, &argv[start], &piped_end, &piped, 0);

            if (return_code != 0)
            {
                while (end < argc)
                {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "||") == 0 || strcmp(argv[end], "|") == 0) break;
                    end++;
                }
            }
        }
        else if (strcmp(argv[end], "||") == 0)
        {
            return_code = call_command(end - start, &argv[start], &piped_end, &piped, 0);

            if (return_code == 0)
            {
                while (end < argc)
                {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&&") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "|") == 0) break;
                    end++;
                }
            }
        }
        else {
            end++;
            continue;
        }

        end++;
        start = end;
    }

    if (start < end) return_code = call_command(end - start, &argv[start], &piped_end, &piped, 0);

    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) continue;

    free_string_array(&piped);

    return return_code;
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

    // Initialize variables
    initconfig();

    struct passwd *pw;
    char command[MAX_LINE_LENGTH];
    int n_argc = 0;
    char **n_argv = NULL;
    int return_code = 0;
    while (1)
    {
        // Print shell prompt
        pw = getpwuid(getuid());
        fprintf(stdout, "CShell(%s) - %s > ", (pw ? pw->pw_name : "no user"), CWD);

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

        // Parse line and call commands
        n_argc = 0;
        free_string_array(&n_argv);
        parse_line(command, &n_argc, &n_argv);
        return_code = parse_commands(n_argc, n_argv);
    }

    return 0;
}
