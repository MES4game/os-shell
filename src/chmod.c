// CShell Project - New chmod command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "chmod.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>


/**
 * @see printf
 */
void _chmod_print_usage(const char *const program_name) {
    printf("Usage: %s [Options]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
}


/**
 * @see strcmp, _chmod_print_usage
 */
int _chmod_parse_arguments(const int argc, const char *const *const argv, const char **const mode_arg, const char **const file_arg) {
    *mode_arg = NULL;
    *file_arg = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            _chmod_print_usage(argv[0]);
            return -1;
        }

        else if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
            perror("Unknown option");
            return 1;
        }

        else if (*mode_arg == NULL) *mode_arg = argv[i];

        else if (*file_arg == NULL) *file_arg = argv[i];

        else {
            perror("Too many arguments");
            return 1;
        }
    }

    if (*mode_arg == NULL || *file_arg == NULL) {
        perror("Missing mode or file");
        return 1;
    }

    return 0;
}


/**
 * @see _chmod_parse_arguments
 */
int our_chmod(const int argc, const char *const *const argv) {
    const char *mode_str;
    const char *filename;

    // Parse the arguments
    int parse_result = _chmod_parse_arguments(argc, argv, &mode_str, &filename);
    if (parse_result == -1) return 0;
    if (parse_result) return parse_result;

    char *endptr;
    long mode = strtol(mode_str, &endptr, 8);
    if (*endptr != '\0') {
        perror("Invalid mode");
        return 1;
    }
    if (chmod(filename, (mode_t)mode) != 0) {
        perror("Error changing mode of file");
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
    return our_chmod(argc, argv);
}
#endif
