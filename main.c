// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


// #include "all.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>


// Debug flag to print debug messages
int DEBUG = 0;


/**
 * @brief Parse the line to get it as argc and argv.
 * @param line The line to parse.
 * @param argc Reference to the number of arguments for return.
 * @param argv Reference to the arguments for return.
 */
void parse_line(const char *line, int *argc, char ***argv) {
    int capacity = 10;
    *argv = malloc(capacity * sizeof(char *));
    *argc = 0;

    const char *ptr = line;
    while (*ptr) {
        while (isspace((unsigned char)*ptr)) ptr++; // Skip leading spaces

        if (*ptr == '\0') break;

        const char *start;
        char *arg;
        int in_quotes = 0;
        int len = 0;

        start = ptr;

        // Estimate the max argument length
        arg = malloc(strlen(ptr) + 1);
        if (!arg) {
            perror("malloc");
            exit(1);
        }

        while (*ptr) {
            if (*ptr == '\"' || *ptr == '\'') {
                in_quotes = !in_quotes;
                ptr++; // Skip the quote
                continue;
            }

            if (!in_quotes && isspace((unsigned char)*ptr))
                break;

            arg[len++] = *ptr++;
        }

        arg[len] = '\0';

        if (*argc >= capacity) {
            capacity *= 2;
            *argv = realloc(*argv, capacity * sizeof(char *));
            if (!*argv) {
                perror("realloc");
                exit(1);
            }
        }

        (*argv)[*argc] = arg;
        (*argc)++;
    }
}


/**
 * @brief Call the appropriate function with the arguments.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @param argv2 The arguments from previous piped command.
 * @param is_piped 1 if the command is piped, 0 otherwise.
 * @return 0 if the program ran successfully.
 * @see strcmp, execvp
 */
int call_command(int argc, char *argv[], char *argv2[], int is_piped) {
    int end = 0;
    char *input_file = NULL;
    char *output_file = NULL;

    while ((end < argc) && (strcmp(argv[end], "<") != 0) && (strcmp(argv[end], ">") != 0)) {
        end++;
    }

    if ((end < argc) && (strcmp(argv[end], "<"))) {
        end++;
        input_file = argv[end];
        end++;
    }

    if ((end < argc) && (strcmp(argv[end], ">"))) {
        end++;
        output_file = argv[end];
        end++;
    }

    char *cmd[4096];
    for (int i = 0; i < end; i++) {
        cmd[i] = argv[i];
    }
    cmd[end] = NULL;

    execvp(cmd[0], cmd);

    return 0;
}


/**
 * @brief Parse arguments to call every commands in right order.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 * @see strcmp, call_command, fork, exit, perror
 */
int parse_commands(int argc, char *argv[]) {
    int start = 0, end = 0;
    int return_code;
    char *piped[4096];

    while (end < argc) {
        if (strcmp(argv[end], ";") == 0) {
            call_command(end - start, &argv[start], piped, 0);
            start = end + 1;
        }

        else if (strcmp(argv[end], "&&") == 0) {
            return_code = call_command(end - start, &argv[start], piped, 0);

            if (return_code == 0) {
                start = end + 1;
            }
            else {
                while (end < argc) {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "||") == 0 || strcmp(argv[end], "|") == 0) {
                        start = end + 1;
                        end++;
                        break;
                    }
                    end++;
                }
            }
        }

        else if (strcmp(argv[end], "||") == 0) {
            return_code = call_command(end - start, &argv[start], piped, 0);

            if (return_code != 0) {
                start = end + 1;
            }
            else {
                while (end < argc) {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&&") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "|") == 0) {
                        start = end + 1;
                        end++;
                        break;
                    }
                    end++;
                }
            }
        }

        else if (strcmp(argv[end], "&") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                call_command(end - start, &argv[start], piped, 0);
                exit(0);
            }
            else if (pid < 0) {
                perror("fork");
                return 1;
            }
            start = end + 1;
        }

        else if (strcmp(argv[end], "|") == 0) {
            // TODO: implement pipe output redirection to piped
            call_command(end - start, &argv[start], piped, 1);
            start = end + 1;
        }

        end++;
    }

    return 0;
}


/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 * @see fprintf
 */
void print_usage(char *program_name) {
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
    fprintf(stdout, "    -v             Verbose, debug mode\n");
}


/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @see strcmp, print_usage, exit
 */
void parse_arguments(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and exit
            print_usage(argv[0]);
            exit(0);
        }

        // Check if the argument is -v
        if (strcmp(argv[i], "-v") == 0) {
            // Set DEBUG
            DEBUG = 1;
            continue;
        }
    }
}


/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 * @see parse_arguments, getuid, getpwuid, getcwd, fprintf, parse_line, parse_commands
 */
int main(int argc, char *argv[]) {
    // Parse the arguments
    parse_arguments(argc, argv);

    struct passwd *pw;
    char cwd[1024];
    char command[4096];
    int n_argc;
    char *n_argv[4096];
    while (1) {
        // Print shell prompt
        pw = getpwuid(getuid());
        fprintf(stdout, "CShell(%s) - %s > ", (pw ? pw->pw_name : "no user"), getcwd(cwd, sizeof(cwd)) != NULL ? cwd : "no directory");

        // Read the command line
        // TODO: Read the command line
        fgets(command, sizeof(command), stdin);

        // Parse and call the command
        n_argc = 0;
        parse_line(command, &n_argc, &n_argv);
        parse_commands(n_argc, n_argv);
    }

    return 0;
}
