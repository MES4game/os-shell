// Project - New cp command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>


// Debug flag to print debug messages
int __cp_DEBUG = 0;

// Space indentation for folders
int __cp_SPACE = 0;

// Hidden flag to allow secret files/folders
int __cp_HIDDEN = 0;

// Buffer size to read/write files
int __cp_BUFFER_SIZE = 1024;


/**
 * @brief Concatenates a folder path with a file/folder name.
 *
 * This function takes two strings and returns their concatenation.
 *
 * @param path The parent folder path.
 * @param name The file/folder name.
 * @return The path to file/folder.
 * @note This function handles missing of '/' at the end of path.
 * @warning It does not check if its return is an existing file/folder.
 * @see strlen, malloc, strcpy, strcat, exit, fprintf
 */
char *__cp_path_file_concat(const char *path, const char *name) {
    int path_size = strlen(path);

    // +2 because of the '/' and '\0'
    char *res = malloc(path_size + strlen(name) + 2);
    if (res == NULL) {
        if (__cp_DEBUG) fprintf(stderr, "%*s  No more memory to malloc\n", __cp_SPACE, "");
        exit(1);
    }

    strcpy(res, path);

    if (path_size != 0 && path[path_size - 1] != '/' && path[path_size - 1] != '\\') strcat(res, "/");

    strcat(res, name);
    return res;
}


/**
 * @brief Copy a file from one path to another.
 *
 * This function takes two paths and copies the file from the first to the second.
 * It uses `copy_file_range` from _GNU_SOURCE to copy the file.
 *
 * @param input_path The input path from where to read the file.
 * @param output_path The output path where to write the file.
 * @return 0 if the file was copied successfully. 1 if input file does not exist. 2 if output file already exists.
 * @note This function handles permissions.
 * @see open, stat, chmod, copy_file_range, close, fprintf
 */
int __cp_copy_file(const char *input_path, const char *output_path) {
    if (__cp_DEBUG) fprintf(stdout, "%*s- Copying file %s to %s\n", __cp_SPACE, "", input_path, output_path);

    // Open the input file for reading
    int input_file  = open(input_path,  O_RDONLY);
    if (input_file == -1) {
        if (__cp_DEBUG) fprintf(stderr, "%*s  Error reading file %s\n", __cp_SPACE, "", input_path);
        return 1;
    }

    // Open the output file for writing (create and no overwrite)
    int output_file = open(output_path, O_WRONLY | O_CREAT | O_EXCL, 0660);
    if (output_file == -1) {
        if (__cp_DEBUG) fprintf(stderr, "%*s  Error writing in file %s\n", __cp_SPACE, "", output_path);
        return 1;
    }

    // Copy the permissions of the input file to the output file
    struct stat input_perms;
    if (stat(input_path, &input_perms) != 0 && __cp_DEBUG)          fprintf(stderr, "%*s  Error reading permissions of %s\n", __cp_SPACE, "", input_path);
    if (chmod(output_path, input_perms.st_mode) != 0 && __cp_DEBUG) fprintf(stderr, "%*s  Error setting permissions of %s\n", __cp_SPACE, "", output_path);

    // Copy loop
    off_t offset = 0;
    while (copy_file_range(input_file, &offset, output_file, NULL, __cp_BUFFER_SIZE, 0) > 0) {
        continue;
    }

    // Close the files
    close(input_file);
    close(output_file);

    return 0;
}


/**
 * @brief Copy a file from one path to another.
 *
 * This function takes two paths and copies the file from the first to the second.
 * It also copies all the files and folders inside the input folder to the output folder.
 *
 * @param input_dir The input path from where to read the folder.
 * @param output_dir The output path where to write the folder.
 * @return
 * @note This function handles permissions.
 * @warning It does not handle the case where the output file already exists.
 * @warning It does not check if the input file exists.
 * @see opendir, stat, mkdir, chmod, readdir, closedir, free, __cp_path_file_concat, __cp_copy_file, fprintf
 */
