// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#define _GNU_SOURCE
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pwd.h>
#include <ctype.h>
#include <fcntl.h>


// Debug flag to print debug messages
static int DEBUG = 0;


/**
 * @see fstat, sizeof, malloc, lseek, read
 */
char *load_memfd_to_string(const int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) return NULL;

    // Allocate memory for the content of the file
    const off_t size = st.st_size;
    char *const content = malloc((size + 1) * sizeof(char));
    if (!content) return NULL;

    lseek(fd, 0, SEEK_SET);  // go back to start of file for reading
    read(fd, content, size);

    content[size] = '\0';  // null-terminate the string
    return content;
}


/**
 * @see memfd_create, fstat, sizeof, malloc, lseek, read, write, free
 */
int clone_memfd(const int src) {
    // Create a new memfd file
    const int dst = memfd_create("cloned_memfd", 0);
    if (dst == -1) return -1;

    struct stat st;
    if (fstat(src, &st) == -1) return dst;

    // Allocate memory to copy in a buffer
    const off_t size = st.st_size;
    char *const content = malloc((size + 1) * sizeof(char));
    if (!content) return dst;

    // Copy the content of the source memfd to the buffer and then to the new memfd
    lseek(src, 0, SEEK_SET);
    read(src, content, size);
    write(dst, content, size);

    free(content);

    // Go back to the start of each file
    lseek(src, 0, SEEK_SET);
    lseek(dst, 0, SEEK_SET);

    return dst;
}


/**
 * @see isalnum, sizeof, malloc, realloc, setenv, free
 */
int setting_envvar(const char *const arg) {
    // Copy the pointer to not edit it outside
    const char *ptr = arg;
    char var[MAX_ENV_NAME_LENGTH] = { '\0' };

    // Get the variable name and check if the syntax is for an environment variable definition
    int i = 0;
    while (*ptr && (i < MAX_ENV_NAME_LENGTH - 1) && (isalnum(*ptr) || *ptr == '_')) var[i++] = *ptr++;
    if (*ptr != '=') return 0;
    ptr++;

    // Copy the value for the environment variable
    int len = 0;
    int val_capacity = 10;
    char *val = malloc(val_capacity * sizeof(char));
    while (*ptr) {
        // Reallocate memory if needed
        while (len >= val_capacity - 1) {
            val_capacity *= 2;
            val = realloc(val, val_capacity * sizeof(char));
        }
        val[len++] = *ptr++;
    }
    val[len] = '\0';

    // Set the environment variable
    setenv(var, val, 1);

    // free memory
    free(val);

    return 1;
}


/**
 * @see strcmp, sizeof, malloc, dup, open, clone_memfd, dup2, close, ftruncate, lseek, setting_envvar, free, printf, fflush, exit, our_*, fork, execvp, perror, wait
 */
