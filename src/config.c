// CShell Project - Configuration of variables
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "config.h"
#include <unistd.h>
#include <string.h>
#include <termios.h>


char COMMAND[MAX_LINE_LENGTH] = { '\0' };
char HISTORY[HISTORY_SIZE][MAX_LINE_LENGTH] = { { '\0' } };
char CWD[MAX_PATH_LENGTH] = { '\0' };
char PWD[MAX_PATH_LENGTH] = { '\0' };
char USER[MAX_ENV_NAME_LENGTH] = { '\0' };
