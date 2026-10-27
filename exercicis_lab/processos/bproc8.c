#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

/*
    El mateix que bproc7 però ara afegim condicions per cada fill
    de forma que uns moren per exit(24), altres exit(i*5) i altres per SIGUSR1/SIGALRM
*/

char buf[256];

void error_and_exit(const char *msg)
{
    perror(msg);
    if (errno != 0)
    {
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
    if (argc != 2)
        usage(argv[0]);
    int N = atoi(argv[1]);
    sprintf(buf, "Pare: %d\n", getpid());
    write(1, buf, strlen(buf));
    int pids[N];
    for (int i = 0; i < N; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0)
            error_and_exit("fork");
        else if (pids[i] == 0)
        {
            if (i % 4 == 0)
            {
                exit(i * 5);
            }
            else if (i % 4 == 1)
            {
                sleep(i);
                exit(24);
            }
            else if (i % 4 == 2)
            {
                alarm(10);
                sleep(12); // Amb l'sleep mantenim bloq el fill per a que no acabi abans de l'alarma
            }
            else if (i % 4 == 3)
            {
                sleep(15);
                kill(getpid(), SIGUSR1);
            }
        }
    }
    int st;
    int mort;
    while ((mort = waitpid(-1, &st, 0)) > 0)
    {
        int i = 0;
        while (pids[i] != mort) ++i;
        if (WIFEXITED(st))
        {
            sprintf(buf, "PARE: el fill %d amb pid %d ha mort per exit amb codi %d\n", i, mort, WEXITSTATUS(st));
            write(1, buf, strlen(buf));
        }
        if (WIFSIGNALED(st))
        {
            sprintf(buf, "PARE: el fill %d amb pid %d ha mort per signal amb codi %d\n", i, mort, WTERMSIG(st));
            write(1, buf, strlen(buf));
        }
    }
    sprintf(buf, "PARE: Adeu!\n");
    write(1, buf, strlen(buf));
}
