#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

/*
    El programa rep una llista de PIDS.
    1. Es creen N-fills concurrents
    2. Cada fill muta a ""grep State /proc/PID/status"
    3. El pare espera els fills 
*/

char buf[256];

void error_and_exit(const char *msg)
{
    perror(msg);
    if (errno != 0) {
        sprintf(buf, "Causa de l'error: %s", strerror(errno));
        write(1, buf, strlen(buf));
    }
    exit(EXIT_FAILURE);
}

void usage(const char *prog_name)
{
    sprintf(buf, "Usage: %s <PID_1> [PID_2] ... [PID_N]\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 2) usage(argv[0]);

    int nproc = argc - 1;

    for (int i = 0; i < nproc; ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)
        {
            int state_pid = atoi(argv[i + 1]);

            sprintf(buf, "FILL <%d>: mutaré per obtenir informació del PID %d\n", getpid(), state_pid);
            write(1, buf, strlen(buf));
            
            sprintf(buf, "/proc/%d/status", state_pid);
            if (execlp("grep", "grep", "State", buf, (char *)NULL) < 0) error_and_exit("execlp");
        }
    }
    while (waitpid(-1, NULL, 0) > 0);
    sprintf(buf, "PARE: tots els fills han acabat\n");
    write(1, buf, strlen(buf));
}