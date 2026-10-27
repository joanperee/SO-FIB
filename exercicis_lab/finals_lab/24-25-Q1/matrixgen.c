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
    sprintf(buf, "Usage: %s <rows> <cols> [file]\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4) usage(argv[0]);

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    if (n <= 0 || m <= 0) usage(argv[0]);

    int size = n*m;
    int *matrix = malloc(size*sizeof(int));
    if (matrix == NULL) error_and_exit("malloc");

    for (int i = 0; i < size; ++i) matrix[i] = i;

    if (argc == 4)
    {
        int fd = open(argv[3], O_WRONLY | O_TRUNC | O_CREAT, 0644);
        if (fd < 0) error_and_exit("open");
        write(fd, matrix, size*sizeof(int));
        close(fd);
    }
    else write(1, matrix, size*sizeof(int));
    free(matrix);
}