int call_command(int argc, char **argv, int *const use_pipe, const int pipe_used, const int is_piped) {
    int end = 0;
    const char *input_file = NULL;
    const char *output_file = NULL;

    // Go to first redirection in the command
    // redirections are always at the end of the command
    while ((end < argc) && (strcmp(argv[end], "<") != 0) && (strcmp(argv[end], ">") != 0)) end++;

    // Get input and output files path/name
    // stdin redirection is always before stdout redirection
    for (int i = end; i < argc - 1; i++) {
        if (strcmp(argv[i], "<") == 0) input_file = argv[++i];
        else if (strcmp(argv[i], ">") == 0) output_file = argv[++i];
    }

    // Copy the command from argv to end it at right position
    int cmd_argc = end;
    const char **cmd_argv = malloc((cmd_argc + 1) * sizeof(char *));
    for (int i = 0; i < cmd_argc; i++) cmd_argv[i] = argv[i];
    cmd_argv[cmd_argc] = NULL;

    // Redirect stdin in this order (if possible) input_file > pipe_used > saved_stdin
    const int saved_stdin = dup(STDIN_FILENO);
    int fd_in = saved_stdin;
    if (input_file != NULL) fd_in = open(input_file, O_RDONLY);
    else if (use_pipe)      fd_in = clone_memfd(pipe_used);
    dup2(fd_in, STDIN_FILENO);
    close(fd_in);

    // Reset pipe_used to 0 for next command
    *use_pipe = is_piped;
    ftruncate(pipe_used, 0);
    lseek(pipe_used, 0, SEEK_SET);

    // Redirect stdout in this order (if possible) output_file > pipe_used > saved_stdin
    const int saved_stdout = dup(STDOUT_FILENO);
    int fd_out = saved_stdout;
    if (output_file != NULL) fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else if (is_piped)       fd_out = pipe_used;
    dup2(fd_out, STDOUT_FILENO);
    if (fd_out != pipe_used) close(fd_out);

    if (setting_envvar(cmd_argv[0]));
    else if (strcmp(cmd_argv[0], "exit") == 0) {
        // free memory
        free(cmd_argv);
        // Reset stdin and stdout
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        // Close CShell
        printf("\nBye Bye \033[32m%s\033[0m!\n\n", USER);
        fflush(stdout);
        exit(0);
    }
    else if (strcmp(cmd_argv[0], "history") == 0) {
        int count = 0;
        while (count < HISTORY_SIZE && HISTORY[count][0]) count++;
        for (int i = count - 1; i > -1; i--) printf("%d: %s\n", count - i, HISTORY[i]);
        if (count == 0) printf("No history available.\n");
    }
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
    else {
        pid_t pid = fork();
        if (pid == 0) {
            execvp(cmd_argv[0], (char **)cmd_argv);
            // execvp exit if it succeeds, if we are here, it failed
            perror("execvp");
            exit(1);
        }
        else if (pid > 0) wait(NULL);
    }

    // free memory
    free(cmd_argv);
    
    // Go back to the start of the pipe
    lseek(pipe_used, 0, SEEK_SET);
    
    // Reset stdin and stdout
    fflush(stdout);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);

    return 0;
}


/**
 * @see memfd_create, strcmp, call_command, fork, getpid, printf, fflush, exit, close
 */
int parse_commands(int argc, char **argv) {
    int start = 0, end = 0;
    int return_code = 0;
    int use_pipe = 0;
    const int pipe_used = memfd_create("pipe_used", 0);

    pid_t pid;
    pid_t cpid;
    while (end < argc) {
        // Run parsed command and wait for it to finish
        if (strcmp(argv[end], ";") == 0)      return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);

        // Run parsed command, capture its output inside our pipe and wait for it to finish
        else if (strcmp(argv[end], "|") == 0) return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 1);

        // Run parsed command and do not wait for it to finish
        else if (strcmp(argv[end], "&") == 0) {
            pid = fork();

            if (pid == 0) {
                cpid = getpid();

                printf("[bg]    Process %d\n", cpid);
                fflush(stdout);
                return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);
                printf("[bg]    Done %d\n", cpid);
                fflush(stdout);

                exit(0);
            }
        }

        // Run parsed command, wait for it to finish and if it failed, skip next command
        else if (strcmp(argv[end], "&&") == 0) {
            return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);

            if (return_code != 0) {
                while (end < argc) {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "||") == 0 || strcmp(argv[end], "|") == 0) break;
                    end++;
                }
            }
        }

        // Run parsed command, wait for it to finish and if it succeeded, skip next command
        else if (strcmp(argv[end], "||") == 0) {
            return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);

            if (return_code == 0) {
                while (end < argc) {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&&") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "|") == 0) break;
                    end++;
                }
            }
        }

        // Continue to parse the command
        else {
            end++;
            continue;
        }

        // Reset parsed command
        start = ++end;
    }

    // Run last command if it is not empty
    if (start < end) return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);

    // Close the pipe
    close(pipe_used);

    return return_code;
}


/**
 * @see sizeof, malloc, isspace, realloc, isalnum, memset, memfd_create, dup, dup2, close, parse_commands, fflush, free, load_memfd_to_string, strlen, strncpy, getenv
 */
