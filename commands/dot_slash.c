#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

/**
 * Affiche le prompt du shell.
 */
void print_prompt() {
    printf("my-shell> ");
    fflush(stdout); 
}

/**
 * Exécute une commande tapée dans le shell.
 */
void execute_command(char **args) {
    if (args[0] == NULL) return;

    // Cas : "./"
    if (strcmp(args[0], "./") == 0 && args[1] == NULL) {
        fprintf(stderr, "Erreur : './' est un répertoire, pas un programme.\n");
        return;
    }

    // Cas : "./ --help"
    if (strcmp(args[0], "./") == 0 && args[1] && strcmp(args[1], "--help") == 0) {
        printf("Usage: ./nom_programme [arguments]\n");
        printf("Ce shell exécute les programmes depuis le dossier courant.\n");
        return;
    }

    // Fork pour exécuter la commande
    pid_t pid = fork();
    if (pid < 0) {
        perror("Erreur fork");
        return;
    }

    if (pid == 0) {
        execvp(args[0], args);
        perror("Erreur exec");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * Fonction de test automatisé du shell.
 */
int test() {
    printf("========= Test Shell ./ =========\n");

    // Test 1 : ./ seul
    char *argv1[] = {"./", NULL};
    printf("[Test 1] ./ seul (doit afficher erreur)...\n");
    execute_command(argv1);

    // Test 2 : ./ --help
    char *argv2[] = {"./", "--help", NULL};
    printf("[Test 2] ./ --help (doit afficher l’aide)...\n");
    execute_command(argv2);

    // Test 3 : programme inexistant
    char *argv3[] = {"./faux_programme", NULL};
    printf("[Test 3] ./faux_programme (doit afficher erreur exec)...\n");
    execute_command(argv3);

    // Test 4 : vrai programme compilé
    char *argv4[] = {"./test_program", "arg1", NULL};
    printf("[Test 4] ./test_program arg1 (si existe)...\n");
    execute_command(argv4);

    printf("========= Fin des tests =========\n");
    return 0;
}

/**
 * Boucle interactive du shell.
 */
void shell_loop() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (1) {
        print_prompt();

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        // Nettoyage du saut de ligne
        line[strcspn(line, "\n")] = '\0';

        // Split ligne en arguments
        int i = 0;
        char *token = strtok(line, " ");
        while (token && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Commande spéciale : exit
        if (args[0] && strcmp(args[0], "exit") == 0) {
            break;
        }

        execute_command(args);
    }
}

/**
 * ONLY FOR COMPILATION
 * DO NOT TOUCH
 */
int main(int argc, char *argv[]) {
    // DO NOT TOUCH
    //return test();
    shell_loop();
}


