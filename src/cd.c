// CShell Project - New cd command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "cd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>


char __cd_CWD[__cd_MAX_PATH_LEN];
char __cd_PWD[__cd_MAX_PATH_LEN];


/**
 * @see fprintf
 */
void __cd_print_usage(char *program_name) {
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}


/**
 * @see strcmp, __cd_print_usage
 */
int __cd_parse_arguments(int argc, char *argv[]) {
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            __cd_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}


/**
 * @see __cd_parse_arguments
 */
int our_cd(int argc, char *argv[]) {
    // Parse the arguments
    int parse_result = __cd_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result)       return parse_result;

    char *path = argv[1];
    if (argc == 1 || ((argc == 2) && (strcmp(argv[1], "~") == 0))) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }

    if ((argc == 2) && strcmp(argv[1], "-") == 0) {
        if (__cd_PWD[0] == '\0') {
            fprintf(stderr, "cd: no previous directory\n");
            return 1;
        }
        path = __cd_PWD;
    }

    char tmp[__cd_MAX_PATH_LEN];

    if (getcwd(tmp, sizeof(tmp)) == NULL) {
        fprintf(stderr, "cd: error getting current working directory\n");
        return 1;
    }

    if (chdir(path) == 0) {
        if (getcwd(__cd_CWD, sizeof(__cd_CWD)) != NULL) {
            // printf("Current working directory: %s\n", __cd_CWD);
            strcpy(__cd_PWD, tmp);
        }
        else {
            fprintf(stderr, "cd: error getting current working directory\n");
            return 1;
        }
    }
    else {
        perror("cd error");
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
    return our_cd(argc, argv);
}
#endif