void parse_line(const char *const line, int *const argc, char ***const argv) {
    int argv_capacity = 10;
    *argc = 0;
    *argv = malloc(argv_capacity * sizeof(char *));

    const char *ptr = line;
    char *arg, end;
    int arg_capacity, len, is_name;

    int sub_i;

    char sub_command[MAX_LINE_LENGTH], **sub_argv = NULL;
    char *sub_result;
    int sub_argc, sub_pipe_used, sub_saved_stdout, sub_len;

    char sub_varname[MAX_ENV_NAME_LENGTH];
    char *sub_envval;
    int sub_bracket;
    while (*ptr) {
        // Skip leading spaces
        while (isspace((unsigned char)*ptr)) ptr++;

        // Break if we reach the end of the line
        if (!(*ptr)) break;

        arg_capacity = 10;
        len = 0;
        end = *ptr == '\"' || *ptr == '\'' ? *ptr : ' ';
        is_name = 1;

        arg = malloc(arg_capacity * sizeof(char));

        if (end != ' ') ptr++;
        while (*ptr && *ptr != end && !(end == ' ' && isspace((unsigned char)*ptr))) {
            // Reallocate memory if needed
            while (len >= arg_capacity - 1) {
                arg_capacity *= 2;
                arg = realloc(arg, arg_capacity * sizeof(char));
            }

            // Check if subsequent characters are valid for an environment variable name
            if ((len >= MAX_ENV_NAME_LENGTH) || !(isalnum(*ptr) || *ptr == '_' || *ptr == '=')) is_name = 0;

            // Check for escaped characters inside double quotes
            // Currently, just to be sure they are not interpreted as special characters
            if (*ptr == '\\' && end != '\'') {
                arg[len++] = *ptr++;
                if (!(*ptr)) break;
                arg[len++] = *ptr++;
            }

            // Check if it is a definition of an environment variable
            else if (is_name && *ptr == '=') {
                is_name = 0;
                arg[len++] = *ptr++;
                if (!(*ptr)) break;
                end = *ptr == '\"' || *ptr == '\'' ? *ptr : ' ';
                if (end != ' ') ptr++;
            }

            // Parse for sub-shell or environment variable
            else if (*ptr == '$' && end != '\'') {
                ptr++;

                // Parse for sub-shell
                if (*ptr == '(') {
                    ptr++;

                    memset(sub_command, '\0', MAX_LINE_LENGTH);
                    sub_i = 0;

                    // Copy the command inside the parentheses
                    while (*ptr && *ptr != ')') {
                        if (*ptr == '\\') {
                            ptr++;
                            if (!(*ptr)) break;
                        }

                        if (sub_i < MAX_LINE_LENGTH - 1) sub_command[sub_i++] = *ptr++;
                    }
                    if (*ptr == ')') ptr++;

                    // Parse the sub-command
                    parse_line(sub_command, &sub_argc, &sub_argv);

                    // Create a new pipe to capture the output of the sub-command
                    sub_pipe_used = memfd_create("sub_pipe_used", 0);
                    sub_saved_stdout = dup(STDOUT_FILENO);
                    dup2(sub_pipe_used, STDOUT_FILENO);
                    close(sub_pipe_used);

                    // Call the sub-command
                    parse_commands(sub_argc, sub_argv);
                    fflush(stdout);

                    // free memory
                    for (int i = 0; i < sub_argc; i++) free(sub_argv[i]);
                    free(sub_argv);
                    sub_argv = NULL;
                    sub_argc = 0;

                    // Get output from the pipe
                    sub_result = load_memfd_to_string(STDOUT_FILENO);
                    sub_len = strlen(sub_result);
                    if (sub_len) {
                        // Reallocate memory if needed
                        while (len + sub_len >= arg_capacity - 1) {
                            arg_capacity *= 2;
                            arg = realloc(arg, arg_capacity * sizeof(char));
                        }

                        // Copy the result to the argument
                        strncpy(&arg[len], sub_result, sub_len);
                        len += sub_len;
                    }
                    if (sub_result) free(sub_result);

                    // Reset stdout to the original value
                    dup2(sub_saved_stdout, STDOUT_FILENO);
                    close(sub_saved_stdout);
                }

                // Parse for environment variable
                else if (*ptr == '{' || isalnum(*ptr) || *ptr == '_') {
                    memset(sub_varname, '\0', MAX_ENV_NAME_LENGTH);
                    sub_i = 0;
                    sub_bracket = (*ptr == '{');

                    // Copy the variable name inside the braces or until a non-valid character
                    if (sub_bracket) ptr++;
                    while (*ptr && (sub_i < MAX_ENV_NAME_LENGTH - 1) && (isalnum(*ptr) || *ptr == '_')) sub_varname[sub_i++] = *ptr++;
                    if (sub_bracket) while (*ptr && *ptr != '}') ptr++;
                    if (*ptr == '}') ptr++;

                    // Get the environment variable value
                    sub_envval = getenv(sub_varname);
                    sub_len = strlen(sub_envval);
                    if (sub_len) {
                        // Reallocate memory if needed
                        while (len + sub_len >= arg_capacity - 1) {
                            arg_capacity *= 2;
                            arg = realloc(arg, arg_capacity * sizeof(char));
                        }

                        // Copy the result to the argument
                        strncpy(&arg[len], sub_result, sub_len);
                        len += sub_len;
                    }
                }
            }

            // Continue to copy characters
            else arg[len++] = *ptr++;
        }
        if (end != ' ') ptr++;

        arg[len] = '\0';

        // Reallocate memory if needed
        while (*argc >= argv_capacity - 1) {
            argv_capacity *= 2;
            *argv = realloc(*argv, argv_capacity * sizeof(char *));
        }

        (*argv)[*argc] = arg;
        (*argc)++;
    }

    (*argv)[*argc] = NULL;
}


