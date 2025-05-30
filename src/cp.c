// CShell Project - New cp command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#define _GNU_SOURCE
#include "cp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


// Debug flag to print debug messages
static int _cp_DEBUG = 0;
// Space indentation for folders
static int _cp_SPACE = 0;
// Hidden flag to allow secret files/folders
static int _cp_HIDDEN = 0;
// Buffer size to read/write files
static int _cp_BUFFER_SIZE = 1024;


/**
 * @see strlen, malloc, strcpy, strcat, fprintf
 */
char *_cp_path_file_concat(const char *path, const char *name) {
    int path_size = strlen(path);

    // +2 because of the '/' and '\0'
    char *res = malloc(path_size + strlen(name) + 2);
    if (res == NULL) {
        if (_cp_DEBUG) fprintf(stderr, "%*s  No more memory to malloc\n", _cp_SPACE, "");
        return NULL;
    }

    strcpy(res, path);

    if (path_size != 0 && path[path_size - 1] != '/' && path[path_size - 1] != '\\') strcat(res, "/");

    strcat(res, name);
    return res;
}


/**
 * @see open, stat, chmod, copy_file_range, close, fprintf
 */
int _cp_copy_file(const char *input_path, const char *output_path) {
    if (_cp_DEBUG) fprintf(stdout, "%*s- Copying file %s to %s\n", _cp_SPACE, "", input_path, output_path);

    // Open the input file for reading
    int input_file  = open(input_path,  O_RDONLY);
    if (input_file == -1) {
        if (_cp_DEBUG) fprintf(stderr, "%*s  Error reading file %s\n", _cp_SPACE, "", input_path);
        return 1;
    }

    // Open the output file for writing (create and no overwrite)
    int output_file = open(output_path, O_WRONLY | O_CREAT | O_EXCL, 0660);
    if (output_file == -1) {
        if (_cp_DEBUG) fprintf(stderr, "%*s  Error writing in file %s\n", _cp_SPACE, "", output_path);
        return 1;
    }

    // Copy the permissions of the input file to the output file
    struct stat input_perms;
    if (stat(input_path, &input_perms) != 0 && _cp_DEBUG)          fprintf(stderr, "%*s  Error reading permissions of %s\n", _cp_SPACE, "", input_path);
    if (chmod(output_path, input_perms.st_mode) != 0 && _cp_DEBUG) fprintf(stderr, "%*s  Error setting permissions of %s\n", _cp_SPACE, "", output_path);

    // Copy loop
    off_t offset = 0;
    while (copy_file_range(input_file, &offset, output_file, NULL, _cp_BUFFER_SIZE, 0) > 0) {
        continue;
    }

    // Close the files
    close(input_file);
    close(output_file);

    return 0;
}


/**
 * @see opendir, stat, mkdir, chmod, readdir, closedir, free, _cp_path_file_concat, _cp_copy_file, fprintf
 */
int _cp_copy_dir(const char *input_dir, const char *output_dir) {
    if (_cp_DEBUG) fprintf(stdout, "%*s- Copying folder %s to %s\n", _cp_SPACE, "", input_dir, output_dir);

    // Open the input directory
    DIR *dir = opendir(input_dir);
    if (dir == NULL) {
        if (_cp_DEBUG) fprintf(stderr, "%*s  Error opening directory %s\n", _cp_SPACE, "", input_dir);
        return 1;
    }

    // Create the output directory
    if (mkdir(output_dir, 0660) != 0 && _cp_DEBUG) fprintf(stderr, "%*s  Error creating %s\n", _cp_SPACE, "", output_dir);

    // Copy the permissions of the input directory to the output directory
    struct stat input_stat;
    if (stat(input_dir, &input_stat) != 0 && _cp_DEBUG)          fprintf(stderr, "%*s  Error reading permissions of %s\n", _cp_SPACE, "", input_dir);
    if (chmod(output_dir, input_stat.st_mode) != 0 && _cp_DEBUG) fprintf(stderr, "%*s  Error setting permissions of %s\n", _cp_SPACE, "", output_dir);

    if (_cp_DEBUG) _cp_SPACE += 4;

    // Copy all the files and folders inside the input directory to the output directory
    struct dirent *current_child;
    char *input_path, *output_path;
    while (1) {
        // Get next child of the directory
        current_child = readdir(dir);
        if (current_child == NULL) break;

        // If __cp_HIDDEN : Skip . (itself) and .. (parent)
        // If not __cp_HIDDEN : Skip . (itself), .. (parent) and all files/folders starting with .
        if (current_child->d_name[0] == '.' && (!_cp_HIDDEN || current_child->d_name[1] == '\0' || (current_child->d_name[1] == '.' && current_child->d_name[2] == '\0'))) continue;

        input_path  = _cp_path_file_concat(input_dir,  current_child->d_name);
        if (input_path == NULL) return 2;
        output_path = _cp_path_file_concat(output_dir, current_child->d_name);
        if (output_path == NULL) return 2;

        // Get permissions of the file/folder to check if it is a directory
        if (stat(input_path, &input_stat)) {
            if (_cp_DEBUG) fprintf(stderr, "%*s  Error reading permissions of %s\n", _cp_SPACE, "", input_path);
            free(input_path);
            free(output_path);
            continue;
        }

        // Copy the file/folder
        if (S_ISDIR(input_stat.st_mode)) _cp_copy_dir(input_path, output_path);
        else                             _cp_copy_file(input_path, output_path);

        // Free the memory allocated by __cp_path_file_concat
        free(input_path);
        free(output_path);
    }

    // Close the directory
    closedir(dir);

    if (_cp_DEBUG) _cp_SPACE -= 4;

    return 0;
}