int __cp_copy_dir(const char *input_dir, const char *output_dir) {
    if (__cp_DEBUG) fprintf(stdout, "%*s- Copying folder %s to %s\n", __cp_SPACE, "", input_dir, output_dir);

    // Open the input directory
    DIR *dir = opendir(input_dir);
    if (dir == NULL) {
        if (__cp_DEBUG) fprintf(stderr, "%*s  Error opening directory %s\n", __cp_SPACE, "", input_dir);
        return 1;
    }

    // Create the output directory
    if (mkdir(output_dir, 0660) != 0 && __cp_DEBUG) fprintf(stderr, "%*s  Error creating %s\n", __cp_SPACE, "", output_dir);

    // Copy the permissions of the input directory to the output directory
    struct stat input_stat;
    if (stat(input_dir, &input_stat) != 0 && __cp_DEBUG)          fprintf(stderr, "%*s  Error reading permissions of %s\n", __cp_SPACE, "", input_dir);
    if (chmod(output_dir, input_stat.st_mode) != 0 && __cp_DEBUG) fprintf(stderr, "%*s  Error setting permissions of %s\n", __cp_SPACE, "", output_dir);

    if (__cp_DEBUG) __cp_SPACE += 4;

    // Copy all the files and folders inside the input directory to the output directory
    struct dirent *current_child;
    char *input_path, *output_path;
    while (1) {
        // Get next child of the directory
        current_child = readdir(dir);
        if (current_child == NULL) break;

        // If __cp_HIDDEN : Skip . (itself) and .. (parent)
        // If not __cp_HIDDEN : Skip . (itself), .. (parent) and all files/folders starting with .
        if (current_child->d_name[0] == '.' && (!__cp_HIDDEN || current_child->d_name[1] == '\0' || (current_child->d_name[1] == '.' && current_child->d_name[2] == '\0'))) continue;

        input_path  = __cp_path_file_concat(input_dir,  current_child->d_name);
        output_path = __cp_path_file_concat(output_dir, current_child->d_name);

        // Get permissions of the file/folder to check if it is a directory
        if (stat(input_path, &input_stat)) {
            if (__cp_DEBUG) fprintf(stderr, "%*s  Error reading permissions of %s\n", __cp_SPACE, "", input_path);
            free(input_path);
            free(output_path);
            continue;
        }

        // Copy the file/folder
        if (S_ISDIR(input_stat.st_mode)) __cp_copy_dir(input_path, output_path);
        else                             __cp_copy_file(input_path, output_path);

        // Free the memory allocated by __cp_path_file_concat
        free(input_path);
        free(output_path);
    }

    // Close the directory
    closedir(dir);

    if (__cp_DEBUG) __cp_SPACE -= 4;

    return 0;
}


/**
 * @brief Print the usage of the program.
 *
 * This function prints the usage of the program.
 *
 * @param program_name The name of the program.
 * @return
 * @see fprintf
 */
void __cp_print_usage(char *program_name) {
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s <input_file_path> <output_file_path> [-v] [-a] [--buffer SIZE]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help      Print this help message\n");
    fprintf(stdout, "    -v               Verbose, debug mode\n");
    fprintf(stdout, "    -a               allow copy of secret files/folders\n");
    fprintf(stdout, "    --buffer SIZE    Number of bytes for the buffer to read/write files\n");
    fprintf(stdout, "                     SIZE > 0 and by default is 4096 bytes\n");
}


/**
 * @brief Parse the arguments of the program.
 *
 * This function parses the arguments of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return
 * @see frprintf, __cp_print_usage, exit, strcmp, isdigit
 */
void __cp_parse_arguments(int argc, char *argv[]) {
    // Check if the number of arguments is correct
    if (argc < 3) {
        fprintf(stderr, "Missing arguments\n");
        __cp_print_usage(argv[0]);
        exit(1);
    }

    // Set DEBUG and BUFFER_SIZE
    for (int i = 3; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and exit
            __cp_print_usage(argv[0]);
            exit(0);
        }

        // Check if the argument is -v
        if (strcmp(argv[i], "-v") == 0) {
            // Set __cp_DEBUG
            __cp_DEBUG = 1;
            continue;
        }

        // Check if the argument is -a
        if (strcmp(argv[i], "-a") == 0) {
            // Set __cp_HIDDEN
            __cp_HIDDEN = 1;
            continue;
        }

        // Check if the argument is --buffer
        if (strcmp(argv[i], "--buffer") == 0) {
            i++;

            // Check if there is an argument after --buffer
            if (i >= argc) {
                fprintf(stderr, "Missing SIZE\n");
                __cp_print_usage(argv[0]);
                exit(1);
            }

            // Check if the argument is empty
            if (*argv[i] == '\0') continue;

            // Check if the argument is a number
            for (int j = 0; argv[i][j] != '\0'; j++) {
                if (!isdigit((unsigned char)argv[i][j])) {
                    fprintf(stderr, "Invalid SIZE: %s\n", argv[i]);
                    __cp_print_usage(argv[0]);
                    exit(1);
                }
            }

            // Set the __cp_BUFFER_SIZE
            __cp_BUFFER_SIZE = atoi(argv[i]);

            // Check if the __cp_BUFFER_SIZE is valid
            if (__cp_BUFFER_SIZE <= 0) {
                fprintf(stderr, "Invalid SIZE: %i\n", __cp_BUFFER_SIZE);
                __cp_print_usage(argv[0]);
                exit(1);
            }
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
 * @see __cp_parse_arguments, stat, fprintf, S_ISDIR, __cp_copy_dir, __cp_copy_file
 */
int our_cp(int argc, char *argv[]) {
    // Parse the arguments
    __cp_parse_arguments(argc, argv);

    // Check if the input file/folder exists
    struct stat input_stat;
    if (stat(argv[1], &input_stat) != 0) {
        fprintf(stderr, "Input file/folder does not exist\n");
        return 1;
    }

    // Copy the file/folder
    int res;
    if (S_ISDIR(input_stat.st_mode)) res = __cp_copy_dir(argv[1], argv[2]);
    else                             res = __cp_copy_file(argv[1], argv[2]);

    return res;
}


/**
 * ONLY FOR COMPILATION
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_cp(argc, argv);
}
