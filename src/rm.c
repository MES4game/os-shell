// CShell Project - New rm command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "rm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>


static int _rm_recursive_flag;
static int _rm_verbose_flag;
static int _rm_force_flag;


/**
 * @see printf
 */
void _rm_print_usage(const char *const program_name) {
    printf("Usage: %s [Options]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
    printf("    -r             Remove directories and their contents recursively\n");
    printf("    -v             Enable verbose mode\n");
    printf("    -f             Ignore nonexistent files and arguments, never prompt\n");
}


/**
 * @see strcmp, _rm_print_usage, perror
 */
int _rm_parse_arguments(const int argc, const char *const *const argv) {
    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            _rm_print_usage(argv[0]);
            return -1;
        }

        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            // Check for options
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'r':
                        _rm_recursive_flag = 1;
                        break;
                    case 'v':
                        _rm_verbose_flag = 1;
                        break;
                    case 'f':
                        _rm_force_flag = 1;
                        break;
                    default:
                        perror("Unknown option");
                        return -1;
                }
            }
        }
    }

    return 0;
}


/**
 * @see stat, S_ISDIR
 */
int _rm_is_directory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return 0; // Assume not a directory on error
    return S_ISDIR(path_stat.st_mode);
}


/**
 * @see opendir, perror, readdir, strcmp, snprintf, _rm_is_directory, remove_recursively, closedir, remove, printf, rmdir
 */
int _rm_remove_recursively(const char *path) {
    // Open the directory
    DIR *dir = opendir(path);
    if (!dir) {
        if (_rm_force_flag) return 0;
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    char full_path[4096];

    // Read the directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip the "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (_rm_is_directory(full_path)) {
            // Recursively remove the directory
            if (_rm_remove_recursively(full_path) != 0) {
                if (_rm_force_flag) return 0;
                closedir(dir);
                return -1;
            }
        }
        else {
            // Remove the file
            if (remove(full_path) != 0) {
                if (!_rm_force_flag) {
                    perror("remove");
                    closedir(dir);
                }
                continue;
            }
            else if (_rm_verbose_flag) printf("Removed file '%s'\n", full_path);
        }
    }

    closedir(dir);

    // Remove the directory itself
    if (rmdir(path) != 0) {
        if (_rm_force_flag) return 0;
        perror("rmdir");
        return -1;
    }

    if (_rm_verbose_flag) printf("Removed directory '%s'\n", path);

    return 0;
}


/**
 * @see _rm_parse_arguments, _rm_is_directory, _rm_remove_recursively, perror, remove, printf
 */
int our_rm(const int argc, const char *const *const argv) {
    // Reset the flags
    _rm_recursive_flag = 0;
    _rm_verbose_flag = 0;
    _rm_force_flag = 0;

    // Parse the arguments
    int parse_result = _rm_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result) return parse_result;

    // remove every file or directory given in the arguments
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') continue;

        if (_rm_is_directory(argv[i])) {
            // Remove the directory recursively
            if (_rm_recursive_flag) {
                if (_rm_remove_recursively(argv[i]) != 0)
                {
                    if (_rm_force_flag) return 0;
                    perror("Error removing directory");
                    return -1;
                }
            }
            else {
                if (_rm_force_flag) return 0;
                perror("Error: removing directory without -r");
                return -1;
            }
        }
        else {
            // Remove the file
            if (remove(argv[i]) != 0) {
                if (_rm_force_flag) continue;
                perror("Error removing file");
                return 1;
            }
            else if (_rm_verbose_flag) printf("Removed file '%s'\n", argv[i]);
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
    return our_rm(argc, argv);
}
#endif
