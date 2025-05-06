// CShell Project - New touch command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "touch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>


// Number of files to create
static int _touch_files_count = 0;
// List of files to create
static char **_touch_files = NULL;


/**
 * @see printf
 */
void _touch_print_usage(const char *const program_name) {
    printf("Usage: %s [Options] file1 [file2 ...]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
}


/**
 * @see perror, _touch_print_usage, sizeof, malloc, strcmp, strlen, strcpy
 */
int _touch_parse_arguments(const int argc, const char *const *const argv) {
    if (argc < 2) {
        perror("Error: No file specified");
        _touch_print_usage(argv[0]);
        return 1;
    }

    _touch_files = malloc(argc * sizeof(char *));

    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            _touch_print_usage(argv[0]);
            return -1;
        }

        // Add the file to the list
        _touch_files[_touch_files_count] = malloc(strlen(argv[i]) + 1);
        strcpy(_touch_files[_touch_files_count], argv[i]);
        _touch_files_count++;
    }

    return 0;
}


/**
 * @see free, _touch_parse_arguments, stat, utime, perror, printf, fopen, fclose
 */
int our_touch(const int argc, const char *const *const argv) {
    // free memory
    for (int i = 0; i < _touch_files_count; i++) free(_touch_files[i]);
    free(_touch_files);
    _touch_files = NULL;
    _touch_files_count = 0;

    // Parse the arguments
    int parse_result = _touch_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result) return parse_result;

    // Create every files
    const char *filename;
    struct stat st;
    for (int i = 0; i < _touch_files_count; i++) {
        filename = _touch_files[i];

        if (stat(filename, &st) == 0) {
            // File exists, update the timestamp
            if (utime(filename, NULL) != 0) {
                perror("Error: Unable to update timestamp");
                return -1;
            }
            printf("Updated timestamp for %s\n", filename);
        }
        else {
            // File does not exist, create it
            FILE *file = fopen(filename, "w");
            if (file == NULL) {
                perror("Error: Unable to create file");
                return -1;
            }
            fclose(file);
        }
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
    return our_touch(argc, argv);
}
#endif
