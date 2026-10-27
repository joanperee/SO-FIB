#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

char buf[256];

void error_and_exit(char *msg) {
    perror(msg);
    sprintf(buf, "Valor de l'errno: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname) {
    sprintf(buf, "Usage: %s <n>, with 0 < n < 5\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main (int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);
    int n = atoi(argv[1]);
    if (n <= 0 || n >= 5) usage(argv[0]);

    for (int i = 0; i < n; ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)
        {
            execlp("./clab", "./clab", (char *)NULL);
            error_and_exit("execlp");
        }
    }
    int mort;
    while ((mort = waitpid(-1, NULL, 0)) > 0)
    {
        sprintf(buf, "Hijo con PID %d ha finalizado\n", mort);
        write(1, buf, strlen(buf));
    }
    exit(EXIT_SUCCESS);
}