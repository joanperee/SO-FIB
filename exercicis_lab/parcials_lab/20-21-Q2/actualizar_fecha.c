#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

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
    sprintf(buf, "Usage: %s [fitxer1] ... [fitxerN]\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    // 1. Validació d'arguments. (si no hi ha cap fitxer no es farà res)
    if (argc > 5) usage(argv[0]);

    int nprocs = argc - 1;

    // 2. Es creen tants nprocs processos fills
    int pids[nprocs];
    for (int i = 0; i < nprocs; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0) error_and_exit("fork");
        if (pids[i] == 0)
        {
            execlp("touch", "touch", argv[i + 1], (char *)NULL);
            error_and_exit("execlp");
        }
    }

    // 3. El procés pare espera als fills i escriu informació
    int mort;
    while ((mort = waitpid(-1, NULL, 0)) > 0)
    {
        int i = 0;
        while (mort != pids[i]) ++i;
        sprintf(buf, "Data %s actualitzada per %d\n", argv[i + 1], pids[i]);
        write(1, buf, strlen(buf));
    }
}