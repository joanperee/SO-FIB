#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

char buf[256];

void Usage(char *progname)
{
    sprintf(buf, "Usage: %s [no paramethers here]\n", progname);
    write(1, buf, strlen(buf));    
}

void error(char *msg)
{
    sprintf(buf, "Error: %s caused by %s\n", msg, strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 1) Usage(argv[0]);

    const char *filename = "salida.txt";
    const char *content = "ABCD";

    int fd = creat(filename, S_IRUSR | S_IWUSR);
    if (fd == -1) error("creat()");
    if (write(fd, content, 4) < 0) error("write()");
    sprintf(buf, "File %s has been created with content %s in the file descriptor %d\n", filename, content, fd);
    write(1, buf, strlen(buf));
    close(fd);
    return 0;
}