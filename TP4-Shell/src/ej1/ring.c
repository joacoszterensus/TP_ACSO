#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Uso: anillo <n> <c> <s>\n");
        exit(EXIT_FAILURE);
    }

    int n_procesos = atoi(argv[1]);
    int valor_inicial = atoi(argv[2]);
    int iniciador = atoi(argv[3]);
    if (n_procesos <= 0 || iniciador <= 0 || iniciador > n_procesos) {
        fprintf(stderr, "Argumentos inválidos.\n");
        exit(EXIT_FAILURE);
    }
    iniciador--;

    int pipes[n_procesos][2];
    for (int i = 0; i < n_procesos; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n_procesos; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            int read_fd = pipes[(i - 1 + n_procesos) % n_procesos][0];
            int write_fd = pipes[i][1];

            for (int j = 0; j < n_procesos; j++) {
                if (j != i)       close(pipes[j][1]);
                if (j != (i - 1 + n_procesos) % n_procesos) close(pipes[j][0]);
            }

            int msg;
            if (i == iniciador) {
                msg = valor_inicial;
                if (write(write_fd, &msg, sizeof(msg)) != sizeof(msg)) {
                    perror("write init");
                    exit(EXIT_FAILURE);
                }
            }

            if (read(read_fd, &msg, sizeof(msg)) != sizeof(msg)) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            msg++;

            if (i == iniciador) {
                printf("Valor final recibido: %d\n", msg);
                exit(EXIT_SUCCESS);
            } else {
                if (write(write_fd, &msg, sizeof(msg)) != sizeof(msg)) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
        }
    }

    // Padre cierra todas las tuberías y espera a los hijos
    for (int i = 0; i < n_procesos; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i < n_procesos; i++) {
        wait(NULL);
    }
    return 0;
}



