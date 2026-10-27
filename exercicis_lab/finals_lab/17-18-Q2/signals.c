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
int seconds = 1;
int count_sigusr1 = 0;

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Errno value: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    sprintf(buf, "Usage: %s <N>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void ras(int s)
{
    if (s == SIGALRM)
    {
        sprintf(buf, "SIGALRM recieved. Second: %d...\n", seconds);
        write(1, buf, strlen(buf));
    }
    else if (s == SIGUSR1)
    {
        ++count_sigusr1;
        sprintf(buf, "SIGUSR1 recieved. Counter: %d\n", count_sigusr1);
        write(1, buf, strlen(buf));
        if (count_sigusr1 == 3) kill(getpid(), SIGKILL);  
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) usage(argv[0]);
    int N = atoi(argv[1]);
    if (N < 0) usage(argv[0]);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGALRM);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    struct sigaction t;
    t.sa_handler = ras;
    sigfillset(&t.sa_mask);
    t.sa_flags = 0;
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    // no fa falta reprogramar sigusr2 perquè mata el procés per defecte
    // quan arribi, la terminal indicarà "User defined signal 2"

    sigset_t masksuspend;
    sigfillset(&masksuspend);
    sigdelset(&masksuspend, SIGALRM);
    
    for (int i = 0; i < N; ++i)
    {    
        alarm(1);
        sigsuspend(&masksuspend);
        ++seconds;
    }
    printf("Final del programa\n");
}