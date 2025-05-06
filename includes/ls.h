// CShell Project - New ls command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef COMMAND_LS_H
#define COMMAND_LS_H

/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void _ls_print_usage(const char *const program_name);

/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int _ls_parse_arguments(const int argc, const char *const *const argv, int *const show_all, int *const long_format);

/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_ls(const int argc, const char *const *const argv);

#endif
