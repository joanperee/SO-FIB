#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

char buf[256];

void Error(const char *msg)
{
    perror(msg);
    if (errno != 0) {
        sprintf(buf, "Causa de l'error: %s", strerror(errno));
        write(2, buf, strlen(buf));
    }
    exit(1);
}

void Usage(const char *prog_name)
{
    sprintf(buf, "Usage: %s <num> <paraula> <nom_fitxer>\n", prog_name);
    write(1, buf, strlen(buf));
    sprintf(buf, "Exemple: ./busqueda_parcial 100 paraula busqueda_parcial.c\n");
    write(1, buf, strlen(buf));
    sprintf(buf, "Descripció: el programa muta a grep i cerca la paraula en el fitxer\n");
    write(1, buf, strlen(buf));
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        Usage(argv[0]);
    }

    int pid = fork();
    if (pid == 0) {
        execlp("grep", "grep", argv[2], argv[3], (char *)NULL);
        Error("Ha fallat l'execlp()");
    }
    else if (pid < 0) {
        Error("Ha fallat el fork()");
    }
    else {
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            Error("Ha fallat el waitpid()");
        }
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            sprintf(buf, "Finalitza grep paraula %s fitxer %s : exit(%d)\n", argv[2], argv[3], exit_code);
            write(1, buf, strlen(buf));
        }
        else {
            sprintf(buf, "El fill no ha finalitzat el procés correctament\n");
            write(1, buf, strlen(buf));
        }
        
    }
}