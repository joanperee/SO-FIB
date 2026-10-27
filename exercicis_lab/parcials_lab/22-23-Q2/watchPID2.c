#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

char buf[256];
int fills_morts = 0;
struct Info
{
    int exit;   // 1 si acaba amb èxit, 0 altrament
    int code;   // codi d'acabament (status)
};
struct Info V[10];
int pids[10];

void error_and_exit(const char *msg)
{
    perror(msg);
    if (errno != 0)
    {
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

void func_chld()
{
    int st;
    int mort;
    while ((mort = waitpid(-1, &st, WNOHANG)) > 0) 
    {
        int i = 0;
        while (pids[i] != mort) ++i;
        ++fills_morts;
        if (WIFEXITED(st))
        {
            V[i].exit = 1; V[i].code = WEXITSTATUS(st);
        }
        else if (WIFSIGNALED(st))
        {
            V[i].exit = 0; V[i].code = WTERMSIG(st);
        }
    }
}

int main(int argc, char **argv)
{
    // 1. Validar paràmetres d'entrada i emmagatzemar-los
    if (argc != 3 && argc != 4)
        usage(argv[0]);
    int intv = atoi(argv[1]);
    if (intv < 1 || intv > 5)
        usage(argv[0]);
    int count = atoi(argv[2]);
    if (count < 1 || count > 10)
        usage(argv[0]);
    int pid_monitor = (argc == 4 ? atoi(argv[3]) : getpid());

    // 2. Bloquejar SIGALRM, SIGUSR1, reprogramar-los (SIGCHLD també) i preparar l'espera passiva
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
        error_and_exit("sigprocmask");
    struct sigaction t;
    t.sa_flags = 0;
    t.sa_handler = func;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0)
        error_and_exit("sigaction");
    t.sa_handler = func_chld;
    if (sigaction(SIGCHLD, &t, NULL) < 0)
        error_and_exit("sigaction");

    // 3. Cear cout-fills concurrents que muten a top en intervals alarm(i*intv)
    for (int i = 0; i < count; ++i)
    {
        pids[i] = fork();
        if (pids[i] < 0)
            error_and_exit("fork");
        if (pids[i] == 0)
        {
            if (sigaction(SIGUSR1, &t, NULL) < 0)
                error_and_exit("sigaction");
            sigemptyset(&mask);
            sigsuspend(&mask);
            if (i > 0)
            {
                sigfillset(&mask);
                sigdelset(&mask, SIGALRM);
                alarm(i * intv);
                sigsuspend(&mask);
            }
            // El fill escriu el missatge per pantalla (index <i> i el seu pid)
            write(1, "\n", 2);
            sprintf(buf, "INDEX: %d, PID: %d\n", i, getpid());
            write(1, buf, strlen(buf));

            // Muta a la comanda top amb <pid_monitor>
            sprintf(buf, "%d", pid_monitor);
            execlp("top", "top", "-b", "-n", "1", "-p", buf, (char *)NULL);
            error_and_exit("execlp");
        }
    }

    // 4. Bucle per enviar signals als fills
    for (int i = 0; i < count; ++i)
    {
        if (i % 2) // Si l'índex és senar s'envia SIGUSR1
        {
            if (kill(pids[i], SIGUSR1) < 0)
                error_and_exit("kill");
        }
        else // Si l'índex és parell s'envia SIGUSR2
        {
            if (kill(pids[i], SIGUSR2) < 0)
                error_and_exit("kill");
        }
    }

    // 5. Espera activa amb el sigchld
    while (fills_morts < count);

    // 6. Imprimir el SUMMARY: cada procés fill imprimeix el seu índex, el seu pid i la forma d'acabament
    sprintf(buf, "\nSUMMARY\n");
    write(1, buf, strlen(buf));
    for (int i = 0; i < count; ++i)
    {
        sprintf(buf, "El procés amb índex %d i PID %d ha acabat per %s amb valor %d\n", i, pids[i], (V[i].exit ? "exit" : "signal"), V[i].code);
        write(1, buf, strlen(buf));
    }
}