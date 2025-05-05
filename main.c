// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#define _GNU_SOURCE
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pwd.h>
#include <ctype.h>
#include <fcntl.h>


// Debug flag to print debug messages
static int DEBUG = 0;
// If CShell is currently running
static int RUNNING = 1;


/**
 * @see fstat, sizeof, malloc, lseek, read
 */
char *load_memfd_to_string(const int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) return NULL;

    const off_t size = st.st_size;
    char *const content = malloc((size + 1) * sizeof(char));
    if (!content) return NULL;

    lseek(fd, 0, SEEK_SET);
    read(fd, content, size);

    content[size] = '\0';
    return content;
}


/**
 * @see memfd_create, fstat, sizeof, malloc, lseek, read, write
 */
int clone_memfd(const int src) {
    const int dst = memfd_create("cloned_memfd", 0);
    if (dst == -1) return -1;

    struct stat st;
    if (fstat(src, &st) == -1) return dst;

    const off_t size = st.st_size;
    char *const content = malloc((size + 1) * sizeof(char));
    if (!content) return dst;

    lseek(src, 0, SEEK_SET);
    read(src, content, size);
    write(dst, content, size);

    free(content);

    lseek(src, 0, SEEK_SET);
    lseek(dst, 0, SEEK_SET);

    return dst;
}


/**
 * @see isalnum, sizeof, malloc, realloc, setenv
 */
int setting_envvar(const char *const arg) {
    const char *ptr = arg;
    char var[MAX_ENV_NAME_LENGTH] = { '\0' };

    int i = 0;
    while (*ptr && (i < MAX_ENV_NAME_LENGTH - 1) && (isalnum(*ptr) || *ptr == '_')) var[i++] = *ptr++;
    if (*ptr != '=') return 0;
    ptr++;

    int len = 0;
    ssize_t val_capacity = 10;
    char *val = malloc(val_capacity * sizeof(char));

    while (*ptr) {
        while (len >= val_capacity - 1) {
            val_capacity *= 2;
            val = realloc(val, val_capacity * sizeof(char));
        }
        val[len++] = *ptr++;
    }
    val[len] = '\0';

    setenv(var, val, 1);

    free(val);

    return 1;
}


/**
 * @see strcmp, sizeof, malloc, dup, fstat, open, clone_memfd, dup2, close, ftruncate, lseek, exit, our_*, printf, fork, execvp, wait, fflush
 */
int call_command(int argc, char **argv, int *const use_pipe, const int pipe_used, const int is_piped) {
    int end = 0;
    const char *input_file = NULL;
    const char *output_file = NULL;

    while ((end < argc) && (strcmp(argv[end], "<") != 0) && (strcmp(argv[end], ">") != 0)) end++;

    for (int i = end; i < argc - 1; i++) {
        if (strcmp(argv[i], "<") == 0) input_file = argv[++i];
        else if (strcmp(argv[i], ">") == 0) output_file = argv[++i];
    }

    int cmd_argc = end;
    const char **cmd_argv = malloc((cmd_argc + 1) * sizeof(char *));
    for (int i = 0; i < cmd_argc; i++) cmd_argv[i] = argv[i];
    cmd_argv[cmd_argc] = NULL;

    const int saved_stdin = dup(STDIN_FILENO);
    int fd_in = saved_stdin;
    if (input_file != NULL) fd_in = open(input_file, O_RDONLY);
    else if (use_pipe)      fd_in = clone_memfd(pipe_used);
    dup2(fd_in, STDIN_FILENO);
    close(fd_in);

    *use_pipe = is_piped;
    ftruncate(pipe_used, 0);
    lseek(pipe_used, 0, SEEK_SET);

    const int saved_stdout = dup(STDOUT_FILENO);
    int fd_out = saved_stdout;
    if (output_file != NULL) fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    else if (is_piped)       fd_out = pipe_used;
    dup2(fd_out, STDOUT_FILENO);
    if (fd_out != pipe_used) close(fd_out);

    if (setting_envvar(cmd_argv[0]));
    else if (strcmp(cmd_argv[0], "exit") == 0) {
        // free
        free(cmd_argv);
        // Reset stdin and stdout
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        // Close CShell
        printf("\nBye Bye \033[1;32m%s\033[0m!\n\n", USER);
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
            perror("execvp");
            exit(1);
        }
        else if (pid > 0) wait(NULL);
    }

    free(cmd_argv);

    fflush(stdout);  // make sure all output is saved before closing

    lseek(pipe_used, 0, SEEK_SET);  // go back to start for pipe_used

    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);

    return 0;
}


