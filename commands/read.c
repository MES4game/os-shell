// Project - New read command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 * @brief Print the usage of the program.
 *
 * This function prints the usage of the program.
 *
 * @param program_name The name of the program.
 * @return
 * @see fprintf
 */
void __read_print_usage(char *program_name) {
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}


/**
 * @brief Parse the arguments of the program.
 *
 * This function parses the arguments of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return
 * @see strcmp, __read_print_usage, exit
 */
void __read_parse_arguments(int argc, char *argv[]) {
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and exit
            __read_print_usage(argv[0]);
            exit(0);
        }
    }
}


/**
 * @brief Main function of the program.
 *
 * This function is the main function of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 * @see __read_parse_arguments
 */
int our_read(int argc, char *argv[]) {
    // Parse the arguments
    __read_parse_arguments(argc, argv);

    // TODO: Implement the read command

    return 0;
}


/**
 * ONLY FOR COMPILATION
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_read(argc, argv);
}
