#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

/*
    El programa rep un argument natural S > 0
    Es fa un sleep de S segons i s'indica la finalització
    Es fa exit(S)
*/

char buf[256];

void error_and_exit(const char *msg)
{
    perror(msg);
    if (errno != 0) {
        sprintf(buf, "Causa de l'error: %s", strerror(errno));
        write(2, buf, strlen(buf));
    }
    exit(EXIT_FAILURE);
}

void usage(const char *prog_name)
{
    sprintf(buf, "Usage: %s S, with S > 0\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);
    int S = atoi(argv[1]);
    if (S <= 0) usage(argv[0]);
    
    sprintf(buf, "El procés amb PID %d esperra %d segons...\n", getpid(), S);
    write(1, buf, strlen(buf));
    
    sleep(atoi(argv[1]));
    
    sprintf(buf, "Ja està! Han passat %d segons\n", S);
    write(1, buf, strlen(buf));

    exit(S);
}