/**
 * @see printf
 */
void print_usage(const char *const program_name) {
    printf("Usage: %s [Options]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
    printf("    -v             Verbose, debug mode\n");
}


/**
 * @see strcmp, print_usage, exit
 */
void parse_arguments(const int argc, const char *const *const argv) {
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
 * @see printf
 */
void print_login_message() {
    printf("\n");
    printf("\033[1;40m   \033[35m  _____  \033[36m  _____   _    _   ______   _        _        _    \033[0m\n");
    printf("\033[1;40m   \033[35m /  ___| \033[36m / ____| | |  | | |  ____| | |      | |      | |   \033[0m\n");
    printf("\033[1;40m   \033[35m|  /     \033[36m| (___   | |__| | | |__    | |      | |      | |   \033[0m\n");
    printf("\033[1;40m   \033[35m| |      \033[36m \\___ \\  |  __  | |  __|   | |      | |      |_|   \033[0m\n");
    printf("\033[1;40m   \033[35m|  \\___  \033[36m ____) | | |  | | | |____  | |____  | |____   _    \033[0m\n");
    printf("\033[1;40m   \033[35m \\_____| \033[36m|_____/  |_|  |_| |______| |______| |______| |_|   \033[0m\n");
    printf("\033[1;40m                                                               \033[0m\n");
    printf("\n");
    printf("Welcome \033[32m%s\033[0m to CShell release 1.0.0 !\n", USER);
    printf("\n");
    printf("If you have any issue, please consider compiling and executing with gcc 13.3.0 under Ubuntu 24.04.2\n");
    printf("If it persists, contact us: cshell@et3-os-project.fr\n");
    printf("\n");
}


/**
 * @see parse_arguments, getcwd, getuid, getpwuid, strncpy, enable_raw_mode, print_login_message, printf, fflush, our_terminal, parse_line, parse_commands, free
 */
int main(int argc, char *argv[]) {
    // Parse the arguments
    parse_arguments(argc, (const char *const *const)argv);

    // Initialize
    getcwd(CWD, MAX_PATH_LENGTH);
    getcwd(PWD, MAX_PATH_LENGTH);

    struct passwd *pw;
    pw = getpwuid(getuid());
    strncpy(USER, pw ? pw->pw_name : "", MAX_ENV_NAME_LENGTH);
    USER[MAX_ENV_NAME_LENGTH - 1] = '\0';

    enable_raw_mode();

    // Display login message
    print_login_message();

    char **n_argv;
    int n_argc, return_code;
    while (1) {
        // Update CWD and USER
        getcwd(CWD, MAX_PATH_LENGTH);
        pw = getpwuid(getuid());
        strncpy(USER, pw ? pw->pw_name : "", MAX_ENV_NAME_LENGTH);
        USER[MAX_ENV_NAME_LENGTH - 1] = '\0';

        // Display terminal for first time
        printf("\033[32m%s\033[37m@\033[32mCShell\033[37m:\033[34m%s\033[0m > ", USER, CWD);
        fflush(stdout);

        // Display terminal and get command line
        while (!our_terminal());
        fflush(stdout);

        // Parse line
        parse_line(COMMAND, &n_argc, &n_argv);

        // Call commands
        return_code = parse_commands(n_argc, n_argv);
        if (DEBUG) printf("Last command return code: %d\n", return_code);
        fflush(stdout);

        // Reset and free n_argv
        for (int i = 0; i < n_argc; i++) free(n_argv[i]);
        free(n_argv);
        n_argv = NULL;
        n_argc = 0;
    }

    printf("\nBye Bye \033[32m%s\033[0m!\n\n", USER);

    return 0;
}