/**
 * @see printf
 */
void _cp_print_usage(const char *const program_name) {
    printf("Usage: %s <input_file_path> <output_file_path> [Options]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help      Print this help message\n");
    printf("    -v               Verbose, debug mode\n");
    printf("    -a               Allow copy of secret files/folders\n");
    printf("    --buffer SIZE    Number of bytes for the buffer to read/write files\n");
    printf("                     SIZE > 0 and by default is 4096 bytes\n");
}


/**
 * @see strcmp, _cp_print_usage
 */
int _cp_parse_arguments(const int argc, const char *const *const argv) {
    // Check if the number of arguments is correct
    if (argc < 3) {
        fprintf(stderr, "Missing arguments\n");
        _cp_print_usage(argv[0]);
        return 1;
    }

    // Check if the input file/folder exists
    struct stat input_stat;
    if (stat(argv[1], &input_stat) != 0) {
        fprintf(stderr, "Input file/folder does not exist\n");
        return 1;
    }

    // Set _cp_DEBUG, _cp_HIDDEN and _cp_BUFFER_SIZE
    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            _cp_print_usage(argv[0]);
            return -1;
        }

        // Check if the argument is -v
        if (strcmp(argv[i], "-v") == 0) {
            // Set __cp_DEBUG
            _cp_DEBUG = 1;
            continue;
        }

        // Check if the argument is -a
        if (strcmp(argv[i], "-a") == 0) {
            // Set __cp_HIDDEN
            _cp_HIDDEN = 1;
            continue;
        }

        // Check if the argument is --buffer
        if (strcmp(argv[i], "--buffer") == 0) {
            i++;

            // Check if there is an argument after --buffer
            if (i >= argc) {
                fprintf(stderr, "Missing SIZE\n");
                _cp_print_usage(argv[0]);
                return 1;
            }

            // Check if the argument is empty
            if (*argv[i] == '\0') continue;

            // Check if the argument is a number
            for (int j = 0; argv[i][j] != '\0'; j++) {
                if (!isdigit((unsigned char)argv[i][j])) {
                    fprintf(stderr, "Invalid SIZE: %s\n", argv[i]);
                    _cp_print_usage(argv[0]);
                    return 1;
                }
            }

            // Set the __cp_BUFFER_SIZE
            _cp_BUFFER_SIZE = atoi(argv[i]);

            // Check if the __cp_BUFFER_SIZE is valid
            if (_cp_BUFFER_SIZE <= 0) {
                fprintf(stderr, "Invalid SIZE: %i\n", _cp_BUFFER_SIZE);
                _cp_print_usage(argv[0]);
                return 1;
            }
        }
    }

    return 0;
}


/**
 * @see _cp_parse_arguments, S_ISDIR, _cp_copy_dir, _cp_copy_file
 */
int our_cp(const int argc, const char *const *const argv) {
    // Parse the arguments
    int parse_result = _cp_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result)       return parse_result;

    // Copy the file/folder
    struct stat input_stat;
    if (stat(argv[1], &input_stat) != 0 && _cp_DEBUG) fprintf(stderr, "%*s  Error reading permissions of %s\n", _cp_SPACE, "", argv[1]);

    int res;
    if (S_ISDIR(input_stat.st_mode)) res = _cp_copy_dir(argv[1], argv[2]);
    else                             res = _cp_copy_file(argv[1], argv[2]);

    return res;
}


#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_cp(argc, argv);
}
#endif
