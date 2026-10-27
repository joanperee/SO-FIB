#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

char buf[256];

void error_and_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: el programa no admet arguments\n");
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int secs = 0;

void signal_handler(int s)
{
    if (s == SIGALRM) {
        secs += 1;
        alarm(1);
    } else if (s == SIGUSR1) {
        secs = 0;
    } else if (s == SIGUSR2) {
        sprintf(buf, "Actualment han passat %d segons\n", secs);
        write(1, buf, strlen(buf));
    }
}

int main(int argc, char **argv)
{
    if (argc > 1) usage(argv[0]);

    // 1. Crear les variables necessàries per manejar signals
    struct sigaction t;
    sigset_t mask;

    // 2. Bloquejar els signals que hem de reprogramar 
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, NULL)) error_and_exit("sigprocmask");

    // 3. Reprogramem SIGALRM, SIGUSR1 i SIGUSR2
    t.sa_flags = 0;
    t.sa_handler = signal_handler;
    sigfillset(&t.sa_mask);

    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");       
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGUSR2, &t, NULL) < 0) error_and_exit("sigaction");

    // 4. Desbloquegem els senyals per poder-los rebre
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL)) error_and_exit("sigprocmask");

    // 5. Iniciem la primera alarma
    alarm(1);

    // 6. Esperem senyals indefinidament sense consumir CPU
    sigemptyset(&mask);
    while (1) {
        sigsuspend(&mask); 
    }
}
