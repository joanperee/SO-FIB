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
    sprintf(buf, "Usage: %s <paraula> <fitxer1> [<fitxer2> ... <fitxer9>]\n", prog_name);
    write(1, buf, strlen(buf));
    write(1, buf, strlen(buf));
    sprintf(buf, "Descripció: el programa cerca la paraula en el/s fitxer/s mutant a grep\n");
    write(1, buf, strlen(buf));
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 11) {
        Usage(argv[0]);
    }
    //acabar implementació
    
}