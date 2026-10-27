#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

char buf[256];

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Causa de l'error: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <nprocs> <secs>, with nprocs > 0 and secs > 0\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

volatile int signaled = 0;
volatile int fills_morts = 0;

void func(int s) 
{
    if (s == SIGALRM)
    {
        signaled = 1;
    }
    else if (s == SIGCHLD)
    {
        int st;
        int mort;
        while ((mort = waitpid(-1, &st, WNOHANG)) > 0)
        {
            ++fills_morts;
            if (WIFEXITED(st))
            {
                int exitCode = WEXITSTATUS(st);
                sprintf(buf, "El fill %d ha acabat amb codi %d\n", mort, exitCode);
                write(1, buf, strlen(buf));
            }
            else if (WIFSIGNALED(st))
            {
                int sigCode = WTERMSIG(st);
                sprintf(buf, "El fill %d ha acabat amb signal %d\n", mort, sigCode);
                write(1, buf, strlen(buf));
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc  != 3) usage(argv[0]);
    int maxsecs = atoi(argv[2]);
    int nprocs = atoi(argv[1]);
    if (maxsecs <= 0 || nprocs <= 0) usage(argv[0]);
    int pids[nprocs];

    // Reprogramam alarma
    struct sigaction t;
    t.sa_handler = func;
    t.sa_flags = 0;
    sigfillset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGCHLD, &t, NULL) < 0) error_and_exit("sigaction");

    // Els fills es creen de forma concurrent
    for (int i = 0; i < nprocs; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0) error_and_exit("fork");
        if (pids[i] == 0)
        {
            sprintf(buf, "%d", i + 1);
            execlp("./ej2", "ej2", buf, (char *)NULL);
            error_and_exit("execlp");
        }
    }

    // El pare es bloqueja durant maxsecs
    sigset_t waitmask;
    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGALRM);
    sigdelset(&waitmask, SIGCHLD);
    alarm(maxsecs);
    while (!signaled) sigsuspend(&waitmask);

    // Matam tots els fills que no hagin acabat
    for (int i = 0; i < nprocs; ++i)
    {
        int mort = waitpid(pids[i], NULL, WNOHANG);
        if (mort == 0)
        {
            if (kill(pids[i], SIGKILL) < 0) error_and_exit("kill");
        }
    }

    while (fills_morts < nprocs);
}