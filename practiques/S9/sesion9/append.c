#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

void error_and_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    fprintf(stderr, "Usage: %s <file>\n", progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) error_and_exit("open");
        
    // Obtenim la mida del fitxer
    int size = lseek(fd, 0, SEEK_END);
    if (size == 0) error_and_exit("fitxer buit");

    // Reservem memòria per al buffer
    char *buf = malloc(size);
    if (buf == NULL) error_and_exit("malloc");

    // Tornem a l'inici i llegim el contingut
    lseek(fd, 0, SEEK_SET);
    int r = read(fd, buf, size);
    if (r != size || r < 0) error_and_exit("read");
        
    // Ens posicionem al final i escrivim el contingut llegit
    lseek(fd, 0, SEEK_END);
    write(fd, buf, size);

    // Escrivim per pantalla el nou contingut per comprovar
    char c;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &c, sizeof(char)) > 0)
    {
        write(1, &c, sizeof(char));
    }
    printf("\n");

    free(buf);
    close(fd);
}