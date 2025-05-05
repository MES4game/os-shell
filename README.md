# OS-SHELL

## How to use
- to compile and execute the complete project:
```bash
gcc main.c src/*.c -Iincludes -Wall -g -o program.exe
./program.exe
```

## How to code
- to add a new command:
  1. add the command in the `src` folder (e.g. `src/func.c`)
  2. add the header of the command in the `includes` folder (e.g. `includes/func.h`)
  3. add the header in the `includes/main.h` file
  4. add the command in `call_command` function in `main.c` file
- to add a new global variable to a command:
  1. add the variable in the command file (e.g. `src/func.c`) as `static`
- to add a new global variable to the project:
  1. add the variable in the config file (e.g. `includes/config.h`) as `extern`
  2. declare the variable in the config file (e.g. `src/config.c`)


## TODO
- [ ] rework every function in `main.c` (comment, styling, etc.)
- [ ] rework every command in `src` (fragment code, use `const`, use `config.c` for globals, etc.)
- [ ] add intro when executing the project
- [ ] look back at every include in files (no unused lib)
