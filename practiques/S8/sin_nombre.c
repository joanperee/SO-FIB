#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];  // pipefd[0] será el extremo de lectura, pipefd[1] el extremo de escritura
    pid_t pid;
    char mensaje[] = "Inicio\n";

    // Crear la pipe
    if (pipe(pipefd) == -1) {
        perror("Error al crear la pipe");
        exit(EXIT_FAILURE);
    }

    // Crear el proceso hijo
    pid = fork();
    if (pid == -1) {
        perror("Error al hacer fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Proceso hijo
        close(pipefd[1]);  // Cerrar el extremo de escritura de la pipe

        // Redirigir la entrada estándar (stdin) al extremo de lectura de la pipe
        if (dup(pipefd[0]) == -1) {
            perror("Error en dup2");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]);  // Ya no es necesario tener abierto pipefd[0] después de dup2

        // Ejecutar el comando `cat`
        execlp("cat", "cat", (char *)NULL);
        // Si execlp falla
        perror("Error al ejecutar execlp");
        exit(EXIT_FAILURE);

    } else {
        // Proceso padre
        close(pipefd[0]);  // Cerrar el extremo de lectura de la pipe

        // Escribir el mensaje en el extremo de escritura de la pipe
        write(pipefd[1], mensaje, sizeof(mensaje) - 1);

        // Cerrar el extremo de escritura de la pipe
        close(pipefd[1]);

        // Esperar a que el proceso hijo termine
        wait(NULL);

        // Mostrar el mensaje final
        printf("Fin\n");
    }

    return 0;
}
