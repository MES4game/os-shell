// CShell Project - New chmod command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef COMMAND_CHMOD_H
#define COMMAND_CHMOD_H

/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void _chmod_print_usage(const char *const program_name);

/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int _chmod_parse_arguments(const int argc, const char *const *const argv, const char **const mode_arg, const char **const file_arg);

/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_chmod(const int argc, const char *const *const argv);

#endif
