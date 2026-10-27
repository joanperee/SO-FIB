#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

/*
    Els N-fills s'excuten concurrentment, i el pare els espera en ordre 
    1. Si el fill és parell fa exit(i*5)
    2. Si el fill és imparell fa exit(i) després d'un sleep d'<i> segons
    3. El pare indica el codi d'exit i EL NUMERO DE <i> DE CADA FILL --> vector pids
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
    sprintf(buf, "Usage: %s N\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);
    int N = atoi(argv[1]);
    sprintf(buf, "Pare: %d\n", getpid());
    write(1, buf, strlen(buf));
    int pids[N];
    for (int i = 0; i < N; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0) error_and_exit("fork");
        else if (pids[i] == 0)
        {
            sprintf(buf, "FILL %d amb pid %d\n", i, getpid());
            write(1, buf, strlen(buf));
            if (i%2 == 0) 
                exit(i*5);
            sleep(i);
            exit(24);
        }
    }
    int st;
    int mort;
    for (int i = 0; i < N; ++i)
    {
        mort = waitpid(pids[i], &st, 0);
        if (WIFEXITED(st))
        {
            sprintf(buf, "PARE: el fill %d amb pid %d ha mort per exit amb codi %d\n", i, mort, WEXITSTATUS(st));
            write(1, buf, strlen(buf));
        }
        if (WIFSIGNALED(st))
        {
            sprintf(buf, "PARE: el fill %d amb pid %d ha mort per signal amb codi %d\n", i, mort, WTERMSIG(st));
            write(1, buf, strlen(buf));
        }
    }
    

    sprintf(buf, "PARE: S'han imprès els fills en ordre de la <i>. Adeu!\n");
    write(1, buf, strlen(buf));
}
