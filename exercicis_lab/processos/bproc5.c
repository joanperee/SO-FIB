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
    2. Crear N-processos concurrents (paral·lelament)
    3. Cada fill imprimeix la seva <i>, el seu <pid> i acaba
    4. Cada vegada que un fill acaba el pare imprimeix el seu PID
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
    sprintf(buf, "Usage: %s N\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);

    sprintf(buf, "Pare: %d\n", getpid());
    write(1, buf, strlen(buf));
    for (int i = 0; i < atoi(argv[1]); ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        else if (pid == 0)
        {
            if (i%2 == 0) sleep(i);
            sprintf(buf, "FILL %d amb pid %d\n", i, getpid());
            write(1, buf, strlen(buf));
            exit(0);
        }
    }
    int mort;
    while ((mort = waitpid(-1, NULL, 0)) > 0)
    {
        sprintf(buf, "PARE: el fill %d ha mort\n", mort);
        write(1, buf, strlen(buf));
    }
    sprintf(buf, "PARE: Adeu!\n");
    write(1, buf, strlen(buf));
}
