#include <errno.h>    // for errno
#include <sys/wait.h> // for waitpid
#include <unistd.h>   // for getpid, getppid, write, fork, execlp
#include <stdio.h>    // for sprintf, perror
#include <stdlib.h>   // for exit, strtol
#include <string.h>   // for strlen
#include <signal.h>   // for sigaction, sigprocmask, sigsuspend, sigXset, kill

int sig = 0;

void usage()
{
    char msg[256];
    sprintf(msg, "Usage: crearJP <NFILLS1> <NFILLS2>, 1 =< NFILLS1 =< 10, 1 =< NFILLS2 =< 5\n");
    write(2, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

void error_and_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void rasALRM(int s)
{
}

void rasUSR(int s)
{
    sig = s;
}

int main(int argc, char *argv[])
{
    int i, r, nfills1, nfills2, res;
    int pids1[10], pids2[5];
    char msg[256];
    char *ptr;
    struct sigaction sa;
    sigset_t m;

    if (argc != 3) usage();
    nfills1 = (int)strtol(argv[1], &ptr, 10);
    if ((*ptr != '\0') || (nfills1 < 1) || (nfills1 > 10)) usage();
    nfills2 = (int)strtol(argv[2], &ptr, 10);
    if ((*ptr != '\0') || (nfills2 < 1) || (nfills2 > 5)) usage();

    sprintf(msg, "Soc el procés %d arrel de la jerarquia\n", getpid());
    write(1, msg, strlen(msg));

    if (sigemptyset(&m) < 0)
        error_and_exit("sigemptyset");
    if (sigaddset(&m, SIGUSR1) < 0)
        error_and_exit("sigaddset USR1");
    if (sigaddset(&m, SIGUSR2) < 0)
        error_and_exit("sigaddset USR2");
    if (sigaddset(&m, SIGALRM) < 0)
        error_and_exit("sigaddset ALRM");
    if (sigprocmask(SIG_BLOCK, &m, NULL) < 0)
        error_and_exit("sigprocmask");

    sa.sa_flags = 0;
    sa.sa_handler = rasALRM;
    if (sigemptyset(&sa.sa_mask) < 0)
        error_and_exit("sigemptyset");
    if (sigaction(SIGALRM, &sa, NULL) < 0)
        error_and_exit("sigaction ALRM");

    for (i = 0; i < nfills1; i++)
    {
        switch (pids1[i] = fork())
        {
        case -1:
            error_and_exit("fork");
        case 0:
            sa.sa_flags = SA_RESETHAND;
            sa.sa_handler = rasUSR;
            if (sigemptyset(&sa.sa_mask) < 0)
                error_and_exit("sigemptyset");
            if (sigaction(SIGUSR1, &sa, NULL) < 0)
                error_and_exit("sigaction USR1");
            if (sigaction(SIGUSR2, &sa, NULL) < 0)
                error_and_exit("sigaction USR2");
            if (sigfillset(&m) < 0)
                error_and_exit("sigfillset");
            if (sigdelset(&m, SIGUSR1) < 0)
                error_and_exit("sigdelset USR1");
            if (sigdelset(&m, SIGUSR2) < 0)
                error_and_exit("sigdelset USR2");

            for (i = 0; i < nfills2; i++)
            {
                switch (pids2[i] = fork())
                {
                case -1:
                    error_and_exit("fork");
                case 0:
                    if (sigsuspend(&m) < 0 && errno != EINTR)
                        error_and_exit("sigsuspend");
                    sprintf(msg, "Soc el procés %d (fill del procés %d): Rebut signal SIGUSR%d (%d)\n", getpid(), getppid(), (sig == SIGUSR1) ? 1 : 2, sig);
                    write(1, msg, strlen(msg));
                    exit(EXIT_SUCCESS);
                }
            }
            if (sigsuspend(&m) < 0 && errno != EINTR)
                error_and_exit("sigsuspend");
            sprintf(msg, "Soc el procés %d (fill del procés %d): Rebut signal SIGUSR%d (%d)\n", getpid(), getppid(), (sig == SIGUSR1) ? 1 : 2, sig);
            write(1, msg, strlen(msg));

            for (i = 0; i < nfills2; i++)
            {
                if (kill(pids2[i], sig) < 0)
                    error_and_exit("kill USR");
            }
            while ((res = waitpid(-1, NULL, 0)) > 0)
                ;
            if (res < 0 && errno != ECHILD)
                error_and_exit("waitpid");
            exit(EXIT_SUCCESS);
        }
    }

    if (sigfillset(&m) < 0)
        error_and_exit("sigfillset");
    if (sigdelset(&m, SIGALRM) < 0)
        error_and_exit("sigdelset ALRM");
    alarm(60);
    if (sigsuspend(&m) < 0 && errno != EINTR)
        error_and_exit("sigsuspend");

    for (i = 0; i < nfills1; i++)
    {
        r = rand() % 2;
        if (r)
        {
            if (kill(pids1[i], SIGUSR2) < 0)
                error_and_exit("kill USR2");
        }
        else
        {
            if (kill(pids1[i], SIGUSR1) < 0)
                error_and_exit("kill USR1");
        }
    }

    while ((res = waitpid(-1, NULL, 0)) > 0)
        ;
    if (res < 0 && errno != ECHILD)
        error_and_exit("waitpid");
    exit(EXIT_SUCCESS);
}
