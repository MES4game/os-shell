// CShell Project - Configuration of variables
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef CONFIG_H
#define CONFIG_H

// Maximum length of an environment variable name
#define MAX_ENV_NAME_LENGTH 256
// Maximum length of a line for command
#define MAX_LINE_LENGTH 4096
// Maximum length of a path
#define MAX_PATH_LENGTH 1024
// Command line
extern char COMMAND[MAX_LINE_LENGTH];
// History size
#define HISTORY_SIZE 50
// History array
extern char HISTORY[HISTORY_SIZE][MAX_LINE_LENGTH];
// current working directory
extern char CWD[MAX_PATH_LENGTH];
// previous working directory
extern char PWD[MAX_PATH_LENGTH];
// current user name
extern char USER[MAX_ENV_NAME_LENGTH];

#endif
