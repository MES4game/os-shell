// CShell Project - New cd command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#ifndef COMMAND_CD_H
#define COMMAND_CD_H


#include "config.h"


// previous working directory
extern char __cd_PWD[MAX_PATH_LENGTH];


/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void __cd_print_usage(char *program_name);


/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int __cd_parse_arguments(int argc, char *argv[]);


/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_cd(int argc, char *argv[]);


#endif
