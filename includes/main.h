// CShell Project - New shell in C
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#ifndef MAIN_H
#define MAIN_H


#include "all.h"


// Debug flag to print debug messages
extern int DEBUG;


/**
 * @brief Parse the line to get it as argc and argv.
 * @param line The line to parse.
 * @param argc Reference to the number of arguments for return.
 * @param argv Reference to the arguments for return.
 */
void parse_line(const char *line, int *argc, char ***argv);


/**
 * @brief Call the appropriate function with the arguments.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @param argv2 The arguments from previous piped command.
 * @param is_piped 1 if the command is piped, 0 otherwise.
 * @return 0 if the program ran successfully.
 */
int call_command(int argc, char *argv[], char *argv2[], int *piped_end, int is_piped);


/**
 * @brief Parse arguments to call every commands in right order.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int parse_commands(int argc, char *argv[]);


/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void print_usage(char *program_name);


/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 */
void parse_arguments(int argc, char *argv[]);


/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int main(int argc, char *argv[]);


#endif
