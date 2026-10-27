#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

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
    sprintf(buf, "Usage: %s intv count [PID], with  1 <= intv <= 5 and 1 <= count <= 10\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void func()
{
}

int main(int argc, char **argv)
{
    // 1. Validar paràmetres d'entrada i emmagatzemar-los
    if (argc != 3 && argc != 4) usage(argv[0]);
    int intv = atoi(argv[1]);
    if (intv < 1 || intv > 5) usage(argv[0]);
    int count = atoi(argv[2]);
    if (count < 1 || count > 10) usage(argv[0]);
    int pid_monitor = (argc == 4 ? atoi(argv[3]) : getpid());

    // 2. Bloquejar SIGALRM, reprogramar-lo i preparar l'espera passiva 
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");
    struct sigaction t;
    t.sa_flags = 0;
    t.sa_handler = func;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");
    sigfillset(&mask);
    sigdelset(&mask, SIGALRM);

    // 3. Cear cout-fills concurrents que muten a top en intervals alarm(i*intv)
    for (int i = 0; i < count; ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");
        if (pid == 0)
        {
            if (i > 0)
            {   
                alarm(i*intv);
                sigsuspend(&mask);
            }
            // El fill escriu el missatge per pantalla (index <i> i el seu pid)
            sprintf(buf, "INDEX: %d, PID: %d\n", i, getpid());
            write(1, buf, strlen(buf));
            
            // Muta a la comanda top amb <pid_monitor>
            sprintf(buf, "%d", pid_monitor);
            execlp("top", "top", "-b", "-n", "1", "-p", buf, (char *)NULL);
            error_and_exit("execlp");
        }
    }
    while (waitpid(-1, NULL, 0) > 0);
    
}