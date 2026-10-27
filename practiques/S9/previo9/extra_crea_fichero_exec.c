#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

char buf[256];

void Usage(char *progname)
{
    sprintf(buf, "Usage: %s [no paramethers here]\n", progname);
    write(1, buf, strlen(buf));    
}

void error(char *msg)
{
    sprintf(buf, "Error: %s caused by %s\n", msg, strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

// Función para ejecutar un comando con execlp() y esperar a que termine
void execute_command(const char *command) {
    int pid = fork();
    if (pid < 0) error("fork()");
    
    if (pid == 0) {
        // Proceso hijo ejecuta el comando
        if (execlp("/usr/bin/bash", "bash", "-c", command, (char *)NULL) < 0) error("execlp()");
    } else {
        // Proceso padre espera a que termine el hijo
        int status;
        if (waitpid(pid, &status, 0) < 0) error("waitpid()");
        if (!WIFEXITED(status)) error("Child process did not exit normally");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 1) Usage(argv[0]);

    const char *filename = "salida.txt";
    const char *content = "previ Lab SO Joan Pere Ollers proves execlp";

    // Ejecutamos el primer comando (echo) para crear el archivo y escribir el contenido
    sprintf(buf, "echo %s > %s", content, filename);
    execute_command(buf);

    // Ejecutamos el segundo comando (chmod) para cambiar los permisos del archivo
    sprintf(buf, "chmod 600 %s", filename);
    execute_command(buf);

    // Finalmente, mostramos el mensaje de confirmación
    sprintf(buf, "File %s has been created with content %s\n", filename, content);
    write(1, buf, strlen(buf));

    return 0;
}
