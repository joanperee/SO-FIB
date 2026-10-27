#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

/*
    El programa no rep arguments. 
    1. Imprimir el pid del pare
    2. Crear un fill, el fill imprimeix el pid del pare (getppid)
    3. El pare espera a que el fill acabi i imprimeix el pid del fill
*/

char buf[256];

void error_and_exit(const char *msg)
{
    perror(msg);
    if (errno != 0) {
        sprintf(buf, "Causa de l'error: %s", strerror(errno));
        write(2, buf, strlen(buf));
    }
    exit(EXIT_FAILURE);
}

void usage(const char *prog_name)
{
    sprintf(buf, "Usage: %s\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc > 1) usage(argv[0]);
    sprintf(buf, "PARE: %d\n", getpid());
    write(1, buf, strlen(buf));
    int pid = fork();
    if (pid < 0) error_and_exit("fork");
    if (pid == 0)  
    {
        sprintf(buf, "FILL: El meu pare es %d\n", getppid());
        write(1, buf, strlen(buf));
        exit(EXIT_SUCCESS);
    }
    waitpid(-1, NULL, 0);
    sprintf(buf, "PARE: el meu fill és %d\n", pid);
    write(1, buf, strlen(buf));
}