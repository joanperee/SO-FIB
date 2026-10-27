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

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Errno value: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <N> [type]\n", progname);
    write(2, buf, strlen(buf));
    sprintf(buf, "Where N is mandatory and positive\n");
    write(2, buf, strlen(buf));
    sprintf(buf, "Where type is optional:\n");
    write(2, buf, strlen(buf));
    sprintf(buf, "      - If included, must be equal to 'c' or 'i'\n");
    write(2, buf, strlen(buf));
    sprintf(buf, "      - If not included, progname must be ./p1-char.exe or ./p1-integer.exe\n");
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void func()
{

}

int main(int argc, char **argv)
{
    /* ----- VALIDAR ARGUMENTS ----- */
    if (argc < 2 || argc > 3) usage(argv[0]);
    int n = atoi(argv[1]);
    if (n <= 0) usage(argv[0]);
    char type = ' ';
    if (argc == 3) 
    {
        type = argv[2][0];
        if (type != 'c' && type != 'i') usage(argv[0]);
    }
    else
    {
        if (strcmp(argv[0], "./p1-char.exe") == 0) type = 'c';
        else if (strcmp(argv[0], "./p1-integer.exe") == 0) type = 'i';
        else usage(argv[0]);
    }

    /* ----- ESPERA PASSIVA SIGUSR1 ----- */
    struct sigaction t;
    t.sa_handler = func;
    t.sa_flags = 0;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigsuspend(&mask);
    sprintf(buf, "El procés %d comença.\n", getpid());
    write(1, buf, strlen(buf));

    /* ----- ESCRIURE PEL CANAL 10 ELS n-ELEMENTS */
    for (int i = 0; i < n; ++i)
    {
        if (type == 'c')
        {
            sprintf(buf, "%c", i);
            write(10, buf, strlen(buf));
        }
        else if (type == 'i')
        {
            sprintf(buf, "%d", i);
            write(10, buf, strlen(buf));
        }
    }
    sprintf(buf, "El procés %d ha finalitzat.\n", getpid());
    write(1, buf, strlen(buf));
    exit(n);
}