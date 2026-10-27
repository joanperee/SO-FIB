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
    sprintf(buf, "Usage: %s <fichero_origen> <fichero_destino> <ini> <fi>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 5) usage(argv[0]);

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) error_and_exit("open origen");

    sprintf(buf, "Abrimos el archivo %s y nos retorna %d\n", argv[1], fd);
    write(1, buf, strlen(buf));

    int fdw = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fdw < 0) error_and_exit("open destino");

    int ini = atoi(argv[3]);
    int fi = atoi(argv[4]);

    if (ini < 0 || fi < ini)
    {
        sprintf(buf, "Error: los valores ini (%d) y fi (%d) no son válidos.\n", ini, fi);
        write(2, buf, strlen(buf));
        close(fd);
        close(fdw);
        exit(EXIT_FAILURE);
    }

    off_t pos = lseek(fd, ini, SEEK_SET);
    if (pos == (off_t)-1) error_and_exit("lseek");

    int n = fi - ini + 1;
    char c;

    for (int i = 0; i < n; ++i)
    {
        ssize_t bytes_read = read(fd, &c, sizeof(char));
        if (bytes_read != 1)
        {
            if (bytes_read == 0)  // EOF prematuro
            {
                write(1, "Fin de archivo antes de lo esperado.\n", 37);
                break;
            }
            error_and_exit("read");
        }

        ssize_t bytes_written = write(fdw, &c, sizeof(char));
        if (bytes_written != 1) error_and_exit("write");
    }

    close(fd);
    close(fdw);
}
