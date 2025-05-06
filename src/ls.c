// CShell Project - New ls command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "ls.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


/**
 * @see printf
 */
void _ls_print_usage(const char *const program_name) {
    printf("Usage: %s [Options]\n", program_name);
    printf("Options:\n");
    printf("    -h | --help    Print this help message\n");
    printf("    -l            List in long format\n");
    printf("    -a            List all files, including hidden ones\n");
}


/**
 * @see strcmp, _ls_print_usage
 */
int _ls_parse_arguments(const int argc, const char *const *const argv, int *const show_all, int *const long_format) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            _ls_print_usage(argv[0]);
            return -1;
        }

        else if (strcmp(argv[i], "-a") == 0) *show_all = 1;

        else if (strcmp(argv[i], "-l") == 0) *long_format = 1;

        else {
            perror("ls: option inconnue");
            return 1;
        }
    }

    return 0;
}


/**
 * @see _ls_parse_arguments, opendir, perror, readdir, printf, stat, S_ISDIR, getpwuid, getgrgid, localtime, strftime, closedir
 */
int our_ls(const int argc, const char *const *const argv) {
    int show_all = 0;
    int long_format = 0;

    int parse_result = _ls_parse_arguments(argc, argv, &show_all, &long_format);
    if (parse_result == -1) return 0;
    if (parse_result) return parse_result;

    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("ls");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') continue;

        if (!long_format) printf("%s  ", entry->d_name);
        else {
            struct stat info;
            if (stat(entry->d_name, &info) == -1) {
                perror("stat");
                continue;
            }

            // Type
            printf(S_ISDIR(info.st_mode) ? "d" : "-");

            // Permissions
            printf((info.st_mode & S_IRUSR) ? "r" : "-");
            printf((info.st_mode & S_IWUSR) ? "w" : "-");
            printf((info.st_mode & S_IXUSR) ? "x" : "-");
            printf((info.st_mode & S_IRGRP) ? "r" : "-");
            printf((info.st_mode & S_IWGRP) ? "w" : "-");
            printf((info.st_mode & S_IXGRP) ? "x" : "-");
            printf((info.st_mode & S_IROTH) ? "r" : "-");
            printf((info.st_mode & S_IWOTH) ? "w" : "-");
            printf((info.st_mode & S_IXOTH) ? "x" : "-");

            // Liens, utilisateur, groupe, taille
            printf(" %lu", info.st_nlink);
            struct passwd *pw = getpwuid(info.st_uid);
            struct group  *gr = getgrgid(info.st_gid);
            printf(" %s %s", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");
            printf(" %5ld", info.st_size);

            // Date
            char date[20];
            strftime(date, sizeof(date), "%b %d %H:%M", localtime(&info.st_mtime));
            printf(" %s", date);

            // Nom
            printf(" %s\n", entry->d_name);
        }
    }

    if (!long_format) printf("\n");

    closedir(dir);
    return 0;
}


#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING PURPOSES
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_ls(argc, argv);
}
#endif
