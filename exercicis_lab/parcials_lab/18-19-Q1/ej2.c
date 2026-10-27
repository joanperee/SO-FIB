#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

char buf[256];
volatile int signaled = 0;
volatile int max_secs;
volatile int texec = 0;

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Causa de l'error: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <segundos>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void func(int s)
{
    if (s == SIGALRM)
    {
        texec++;
        if (texec >= max_secs)
        {
            sprintf(buf, "Temporitzador expirat en %d segons\n", max_secs);
            write(1, buf, strlen(buf));
            exit(max_secs);
        }
        alarm(1);
    }
    if (s == SIGUSR1)
    {
        signaled = 1;
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);
    
    max_secs = atoi(argv[1]);
    if (max_secs <= 0) usage(argv[0]);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    struct sigaction t;
    t.sa_flags = 0;
    t.sa_handler = func;
    sigfillset(&t.sa_mask);
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");

    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    alarm(1);
    while (!signaled);

    sprintf(buf, "Ha arribat un SIGUSR1 després de %d segons\n", texec);
    write(1, buf, strlen(buf));
    exit(texec);
}
