#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

/*
    Excelp 2 (sleepS espera bloquejant)
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

int main (int argc, char **argv)
{
    if (argc != 2 || atoi(argv[1]) <= 0) usage(argv[0]);
    int N = atoi(argv[1]);
    
    sprintf(buf, "PARE: tenc el pid %d, hola!\n", getpid());
    write(1, buf, strlen(buf));

    for (int i = 1; i <= N; ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)
        {
            sprintf(buf, "FILL: soc el fill nº%d amb pid %d\n", i, getpid());
            write(1, buf, strlen(buf));

            sprintf(buf, "%d", i);
            if (execlp("./sleepS", "sleepS", buf, (char *)NULL) < 0) error_and_exit("execlp");
        }
    }

    int mort;
    int st;
    while ((mort = waitpid(-1, &st, 0)) > 0)
    {
        sprintf(buf, "PARE: el fill amb pid %d ha acabat amb codi %d\n", mort, WEXITSTATUS(st));
        write(1, buf, strlen(buf));
    }
}