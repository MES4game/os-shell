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

int recursive_flag;
int verbose_flag;
int force_flag;

/**
 * @see fprintf
 */
void __rm_print_usage(char *program_name)
{
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
    fprintf(stdout, "    -r             Remove directories and their contents recursively\n");
    fprintf(stdout, "    -v             Enable verbose mode\n");
    fprintf(stdout, "    -f             Ignore nonexistent files and arguments, never prompt\n");
}

/**
 * @see strcmp, __rm_print_usage
 */
int __rm_parse_arguments(int argc, char *argv[])
{
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++)
    {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            // Print the usage and return
            __rm_print_usage(argv[0]);
            return -1;
        }

        if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            // Check for options
            for (int j = 1; argv[i][j] != '\0'; j++)
            {
                switch (argv[i][j])
                {
                case 'r':
                    recursive_flag = 1;
                    break;
                case 'v':
                    verbose_flag = 1;
                    break;
                case 'f':
                    force_flag = 1;
                    break;
                default:
                    fprintf(stderr, "Unknown option: -%c\n", argv[i][j]);
                    return -1;
                }
            }
        }
    }

    return 0;
}

int is_directory(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
    {
        return 0; // Assume not a directory on error
    }
    return S_ISDIR(path_stat.st_mode);
}

int remove_recursively(const char *path)
{
    DIR *dir = opendir(path);
    if (!dir)
    {
        if (force_flag)
        {
            return 0;
        }
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    char full_path[4096];

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip the "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (is_directory(full_path))
        {
            // Recursively remove the directory
            if (remove_recursively(full_path) != 0)
            {
                if (force_flag)
                {
                    return 0;
                }
                closedir(dir);
                return -1;
            }
        }
        else
        {
            // Remove the file
            if (remove(full_path) != 0)
            {
                if (!force_flag)
                {
                    perror("remove");
                    closedir(dir);
                }
                continue;
            }
            else if (verbose_flag)
            {
                printf("Removed file '%s'\n", full_path);
            }
        }
    }
    closedir(dir);
    // Remove the directory itself
    if (rmdir(path) != 0)
    {
        if (force_flag)
        {
            return 0;
        }
        perror("rmdir");
        return -1;
    }
    if (verbose_flag)
    {
        printf("Removed directory '%s'\n", path);
    }
    return 0;
}

/**
 * @see __rm_parse_arguments
 */
int our_rm(int argc, char *argv[])
{
    recursive_flag = 0;
    verbose_flag = 0;
    force_flag = 0;
    // Parse the arguments
    int parse_result = __rm_parse_arguments(argc, argv);
    if (parse_result == -1)
        return 0;
    if (parse_result)
        return parse_result;

    // TODO: Implement the rm command
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            // fprintf(stderr, "Warning: ignoring unsupported option '%s'\n", argv[i]);
            continue;
        }
        if (is_directory(argv[i]))
        {
            if (recursive_flag)
            {
                // Remove the directory recursively
                if (remove_recursively(argv[i]) != 0)
                {
                    if (force_flag)
                    {
                        return 0;
                    }
                    fprintf(stderr, "Error removing directory '%s': %s\n", argv[i], strerror(errno));
                    return -1;
                }
            }
            else
            {
                if (force_flag)
                {
                    return 0;
                }
                fprintf(stderr, "Error: '%s' is a directory. Use -r to remove it recursively.\n", argv[i]);
                return -1;
            }
        }
        else
        {
            if (remove(argv[i]) != 0)
            {
                if (!force_flag)
                {
                    fprintf(stderr, "Error removing file '%s': %s\n", argv[i], strerror(errno));
                }
                continue;
            }
            else if (verbose_flag)
            {
                printf("Removed file '%s'\n", argv[i]);
            }
        }
    }

    return 0;
}

#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[])
{
    // DO NOT TOUCH
    return our_rm(argc, argv);
}
#endif
