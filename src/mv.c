// CShell Project - New mv command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "mv.h"
#include "cp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


/**
 * @see printf
 */
void _mv_print_usage(const char *const program_name) {
    printf("Usage: %s [Options] old_path new_path\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
}


/**
 * @see strcmp, _mv_print_usage
 */
int _mv_parse_arguments(const int argc, const char *const *const argv) {
    if (argc < 3) {
        // Print the usage and return
        _mv_print_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            _mv_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}


/**
 * @see _mv_parse_arguments, rename, printf, our_cp, perror, unlink
 */
int our_mv(const int argc, const char *const *const argv) {
    // Parse the arguments
    int parse_result = _mv_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result) return parse_result;

    const char *const source = argv[1];
    const char *const destination = argv[2];

    if (rename(source, destination) == 0) {
        printf("Moved '%s' to '%s'\n", source, destination);
        return 0;
    }

    // If rename fails, try to copy and then delete the source
    if (our_cp(argc, argv) != 0) {
        perror("Error: Failed to copy");
        return 1;
    }

    if (unlink(source) != 0) {
        perror("Error: Failed to delete source file");
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
    return our_mv(argc, argv);
}
#endif
