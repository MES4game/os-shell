# OS-SHELL

## How to use
- to run the complete project:
```bash
gcc main.c src/*.c -Iincludes -Wall -g -o program.exe
./program.exe
```
- to run a command of the project:
```bash
gcc src/func.c -Iincludes -Wall -g -o program.exe
./program.exe [options]
```

## How to code
- to add a new command:
  1. add the command in the `src` folder (e.g. `src/func.c`) with this skeleton:
```c
// CShell Project - New func command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#include "func.h"
#include <stdio.h>
#include <string.h>


/**
 * @see fprintf
 */
void __func_print_usage(char *program_name) {
    // TODO: Implement the print_usage function
    fprintf(stdout, "Usage: %s [Options]\n", program_name);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    -h | --help    Print this help message\n");
}


/**
 * @see strcmp, __func_print_usage
 */
int __func_parse_arguments(int argc, char *argv[]) {
    // TODO: Implement the parse_arguments function
    for (int i = 1; i < argc; i++) {
        // Check if the argument is -h or --help
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            // Print the usage and return
            __func_print_usage(argv[0]);
            return -1;
        }
    }
}


/**
 * @see __func_parse_arguments
 */
int our_func(int argc, char *argv[]) {
    // Parse the arguments
    int parse_result = __func_parse_arguments(argc, argv);
    if (parse_result == -1) return 0;
    if (parse_result)       return parse_result;

    // TODO: Implement the func command

    return 0;
}


#ifdef TEST_MAIN
/**
 * ONLY FOR TESTING ALONE
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    return our_func(argc, argv);
}
#endif
```
  2. add the header of the command in the `includes` folder (e.g. `includes/func.h`) with this skeleton:
```c
// CShell Project - New func command
// Author: Maxime DAUPHIN, Andrew ZIADEH and Abbas ALDIRANI
// Date: 2025-03-17


#ifndef COMMAND_FUNC_H
#define COMMAND_FUNC_H


/**
 * @brief Print the usage of the program.
 * @param program_name The name of the program.
 */
void __func_print_usage(char *program_name);


/**
 * @brief Parse the arguments of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if arguments are good, -1 if -h or --help used.
 */
int __func_parse_arguments(int argc, char *argv[]);


/**
 * @brief Main function of the program.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return 0 if the program ran successfully.
 */
int our_func(int argc, char *argv[]);


#endif
```
  3. add the header in the `includes/all.h` file:
```c
#include "func.h"
```
  4. add the command in `call_command` function in `main.c` file:
```c
// TODO: see how to implement run of personal commands
```
- to add a new global variable to a command:
  1. add the variable in the command file (e.g. `src/func.c`) with this skeleton:
```c
type_of_variable variable_name = value;
```
  2. add the variable in the header file (e.g. `includes/func.h`) with this skeleton:
```c
// A description of the variable
extern type_of_variable variable_name;
```


## TODO
- [ ] make `main.c` works (for now the shell do nothing)
- [ ] implement the run of personal commands in `call_command` function in `main.c`
- [ ] implement redirection in `call_command` function in `main.c`
- [ ] implement pipe in `call_command` function in `main.c`
- [ ] implement every command in src
- [ ] rework function `parse_line` in `main.c`
- [ ] rework display of the prompt
- [ ] add an history of commands
- [ ] add a function to manage the history of commands
- [ ] add load of command from history with up and down arrows
- [ ] add possibility to use left and right arrows to move in the command line
- [ ] add a makefile
