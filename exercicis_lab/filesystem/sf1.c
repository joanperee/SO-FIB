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
    sprintf(buf, "Usage: %s <fitxer1>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc,char **argv)
{
    if (argc != 2) usage(argv[0]);

    // Obrim l'arxiu en mode lectura. Si no existeix fd = -1
    int fd = open(argv[1], O_RDONLY);
    sprintf(buf, "Obrim l'arxiu %s i ens retorna %d\n", argv[1], fd);
    write(1, buf, strlen(buf));

    // LLegim el contingut del fitxer i escrivim pel canal stdout (1)
    char c;
    while (read(fd, &c, sizeof(char)) > 0)
    {
        write(1, &c, sizeof(char));
    }
    sprintf(buf, "\n");
    write(1, buf, strlen(buf));
    close(fd);
}