#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

char buf[256];

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Causa: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <file> <file_out>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc,char **argv)
{
    if (argc != 3) usage(argv[0]);

    // El pare obri l'arxiu file només de lectura
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) error_and_exit("open");

    // Cream una pipe sense nom: file -> Pare -> pipe -> Fill -> file_out
    int p[2];
    if (pipe(p) < 0) error_and_exit("pipe");

    // Cream el fill que llegirà el contingut de la pipe 
    int pid = fork();
    if (pid < 0) error_and_exit("fork");
    else if (pid == 0)
    {
        int fdw = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fdw < 0) error_and_exit("open");
        close(p[1]);
        char c;
        while (read(p[0], &c, sizeof(char)) > 0)
        {
            write(fdw, &c, sizeof(char));
        }
        close(fdw);
        exit(EXIT_SUCCESS);
    }

    char c;
    while (read(fd, &c, sizeof(char)) > 0)
    {
        write(p[1], &c, sizeof(char));
    }
    close(p[1]);
    if (waitpid(pid, NULL, 0) < 0) error_and_exit("waitpid");
    close(fd);
    close(p[1]);
}