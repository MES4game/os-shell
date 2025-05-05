// CShell Project - New mv command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef COMMAND_MV_H
#define COMMAND_MV_H

/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void _mv_print_usage(const char *const program_name);

/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int _mv_parse_arguments(const int argc, const char *const *const argv);

/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_mv(const int argc, const char *const *const argv);

#endif
