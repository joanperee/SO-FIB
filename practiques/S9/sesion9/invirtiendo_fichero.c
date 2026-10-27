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

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0) error_and_exit("open input file");

    // Obtenim la mida del fitxer d'entrada
    off_t size = lseek(fd_in, 0, SEEK_END);
    if (size == -1) error_and_exit("lseek");
    if (size == 0) {
        fprintf(stderr, "El fitxer està buit\n");
        close(fd_in);
        exit(EXIT_FAILURE);
    }

    // Reservem memòria per al buffer
    char *buf = malloc(size);
    if (buf == NULL) error_and_exit("malloc");

    // Tornem a l'inici i llegim el contingut
    if (lseek(fd_in, 0, SEEK_SET) == -1) error_and_exit("lseek");
    ssize_t r = read(fd_in, buf, size);
    if (r < 0 || r != size) error_and_exit("read");
    close(fd_in);

    // Invertim el contingut al buffer
    for (off_t i = 0; i < size / 2; i++) {
        char tmp = buf[i];
        buf[i] = buf[size - 1 - i];
        buf[size - 1 - i] = tmp;
    }

    // Creem el nom del fitxer de sortida
    char *nom_sortida = malloc(strlen(argv[1]) + 5); // ".inv" + '\0'
    if (nom_sortida == NULL) {
        free(buf);
        error_and_exit("malloc");
    }
    sprintf(nom_sortida, "%s.inv", argv[1]);

    int fd_out = open(nom_sortida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        free(buf);
        free(nom_sortida);
        error_and_exit("open output file");
    }

    // Escrivim el buffer invertit al fitxer de sortida
    ssize_t w = write(fd_out, buf, size);
    if (w < 0 || w != size) {
        free(buf);
        free(nom_sortida);
        close(fd_out);
        error_and_exit("write");
    }

    // Netejem i tanquem fitxers
    free(buf);
    free(nom_sortida);
    close(fd_out);

    return 0;
}