/**
 * @see memfd_create, strcmp, call_command, fork, getpid, printf, exit, close
 */
int parse_commands(int argc, char **argv) {
    int start = 0, end = 0;
    int return_code = 0;
    int use_pipe = 0;
    const int pipe_used = memfd_create("pipe_used", 0);

    pid_t pid;
    pid_t cpid;
    while (end < argc) {
        if (strcmp(argv[end], ";") == 0)      return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);
        else if (strcmp(argv[end], "|") == 0) return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 1);
        else if (strcmp(argv[end], "&") == 0) {
            pid = fork();

            if (pid == 0) {
                cpid = getpid();

                printf("[bg]    Process %d\n", cpid);
                return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);
                printf("[bg]    Done %d\n", cpid);

                exit(0);
            }
        }
        else if (strcmp(argv[end], "&&") == 0) {
            return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);

            if (return_code != 0) {
                while (end < argc) {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "||") == 0 || strcmp(argv[end], "|") == 0) break;
                    end++;
                }
            }
        }
        else if (strcmp(argv[end], "||") == 0) {
            return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);

            if (return_code == 0) {
                while (end < argc) {
                    if (strcmp(argv[end], ";") == 0 || strcmp(argv[end], "&&") == 0 || strcmp(argv[end], "&") == 0 || strcmp(argv[end], "|") == 0) break;
                    end++;
                }
            }
        }
        else {
            end++;
            continue;
        }

        start = ++end;
    }

    if (start < end) return_code = call_command(end - start, &argv[start], &use_pipe, pipe_used, 0);

    close(pipe_used);

    return return_code;
}


/**
 * @see sizeof, malloc, isspace, realloc, memset, memfd_create, dup, dup2, close, parse_commands, fflush, isalnum, free, getenv
 */
