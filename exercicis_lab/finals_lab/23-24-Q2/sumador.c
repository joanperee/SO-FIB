#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

char buf[256];

void error_and_exit(char *msg)
{
    perror(msg);
    sprintf(buf, "Cause: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *prog)
{
    sprintf(buf, "Usage: %s <N>, with N > 0 and even\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    /* ---- Validar arguments */
    if (argc != 2) usage(argv[0]);

    /* ---- Etapa 1 ---- */
    int p[2];
    if (pipe(p) < 0) error_and_exit("unnamed pipe");
    int pid = fork();
    if (pid < 0) error_and_exit("fork");
    if (pid == 0)
    {
        dup2(p[1], 1);
        close(p[1]); close(p[0]);
        execlp("./initvector", "initvector", argv[1], (char *)NULL);
        error_and_exit("execlp");
    }
    close(p[1]);

    /* ---- Etapa 2 ---- */
    if (mknod("MIPIPE", S_FIFO | 0664, 0) < 0 && errno != EEXIST)
        error_and_exit("named pipe"); 
    int n;
    if (read(p[0], &n, sizeof(int)) < 0) error_and_exit("read");
    int fd = open("MIPIPE", O_RDWR);
    if (fd < 0) error_and_exit("named pipe");
    for (int i = 0; i < n/2; ++i)
    {
        int x, y;
        if (read(p[0], &x, sizeof(int)) < 0 || read(p[0], &y, sizeof(int)) < 0)
            error_and_exit("read");
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)
        {
            dup2(fd, 1); 
            close(fd);  
            sprintf(buf, "%d + %d", x, y);
            execlp("expr", "expr", buf, (char *)NULL);
            error_and_exit("execlp"); 
        }
        int st;
        if (waitpid(pid, &st, 0) < 0) error_and_exit("waitpid");
        if (WIFEXITED(st))
        {
            sprintf(buf, "El procés %d ha acabat voluntàriament.\n", pid);
            write(2, buf, strlen(buf)); 
        }
        else if (WIFSIGNALED(st))
        {
            sprintf(buf, "El procés %d ha acabat involuntàriament.\n", pid);
            write(2, buf, strlen(buf)); 
        }
        alarm(1);
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGALRM);
        sigsuspend(&mask);
    }     
    close(fd); close(p[0]);   
}