// CShell Project - New mkdir command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#ifndef COMMAND_MKDIR_H
#define COMMAND_MKDIR_H


/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void __mkdir_print_usage(char *program_name);


/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int __mkdir_parse_arguments(int argc, char *argv[]);


/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_mkdir(int argc, char *argv[]);


#endif
