#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

char buf[256];

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Value of errno: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <file> <cols>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 3) usage(argv[0]);

    int p[2];
    if (pipe(p) < 0) error_and_exit("pipe");

    int pid1 = fork();
    if (pid1 < 0) error_and_exit("fork");
    if (pid1 == 0)
    {
        dup2(p[1], 1); close(p[1]); close(p[0]);
        execlp("cat", "cat", argv[1], (char *)NULL);
        error_and_exit("execlp");
    }

    int pid2 = fork();
    if (pid2 < 0) error_and_exit("fork");
    if (pid2 == 0)
    {
        dup2(p[0], 0);
        close(p[0]); close(p[1]);
        sprintf(buf, "-w%d", atoi(argv[2])*4);
        execlp("od", "od", "-t", "d4", buf, (char *)NULL);
        error_and_exit("execlp");
    }

    close(p[1]);
    while (waitpid(-1, NULL, 0) > 0);
    exit(EXIT_SUCCESS);
}