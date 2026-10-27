#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

char buf[256];
const int MAX_PID = 441;

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Causa: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <PID>, with PID being a valid process\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    // 1. Validació del paràmetre 
    assert(argc == 2);
    int pid = atoi(argv[1]);
    assert((pid > 0) && (pid <= MAX_PID));
    kill(pid, 0);
    assert(errno != ESRCH); 

    // 2. Es crea un fill que mostra la jerarquia de processos amb arrel PID
    int ret = fork();
    if (ret < 0) error_and_exit("fork");
    if (ret == 0)
    {
        execlp("pstree", "pstree", "-p", argv[1], (char *)NULL);
        error_and_exit("execlp");
    }
    if (waitpid(ret, NULL, 0) < 0) error_and_exit("waitpid");
}