// CShell Project - New terminal interface
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "config.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>


// Current line to read stdin
static char CUR_LINE[MAX_LINE_LENGTH] = { '\0' };
// Length of the current line
static int CUR_LINE_LENGTH = 0;
// Position of descriptor in the current line
static int CUR_DESC = 0;
// Position in history
static int HISTORY_INDEX = -1;


/**
 * @see tcgetattr, disable_raw_mode, tcsetattr
 */
void enable_raw_mode() {
    // Make a copy of the current settings to modify
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);

    // Disable echoing and canonical mode
    raw.c_lflag &= ~(ECHO | ICANON);
    // Set read to return as soon as a single character is available
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    // Apply new terminal
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


/**
 * @see read, memmove, strncpy, strlen, memset, write, getuid, getpwuid, sizeof, malloc, snprintf
 */
int our_terminal() {
    // get the next character from stdin
    char c;
    read(STDIN_FILENO, &c, 1);

    // Erase character in line at descriptor position
    if (c == 127) {
        if (CUR_DESC > 0) {
            // Move substring after descriptor by one on left (overwrite character at descriptor position)
            memmove(&CUR_LINE[CUR_DESC - 1], &CUR_LINE[CUR_DESC], CUR_LINE_LENGTH - CUR_DESC);

            // Decrement length and descriptor, and make sure last character of line is '\0'
            CUR_LINE[--CUR_LINE_LENGTH] = '\0';
            CUR_DESC--;
        }
    }

    // Parse for special characters (only arrow for now)
    else if (c == 27) {
        // read two next characters to know wich special character it is
        char seq[2];
        read(STDIN_FILENO, &seq[0], 1);
        read(STDIN_FILENO, &seq[1], 1);

        if (seq[0] == '[') {
            // Case left arrow, decrement descriptor if not at start of line
            if (seq[1] == 'D') { if (CUR_DESC > 0) CUR_DESC--; }

            // Case right arrow, increment descriptor if not at end of line
            else if (seq[1] == 'C') { if (CUR_DESC < CUR_LINE_LENGTH) CUR_DESC++; }

            // Case up arrow, get previous command in history
            else if (seq[1] == 'A') {
                // Increment index and clip it to last index (no segfault)
                if (++HISTORY_INDEX >= HISTORY_SIZE) HISTORY_INDEX = HISTORY_SIZE - 1;

                // Decrement index if corresponding history command is a void string
                while (HISTORY_INDEX > 0 && !HISTORY[HISTORY_INDEX][0]) HISTORY_INDEX--;

                // Copy corresponding command from history into line
                strncpy(CUR_LINE, HISTORY[HISTORY_INDEX], MAX_LINE_LENGTH);
                CUR_LINE_LENGTH = CUR_DESC = strlen(CUR_LINE);
            }

            // Case down arrow, get next command in history
            else if (seq[1] == 'B') {
                // Decrement index and clip it to -1
                if (--HISTORY_INDEX < 0) {
                    HISTORY_INDEX = -1;
                    // Reset line to void string
                    memset(CUR_LINE, '\0', MAX_LINE_LENGTH);
                }

                // Copy corresponding command from history into line
                else strncpy(CUR_LINE, HISTORY[HISTORY_INDEX], MAX_LINE_LENGTH);

                CUR_LINE_LENGTH = CUR_DESC = strlen(CUR_LINE);
            }
        }
    }

    // Add line to history and reset it after
    else if (c == '\n') {
        // Print new line in stdout to disable last '\r'
        write(STDOUT_FILENO, "\n", 1);

        // Add line to history
        if (CUR_LINE_LENGTH > 0) {
            // Goes to last registered command
            int count = 0;
            while (count < HISTORY_SIZE && HISTORY[count][0]) count++;
            if (count >= HISTORY_SIZE) count = HISTORY_SIZE - 1;

            // Move by one on right every command in history
            for (int i = count; i > 0; i--) strncpy(HISTORY[i], HISTORY[i - 1], MAX_LINE_LENGTH);

            // Add line at start of history
            strncpy(HISTORY[0], CUR_LINE, MAX_LINE_LENGTH);
        }

        // Send line to main.c
        strncpy(COMMAND, CUR_LINE, MAX_LINE_LENGTH);
        // Reset line attributes
        memset(CUR_LINE, '\0', MAX_LINE_LENGTH);
        CUR_LINE_LENGTH = CUR_DESC = 0;
        HISTORY_INDEX = -1;

        return 1;
    }

    // add normal characters to the line
    else {
        if (CUR_LINE_LENGTH < MAX_LINE_LENGTH - 1) {
            // Move by one on right the substring after descriptor
            memmove(&CUR_LINE[CUR_DESC + 1], &CUR_LINE[CUR_DESC], CUR_LINE_LENGTH - CUR_DESC);

            // Add the character at descriptor position
            CUR_LINE[CUR_DESC] = c;
            CUR_LINE_LENGTH++;
            CUR_DESC++;
        }
    }

    // Get current user name
    struct passwd *pw = getpwuid(getuid());
    char *user = pw ? pw->pw_name : "no user";

    // Length of what is displayed (for descriptor position in display)
    int prompt_length = strlen(user) + strlen(CWD) + 10;
    // Length of the string (with invisible characters)
    int prompt_size = prompt_length + 34;

    // Create basic string for terminal
    char *prompt = malloc((prompt_size + 1) * sizeof(char));
    snprintf(prompt, prompt_size, "\033[1;32m%s\033[0m@\033[1;32mCShell\033[0m:\033[1;34m%s\033[0m> ", user, CWD);
    prompt[prompt_size + 1] = '\0';

    // display basic string and replace previous
    write(STDOUT_FILENO, "\r\033[K", 4);
    write(STDOUT_FILENO, prompt, strlen(prompt));
    free(prompt);

    // display line writen by user
    write(STDOUT_FILENO, CUR_LINE, CUR_LINE_LENGTH);

    // Set descriptor position in stdout
    char buf[64];
    snprintf(buf, sizeof(buf), "\r\033[%dC", CUR_DESC + prompt_length);
    write(STDOUT_FILENO, buf, strlen(buf));

    return 0;
}
