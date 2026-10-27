#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

char buf[256];
int seconds = 0;

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Errno value: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <N_seconds> <cmd>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void ras(int s)
{
    if (s == SIGALRM)
    {
        seconds++;
        alarm(1);
        sprintf(buf, "SIGALRM recieved.\n");
        write(1, buf, strlen(buf));
    }
    if (s == SIGUSR1)
    {
        sprintf(buf, "SIGUSR1 recieved. Value of seconds: %d\n", seconds);
        write(1, buf, strlen(buf));
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) usage(argv[0]);

    // 1. Bloquejam tots els signals que no son SIGALRM, SIGUSR1 i SIGCHLD
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGALRM);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    // 2. Reprogramem sigalrm, el sigusr1 i el sigchld 
    struct sigaction t;
    t.sa_handler = ras;
    t.sa_flags = 0;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGCHLD, &t, NULL) < 0) error_and_exit("sigaction");

    // 3. Creem el fill que mutarà a cmd (argv[2])
    int pid = fork();
    if (pid < 0) error_and_exit("fork");
    if (pid == 0)
    {
        execlp(argv[2], argv[2], (char *)NULL);
        error_and_exit("execlp");
    }

    // 4. El pare programa l'alarma 
    for (int i = 0; i < N; and not stop )
    alarm(1);
    sigsuspend(&mask);
}