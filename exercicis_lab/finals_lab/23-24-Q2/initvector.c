#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

char buf[256];

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Cause: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *prog)
{
    sprintf(buf, "Usage: %s <N> [filename], with N > 0 and even\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    /* ---- Validar arguments ---- */
    if (argc < 2 || argc > 3) usage(argv[0]);
    int n = atoi(argv[1]);
    if (n%2 != 0) usage(argv[0]);

    /* ---- Crear el vector d'enters ---- */
    int *vector = sbrk(n*sizeof(int));
    for (int i = 0; i < n; ++i) 
        vector[i] = i;

    /* ---- Gestionar la sortida ---- */
    int fd;
    if (argc == 3) fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0640);
    else fd = 1;
    write(fd, &n, sizeof(int));
    for (int i = 0; i < n; ++i)
        write(fd, &vector[i], sizeof(int));

    /* ---- Alliberar el heap ---- */
    sbrk(-n*sizeof(int));
}