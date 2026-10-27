#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

char buf[256];
int pids[10];

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Causa de l'error: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <Nprocs>, with 0 < Nprocs <= 10\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void ras(int s)
{
    if (s == SIGUSR1) {
        exit(alarm(0)); // retorna segons restants
    } 
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);
    int nprocs = atoi(argv[1]);
    if (nprocs <= 0 || nprocs > 10) usage(argv[0]);

    // Preparem la gestió de senyals
    sigset_t mask, oldmask, suspend_mask;
    struct sigaction t;

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) < 0) error_and_exit("sigprocmask");

    t.sa_handler = ras;
    t.sa_flags = SA_RESTART;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction ALRM");
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction USR1");

    // Creem els fills
    for (int i = 0; i < nprocs; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0) error_and_exit("fork");

        if (pids[i] == 0) // Codi fill
        {
            alarm(3);

            // Desbloquegem senyals i esperem
            sigset_t suspendmask;
            sigfillset(&suspendmask);
            sigdelset(&suspendmask, SIGALRM);
            sigdelset(&suspendmask, SIGUSR1);
            sigsuspend(&suspend_mask); 

            // Si ha expirat l'alarm
            sprintf(buf, "%d: Timeout\n", getpid());
            write(1, buf, strlen(buf));
            exit(EXIT_SUCCESS);
        }
    }

    // El pare envia SIGUSR1 als fills amb PID parell
    for (int i = 0; i < nprocs; ++i)
    {
        if (pids[i] % 2 == 0)
        {
            if (kill(pids[i], SIGUSR1) < 0) error_and_exit("kill");
        }
    }

    // Esperam tots els fills
    int st;
    int mort;
    while ((mort = waitpid(-1, &st, 0)) > 0)
    {
        if (WIFEXITED(st))
        {
            int ret = WEXITSTATUS(st);
            if (mort % 2 == 0)
            {
                sprintf(buf, "Fill %d. Segons restants: %d\n", mort, ret);
                write(1, buf, strlen(buf));
            }
        }
    }
}
