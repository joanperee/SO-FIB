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
    2. Crear N-processos seqüencials
    3. Cada vegada que un fill acaba el pare ho indica amb el seu pid 
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
    sprintf(buf, "Usage: %s N, with N > 0\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);

    sprintf(buf, "PARE: %d\n", getpid());
    write(1, buf, strlen(buf));
    
    int N = atoi(argv[1]);
    if (N < 0) usage(argv[0]);

    for (int i = 0; i < N; ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)  
        {
            sprintf(buf, "FILL nº %d amb PID %d\n", i, getpid());
            write(1, buf, strlen(buf));
            exit(EXIT_SUCCESS);
        }
        if (pid > 0)
        {
            if (waitpid(pid, NULL, 0) < 0) error_and_exit("wait");
            sprintf(buf, "PARE: el fill %d ha acabat\n", pid);
            write(1, buf, strlen(buf));
        }
    }
}