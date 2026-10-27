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
    sprintf(buf, "Usage: %s <file_in> <file_out>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 3) usage(argv[0]); 

    int pid = fork();
    if (pid < 0) error_and_exit("fork");
    if (pid == 0)
    {
        close(0);
        int fdin = open(argv[1], O_RDONLY);
        if (fdin < 0) error_and_exit("open file_in");
        
        close(1);
        int fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (fdout < 0) error_and_exit("open file_out");

        execlp("./desc_ok", "desc_ok", (char *)NULL);
        error_and_exit("execlp");
    }
    if (waitpid(pid, NULL, 0) < 0) error_and_exit("waitpid");
    exit(EXIT_SUCCESS);
}