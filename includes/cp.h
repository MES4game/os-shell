// CShell Project - New cp command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef COMMAND_CP_H
#define COMMAND_CP_H

/**
 * @brief Concatenates a folder path with a file/folder name.
 * @param path The parent folder path.
 * @param name The file/folder name.
 * @return The path to file/folder, NULL if malloc error.
 * @note This function handles missing of '/' at the end of path.
 * @warning It does not check if its return is an existing file/folder.
 */
char *_cp_path_file_concat(const char *path, const char *name);

/**
 * @brief Copy a file from one path to another.
 * It uses `copy_file_range` from _GNU_SOURCE to copy the file.
 * @param input_path The input path from where to read the file.
 * @param output_path The output path where to write the file.
 * @return 0 if the file was copied successfully, 1 if input file does not exist, 2 if output file already exists.
 * @note This function handles permissions.
 */
int _cp_copy_file(const char *input_path, const char *output_path);

/**
 * @brief Copy a file from one path to another.
 * @param input_dir The input path from where to read the folder.
 * @param output_dir The output path where to write the folder.
 * @return 0 if the folder was copied successfully, 1 if input folder does not exist, 2 if malloc error.
 * @note This function handles permissions.
 * @warning It does not handle the case where the output file already exists.
 * @warning It does not check if the input file exists.
 */
int _cp_copy_dir(const char *input_dir, const char *output_dir);

/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void _cp_print_usage(const char *const program_name);

/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int _cp_parse_arguments(const int argc, const char *const *const argv);

/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_cp(const int argc, const char *const *const argv);

#endif
