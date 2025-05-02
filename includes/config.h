// CShell Project - Barrel includes
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef CONFIG_H
#define CONFIG_H


// Maximum length of a line for command
#define MAX_LINE_LENGTH 8192
// Maximum path length
#define MAX_PATH_LENGTH 4096
// current working directory
extern char CWD[MAX_PATH_LENGTH];
// History maximum size
#define MAX_HISTORY 100
// History array
extern char *history[MAX_HISTORY];
// History count
extern int history_count;


void initconfig();


#endif
