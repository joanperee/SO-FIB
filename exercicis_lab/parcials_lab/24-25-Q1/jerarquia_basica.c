#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

char buf[256];

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
    sprintf(buf, "Usage: %s <N>, with 1 <= N <= 10\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    // 1. Validació d'arguments
    if (argc != 2) usage(argv[0]);
    int n = atoi(argv[1]);
    if (n < 1 || n > 10) usage(argv[0]);

    // 2. Crear n-procs concurrents
    for (int i = 0; i < n; ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)
        {
            sprintf(buf, "Soc el fill %d i m'han creat en %d posició\n", getpid(), i);
            write(1, buf, strlen(buf));
            exit(i);
        }
    }

    // 3. El pare espera als fills bloquejat
    int st;
    int mort;
    while ((mort = waitpid(-1, &st, 0)) > 0)
    {
        if (WIFEXITED(st))
        {
            sprintf(buf, "El fill %d per exit amb paràmetre %d\n", mort, WEXITSTATUS(st));
            write(1, buf, strlen(buf));
        }
    }
}