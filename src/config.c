// CShell Project - New cd command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "config.h"
#include <unistd.h>


char CWD[MAX_PATH_LENGTH];

char *history[MAX_HISTORY];
int history_count = 0;


void initconfig()
{
    getcwd(CWD, sizeof(CWD));
}
