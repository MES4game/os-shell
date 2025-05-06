// CShell Project - New mkdir command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "mkdir.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>


/**
 * @see printf
 */
void _mkdir_print_usage(const char *const program_name) {
    printf("Usage: %s [Options] path\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
}


/**
 * @see strcmp, _mkdir_print_usage
 */
int _mkdir_parse_arguments(const int argc, const char *const *const argv) {
    if (argc < 2) {
        // Print the usage and return
        _mkdir_print_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            _mkdir_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}


/**
 * @see _mkdir_parse_arguments, mkdir, printf, perror
 */
int our_mkdir(const int argc, const char *const *const argv) {
    // Parse the arguments
    int parse_result = _mkdir_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result) return parse_result;

    const char *dir_name = argv[1];
    int status = mkdir(dir_name, 0755);

    if (status == 0) printf("Directory '%s' created successfully.\n", dir_name);
    else {
        perror("Error creating directory");
        return 1;
    }

    return 0;
}


#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_mkdir(argc, argv);
}
#endif