void parse_line(const char *const line, int *const argc, char ***const argv) {
    ssize_t argv_capacity = 10;
    *argc = 0;
    *argv = malloc(argv_capacity * sizeof(char *));

    const char *ptr = line;
    char *arg, end;
    ssize_t arg_capacity, len, is_name;

    int sub_i;

    char sub_command[MAX_LINE_LENGTH], **sub_argv = NULL;
    char *sub_result;
    int sub_depth, sub_argc, sub_pipe_used, sub_saved_stdout, sub_len;

    char sub_varname[MAX_ENV_NAME_LENGTH];
    char *sub_envval;
    int sub_bracket;
    while (*ptr) {
        while (isspace((unsigned char)*ptr)) ptr++;

        if (!(*ptr)) break;

        arg_capacity = 10;
        len = 0;
        end = *ptr == '\"' || *ptr == '\'' ? *ptr : ' ';
        is_name = 1;

        arg = malloc(arg_capacity * sizeof(char));

        if (end != ' ') ptr++;
        while (*ptr && *ptr != end && !(end == ' ' && isspace((unsigned char)*ptr))) {
            while (len >= arg_capacity - 1) {
                arg_capacity *= 2;
                arg = realloc(arg, arg_capacity * sizeof(char));
            }

            if ((len >= MAX_ENV_NAME_LENGTH) || !(isalnum(*ptr) || *ptr == '_' || *ptr == '=')) is_name = 0;

            if (*ptr == '\\' && end != '\'') {
                arg[len++] = *ptr++;
                if (!(*ptr)) break;
                arg[len++] = *ptr++;
            }
            else if (is_name && *ptr == '=') {
                is_name = 0;
                arg[len++] = *ptr++;
                if (!(*ptr)) break;
                end = *ptr == '\"' || *ptr == '\'' ? *ptr : ' ';
                if (end != ' ') ptr++;
            }
            else if (*ptr == '$' && end != '\'') {
                ptr++;
                // Parse for sub-shell
                if (*ptr == '(') {
                    memset(sub_command, '\0', MAX_LINE_LENGTH);
                    sub_i = 0;
                    sub_depth = 0;

                    ptr++;
                    while (*ptr && (*ptr != ')' || sub_depth)) {
                        if (*ptr == '\\') {
                            ptr++;
                            if (!(*ptr)) break;
                        }
                        else if (*ptr == '(') sub_depth++;
                        else if (*ptr == ')') sub_depth--;

                        if (sub_i < MAX_LINE_LENGTH - 1) sub_command[sub_i++] = *ptr++;
                    }
                    if (*ptr == ')') ptr++;

                    parse_line(sub_command, &sub_argc, &sub_argv);

                    sub_pipe_used = memfd_create("sub_pipe_used", 0);
                    sub_saved_stdout = dup(STDOUT_FILENO);
                    dup2(sub_pipe_used, STDOUT_FILENO);
                    close(sub_pipe_used);

                    parse_commands(sub_argc, sub_argv);
                    fflush(stdout);  // make sure all output is saved before closing

                    for (int i = 0; i < sub_argc; i++) free(sub_argv[i]);
                    free(sub_argv);
                    sub_argc = 0;

                    sub_result = load_memfd_to_string(STDOUT_FILENO);
                    sub_len = strlen(sub_result);
                    if (sub_len) {
                        while (len + sub_len >= arg_capacity - 1) {
                            arg_capacity *= 2;
                            arg = realloc(arg, arg_capacity * sizeof(char));
                        }
                        strncpy(&arg[len], sub_result, sub_len);
                        len += sub_len;
                    }
                    if (sub_result) free(sub_result);

                    dup2(sub_saved_stdout, STDOUT_FILENO);
                    close(sub_saved_stdout);
                }

                // Parse for environment variable
                else if (*ptr == '{' || isalnum(*ptr) || *ptr == '_') {
                    memset(sub_varname, '\0', MAX_ENV_NAME_LENGTH);
                    sub_i = 0;
                    sub_bracket = (*ptr == '{');

                    if (sub_bracket) ptr++;
                    while (*ptr && (sub_i < MAX_ENV_NAME_LENGTH - 1) && (isalnum(*ptr) || *ptr == '_')) sub_varname[sub_i++] = *ptr++;
                    if (sub_bracket) while (*ptr && *ptr != '}') ptr++;
                    if (*ptr == '}') ptr++;

                    sub_envval = getenv(sub_varname);
                    sub_len = strlen(sub_envval);
                    if (sub_len) {
                        while (len + sub_len >= arg_capacity - 1) {
                            arg_capacity *= 2;
                            arg = realloc(arg, arg_capacity * sizeof(char));
                        }
                        strncpy(&arg[len], sub_result, sub_len);
                        len += sub_len;
                    }
                }
            }
            else arg[len++] = *ptr++;
        }
        if (end != ' ') ptr++;

        arg[len] = '\0';

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
 * @see parse_arguments, our_config_init, enable_raw_mode, getuid, getpwuid, printf, fflush, parse_line, parse_commands
 */
int main(int argc, char *argv[]) {
    // Parse the arguments
    parse_arguments(argc, (const char *const *const)argv);

    // Initialize
    getcwd(PWD, MAX_PATH_LENGTH);
    enable_raw_mode();

    struct passwd *pw;
    char **n_argv;
    int n_argc, return_code;
    while (RUNNING) {
        // Get CWD and USER
        getcwd(CWD, MAX_PATH_LENGTH);
        pw = getpwuid(getuid());
        strncpy(USER, pw ? pw->pw_name : "", MAX_ENV_NAME_LENGTH);
        USER[MAX_ENV_NAME_LENGTH - 1] = '\0';

        // Display terminal for first time
        printf("\033[1;32m%s\033[0m@\033[1;32mCShell\033[0m:\033[1;34m%s\033[0m> ", USER, CWD);
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

        // Reset and free args
        for (int i = 0; i < n_argc; i++) free(n_argv[i]);
        free(n_argv);
        n_argc = 0;
    }

    return 0;
}
