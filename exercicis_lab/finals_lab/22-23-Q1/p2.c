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
    sprintf(buf, "Usage: %s <X> <Y>\n", progname);
    write(2, buf, strlen(buf));
    sprintf(buf, "Where X is a number and Y a char in ('c' 'i')\n");
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int sigalrm = 0;
void ras()
{
    sigalrm = 1;
}

int main(int argc, char **argv)
{
    if (argc < 3) usage(argv[0]);
    if ((mknod("MYPIPE", S_IFIFO | 0664, 0) < 0) && (errno != EEXIST)) error_and_exit("mknod");
    int N = (argc - 1)/2;
    int pids[N];

    struct sigaction t;
    t.sa_handler = ras;
    t.sa_flags = 0;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");

    for (int i = 0; i < N; ++i)
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

        pids[i] = fork();
        if (pids[i] < 0) error_and_exit("fork");
        if (pids[i] == 0)
        {
            int fdw = open("MYPIPE", O_WRONLY);
            if (fdw < 0) error_and_exit("open fdw");
            dup2(fdw, 10);
            close(fdw);
            execlp("./p1", "p1", argv[i + 1], argv[i + 2], (char *)NULL);
            error_and_exit("execlp");
        }
        alarm(2);
        while (!sigalrm);
        if (kill(pids[i], SIGUSR1) < 0) error_and_exit("kill");
        
        if (waitpid(pids[i], NULL, 0) < 0) error_and_exit("waitpid");
    }
    
}