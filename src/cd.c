// CShell Project - New cd command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "config.h"
#include "cd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>


/**
 * @see printf
 */
void _cd_print_usage(const char *const program_name) {
    printf("Usage: %s [Options]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
}


/**
 * @see strcmp, _cd_print_usage
 */
int _cd_parse_arguments(const int argc, const char *const *const argv) {
    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            _cd_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}


/**
 * @see _cd_parse_arguments
 */
int our_cd(const int argc, const char *const *const argv) {
    // Parse the arguments
    int parse_result = _cd_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result)       return parse_result;

    const char *path = argv[1];
    if (argc == 1 || ((argc == 2) && (strcmp(argv[1], "~") == 0))) {
        path = getenv("HOME");
        if (path == NULL) {
            perror("cd: HOME not set");
            return 1;
        }
    }

    if ((argc == 2) && strcmp(argv[1], "-") == 0) {
        if (PWD[0] == '\0') {
            perror("cd: no previous directory");
            return 1;
        }
        path = PWD;
    }

    if (chdir(path) == 0) strncpy(PWD, CWD, MAX_PATH_LENGTH);
    else perror("cd error");

    return 0;
}


#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_cd(argc, argv);
}
#endif
