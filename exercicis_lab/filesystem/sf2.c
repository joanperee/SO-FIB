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
    sprintf(buf, "Usage: %s <fitxer> <nom_copia_fitxer>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc,char **argv)
{
    if (argc != 3) usage(argv[0]);

    // Obrim l'arxiu de nom argv[1] en mode lectura, si no existeix peta
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) error_and_exit("open");
    sprintf(buf, "Obrim l'arxiu %s i ens retorna %d\n", argv[1], fd);
    write(1, buf, strlen(buf));

    // Cream i obrim el nou arxiu, que te nom argv[2]
    int fdw = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fd < 0) error_and_exit("open");

    // Copiam el contingut del fitxer argv[1] (llegint) al fitxer argv[2] (escrivint)
    char c;
    while (read(fd, &c, sizeof(char)) > 0)
    {
        write(fdw, &c, sizeof(char));
    }
    sprintf(buf, "S'ha copiat correctament el fitxer %s\n", argv[1]);
    write(1, buf, strlen(buf));
    close(fd);
    close(fdw);
}