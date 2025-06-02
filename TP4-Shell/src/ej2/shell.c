
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

void trim(char *s) {        
    while (*s == ' ' || *s == '\t') s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t')) *end-- = '\0';
}

// Elimina comillas dobles o simples si están en los extremos
void strip_quotes(char *s) {
    int len = strlen(s);
    if (len >= 2 && ((s[0] == '"' && s[len - 1] == '"') || (s[0] == '\'' && s[len - 1] == '\''))) {
        s[len - 1] = '\0';
        memmove(s, s + 1, len); // Corre todo a la izquierda 1 carácter
    }
}

int main() {
    char command[256];
    char *commands[MAX_COMMANDS];

    while (1) {
        printf("Shell> ");
        if (!fgets(command, sizeof(command), stdin)) break;
        command[strcspn(command, "\n")] = '\0';
        trim(command);

        if (strcmp(command, "exit") == 0) break;

        int command_count = 0;
        char *token = strtok(command, "|");
        while (token != NULL && command_count < MAX_COMMANDS) {
            trim(token);
            commands[command_count++] = strdup(token);
            token = strtok(NULL, "|");
        }

        int pipes[MAX_COMMANDS - 1][2];
        for (int i = 0; i < command_count - 1; i++) pipe(pipes[i]);

        for (int i = 0; i < command_count; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
                if (i < command_count - 1) dup2(pipes[i][1], STDOUT_FILENO);
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                char *args[MAX_ARGS];
                int argc = 0;
                char *arg = strtok(commands[i], " ");
                while (arg && argc < MAX_ARGS - 1) {
                    strip_quotes(arg);               
                    args[argc++] = arg;
                    arg = strtok(NULL, " ");
                }
                args[argc] = NULL;

                execvp(args[0], args);
                perror("exec");
                exit(1);
            }
        }

        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < command_count; i++) wait(NULL);
        for (int i = 0; i < command_count; i++) free(commands[i]);
    }

    return 0;
}
