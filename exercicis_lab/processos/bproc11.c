#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

/*
    Execlp bàsic (echo)
    si volem saber la <i> substituïr int pid -> pids[N] -> pids[i]
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

int main (int argc, char **argv)
{
    if (argc != 2 || atoi(argv[1]) <= 0) usage(argv[0]);
    int N = atoi(argv[1]);
    
    sprintf(buf, "PARE: tenc el pid %d, hola!\n", getpid());
    write(1, buf, strlen(buf));
    int pids[N];
    for (int i = 0; i < N; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0) error_and_exit("fork");
        if (pids[i] == 0)
        {
            sprintf(buf, "FILL: soc el fill nº%d amb pid %d\n", i, getpid());
            write(1, buf, strlen(buf));

            sprintf(buf, "Hola! Soc el fill %d i he migrat al programa echo", i);
            if (execlp("echo", "echo", buf, (char *)NULL) < 0) error_and_exit("execlp");
        }
    }

    int mort;
    int st;
    int i = 0;
    while ((mort = waitpid(pids[i], &st, 0)) > 0)
    {
        if (WIFEXITED(st))
        {
            sprintf(buf, "PARE: el fill nº%d amb pid %d ha acabat amb codi %d\n", i, mort, WEXITSTATUS(st));
            write(1, buf, strlen(buf));
        }
        ++i;
    }
}