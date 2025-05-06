// CShell Project - New rm command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef COMMAND_RM_H
#define COMMAND_RM_H

/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void _rm_print_usage(const char *const program_name);

/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int _rm_parse_arguments(const int argc, const char *const *const argv);

/**
 * @brief Check if path refer to a directory.
 * @param path The path to check.
 * @return 1 if it is a directory, 0 otherwise.
 */
int _rm_is_directory(const char *path);

/**
 * @brief Remove a directory recursively.
 * @param path The path to directory to remove.
 * @return 0 if the operation was successful, -1 otherwise.
 */
int _rm_remove_recursively(const char *path);

/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_rm(const int argc, const char *const *const argv);

#endif
