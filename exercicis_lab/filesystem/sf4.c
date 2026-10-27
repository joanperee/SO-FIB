#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

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
    sprintf(buf, "Usage: %s <fitxer1> <offset>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 3) usage(argv[0]);

    int fd = open(argv[1], O_RDONLY);
    if (fd < 1) error_and_exit(argv[1]);

    int size = lseek(fd, 0, SEEK_END);
    if (size < 0 || lseek(fd, 0, SEEK_SET) < 0) error_and_exit("lseek");


    char c;
    int offset = atoi(argv[2]);
    while (read(fd, &c, sizeof(char)) > 0)
    {
        write(1, &c, sizeof(char));
        lseek(fd, offset - 1, SEEK_CUR);
    }
    printf("\n");
}