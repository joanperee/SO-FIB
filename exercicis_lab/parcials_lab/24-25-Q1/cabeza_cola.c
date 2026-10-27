#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

// Vars globals (poden ser declarades volatile)
char buf[256];
int sigusr1 = 0;
int sigusr2 = 0;

void error_and_exit(const char *msg)
{
    perror(msg);
    sprintf(buf, "Causa de l'error: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(const char *progname)
{
    sprintf(buf, "%s: paràmetres invàlids\n", progname);
    write(2, buf, strlen(buf));
    sprintf(buf, "Usage: %s <num_línies> <segons> <fitxer1> ... [fitxerN]\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void ras(int s)
{
    if (s == SIGUSR1) sigusr1 = 1;
    else if (s == SIGUSR2) sigusr2 = 1;
}

int main(int argc, char **argv)
{
    // 1. Validació d'arguments 
    if (argc < 4) usage(argv[0]);
    int nfitxers = argc - 3;
    if (nfitxers%2 == 1 || nfitxers > 10) usage(argv[0]);

    // 2. Reprogramar SIGUSR1/2 i SIGALRM
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");
    struct sigaction t;
    t.sa_handler = ras;
    t.sa_flags = 0;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGUSR2, &t, NULL) < 0) error_and_exit("sigaction");

    // 3. Crear n-procs concurrents però el pare espera a que arribi algun signal
    for (int i = 0; i < nfitxers; ++i)
    {
        // Primer esperam l'alarma (temps mínim)
        alarm(atoi(argv[2]));
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
        sigdelset(&mask, SIGALRM);
        sigsuspend(&mask);

        // Després esperam a algun SIGUSR1 o SIGUSR2
        sigsuspend(&mask);

        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)
        {
            if (sigusr2 && !sigusr1)    // SIGUSR2 rebut
            {
                execlp("tail", "tail", "-n", argv[1], argv[i + 3], (char *)NULL);
                error_and_exit("execlp");
            }
            else if (sigusr1 && !sigusr2)   // SIGUSR1 rebut
            {
                int num_linies = 2*atoi(argv[1]);
                sprintf(buf, "%d", num_linies);
                execlp("head", "head", "-n", buf, argv[i + 3], (char *)NULL);
                error_and_exit("execlp");
            }
        }
    }

    // 4. El pare espera als fills bloquejat
    int st;
    int mort;
    while ((mort = waitpid(-1, &st, 0)) > 0)
    {
        if (WIFEXITED(st))
        {
            sprintf(buf, "El fill %d per exit amb paràmetre %d\n", mort, WEXITSTATUS(st));
            write(1, buf, strlen(buf));
        }
        if (WIFSIGNALED(st))
        {
            sprintf(buf, "El fill %d per signal amb paràmetre %d\n", mort, WTERMSIG(st));
            write(1, buf, strlen(buf));
        }
    }
}