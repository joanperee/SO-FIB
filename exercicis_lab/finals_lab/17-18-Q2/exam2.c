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
    // 1. Validam paràmetres
    if (argc != 3) usage(argv[0]); 

    // 2. Cream la pipe sense nom 
    int p[2];
    if (pipe(p) < 0) error_and_exit("pipe");    

    // 3. El pare P crea un fill P0
    int pid = fork();

    if (pid < 0) error_and_exit("fork");
    if (pid == 0)
    {
        // Redirigim fdin a stdin
        close(0);
        int fdin = open(argv[1], O_RDONLY);
        if (fdin < 0) error_and_exit("open file_in");
        
        // Redirigm l'extrem d'escriptura de la pipe a stdout
        dup2(p[1], 1);
        close(p[0]); close(p[1]);

        // El fill muta al descompressor
        execlp("./desc_ok", "desc_ok", (char *)NULL);
        error_and_exit("execlp");
    }
    
    int fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0640);
    if (fdout < 0) error_and_exit("open file_out");

    // Tancam l'extrem d'escriptura de la pipe del pare
    close(p[1]); 

    char c;
    while (read(p[0], &c, sizeof(char)) > 0)
    {
        write(fdout, &c, sizeof(char));
    }
    close(p[0]);

    // Espera a que el fill acabi de mutar
    if (waitpid(pid, NULL, 0) < 0) error_and_exit("waitpid");
    exit(EXIT_SUCCESS);
}