// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include "terminal.h"
#include "cat.h"
#include "cd.h"
#include "chmod.h"
#include "chown.h"
#include "cp.h"
#include "ls.h"
#include "mkdir.h"
#include "mv.h"
#include "rm.h"
#include "touch.h"

/**
 * @brief Load content of a file as a string.
 * @param fd Id of in-memory file to load.
 * @return Content load from the file.
 */
char *load_memfd_to_string(const int fd);

/**
 * @brief Clone a file inside a in-memory file.
 * @param src The id of file to copy.
 * @return Id of in-memory file that as copied content of src.
 */
int clone_memfd(const int src);

/**
 * @brief Create an environement variable if syntax allow.
 * @param arg The argument to parse.
 * @return 1 if the argument is a valid definition syntax, 0 otherwise.
 */
int setting_envvar(const char *const arg);

/**
 * @brief Call the appropriate function with the arguments.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @param use_pipe If last command piped its stdout and we have to take it (if no redirection).
 * @param pipe_used Id of in-memory file containing stdout of previous command if it was piped.
 * @param is_piped 1 if the command is piped, 0 otherwise.
 * @return 0 if the program ran successfully.
 */
int call_command(int argc, char **argv, int *const use_pipe, const int pipe_used, const int is_piped);

/**
 * @brief Parse arguments to call every commands in right order.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int parse_commands(int argc, char **argv);

/**
 * @brief Parse the line to get it as argc and argv.
 * @param line The line to parse.
 * @param argc Reference to the number of arguments for return.
 * @param argv Reference to the arguments for return.
 */
void parse_line(const char *const line, int *const argc, char ***const argv);

/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void print_usage(const char *const program_name);

/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 */
void parse_arguments(const int argc, const char *const *const argv);

/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int main(int argc, char *argv[]);

#endif
