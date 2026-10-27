#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

char buf[256];

void error_and_exit(char *msg) {
    perror(msg);
    sprintf(buf, "Valor de l'errno: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname) {
    sprintf(buf, "Usage: %s <secs>, with 0 <= secs <= 255\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main (int argc, char **argv)
{
    if (argc > 2) usage(argv[0]);
    int secs = ((argc == 2) ? atoi(argv[1]) : 1);
    if (secs < 0 || secs > 255) usage(argv[0]);

    alarm(secs);
}