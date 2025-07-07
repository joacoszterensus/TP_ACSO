

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

 char *strdup(const char *s);


void separador(char *s) {
    while (*s == ' ' || *s == '\t') s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t')) *end-- = '\0';
}

void sin_comillas(char *s) {
    int len = strlen(s);
    if (len >= 2 && ((s[0] == '"' && s[len - 1] == '"') || (s[0] == '\'' && s[len - 1] == '\''))) {
        s[len - 1] = '\0';
        memmove(s, s + 1, len);
    }
}

void free_commands(char **commands, int count) {
    for (int i = 0; i < count; i++) {
        if (commands[i]) {
            free(commands[i]);
            commands[i] = NULL;
        }
    }
}

int main() {
    char command[256];
    char *commands[MAX_COMMANDS] = {NULL};

    while (1) {
        printf("Shell> ");
        if (!fgets(command, sizeof(command), stdin)) break;
        command[strcspn(command, "\n")] = '\0';
        separador(command);

        if (strcmp(command, "exit") == 0) break;
        if (strlen(command) == 0) continue;

        int command_count = 0;
        char *token = strtok(command, "|");
        while (token != NULL && command_count < MAX_COMMANDS) {
            separador(token);
            commands[command_count] = strdup(token);
            if (!commands[command_count]) {
                perror("strdup");
                free_commands(commands, command_count);
                continue;
            }
            command_count++;
            token = strtok(NULL, "|");
        }

        if (command_count == 0) continue;

        int pipes[MAX_COMMANDS - 1][2];
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                free_commands(commands, command_count);
                continue;
            }
        }

        for (int i = 0; i < command_count; i++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                free_commands(commands, command_count);
                continue;
            }

            if (pid == 0) {
                if (i > 0) {
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                        perror("dup2 stdin");
                        exit(EXIT_FAILURE);
                    }
                }
                if (i < command_count - 1) {
                    if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                        perror("dup2 stdout");
                        exit(EXIT_FAILURE);
                    }
                }

                
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Parse command arguments
                char *args[MAX_ARGS];
                int argc = 0;
                char *cmd_copy = strdup(commands[i]);
                if (!cmd_copy) {
                    perror("strdup");
                    exit(EXIT_FAILURE);
                }

                char *arg = strtok(cmd_copy, " ");
                while (arg && argc < MAX_ARGS - 1) {
                    sin_comillas(arg);
                    args[argc++] = arg;
                    arg = strtok(NULL, " ");
                }
                args[argc] = NULL;

                execvp(args[0], args);
                perror("execvp");
                free(cmd_copy);
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < command_count; i++) {
            wait(NULL);
        }

        free_commands(commands, command_count);
        command_count = 0;
    }

    return 0;
}