#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

char buf[256];

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Errno value: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <N>, with N > 0\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);
    int nprocs = atoi(argv[1]);
    if (nprocs <= 0) usage(argv[0]);

    int pids[nprocs];
    for (int i = 0; i < nprocs; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0) error_and_exit("fork");
        if (pids[i] == 0)
        {
            if (i == 0) sprintf(buf, "My father's PID is %d\n", getppid());
            else sprintf(buf, "My big brother's PID is %d\n", pids[i - 1]);
            write(1, buf, strlen(buf));
        }
    }
    
    sigset_t mask;
    sigfillset(&mask);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");
    while (waitpid(-1, NULL, 0) > 0);
}