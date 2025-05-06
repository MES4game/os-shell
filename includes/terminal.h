// CShell Project - New terminal interface
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17

#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * @brief Enables raw mode for the terminal, to allow key-by-key input reading.
 */
void enable_raw_mode();

/**
 * @brief Display terminal and get stdin for command
 * @return 1 if the character read is '\n', 0 otherwise.
 */
int our_terminal();

#endif
