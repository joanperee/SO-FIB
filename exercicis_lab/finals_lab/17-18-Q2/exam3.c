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
    
    // El pare torna a obrir els fitxers i consulta la mida
    int fdin = open(argv[1], O_RDONLY);
    double sizein = (double) lseek(fdin, 0, SEEK_END);
    if (sizein < 0) error_and_exit("lseek");

    int fdout = open(argv[2], O_RDONLY);
    double sizeout = (double) lseek(fdout, 0, SEEK_END);
    if (sizeout < 0) error_and_exit("lseek");
    
    // Retornem la relació
    double rel = sizeout/sizein;
    sprintf(buf, "La relació del fitxer descomprimit entre el fitxer comprimit és:\n");
    write(2, buf, strlen(buf));
    sprintf(buf, "%f/%f = %f\n", sizeout, sizein, rel);
    write(2, buf, strlen(buf));

    exit(EXIT_SUCCESS);
}