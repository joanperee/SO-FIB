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
    sprintf(buf, "Usage: %s [no arguments needed]\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

volatile int texec = 0;
volatile int signaled = 0;

void func (int s)
{
    if (s == SIGALRM)
    {
        texec++;
        alarm(1);
    }
    if (s == SIGUSR1)
    {
        signaled = 1;
    }
}

int main(int argc, char **argv)
{
    if (argc > 1) usage(argv[0]);

    // 1. Bloquejam SIGALRM i SIGUSR1 abans de reprogramar-los
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    // 2. Reprogramam els signals corresponents
    struct sigaction t;
    t.sa_flags = 0;
    t.sa_handler = func;
    sigfillset(&t.sa_mask);
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");

    // 3. Desbloquejar els signals
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    // 4. Programam la primera alarma d'1 segon i fem l'espera activa
    alarm(1);
    while (!signaled);

    // 5. A partir d'aquí ha arribat el SIGUSR1
    sprintf(buf, "Ha arribat un SIGUSR1. Han passat %d segons\n", texec);
    write(1, buf, strlen(buf));
    exit(texec);
}