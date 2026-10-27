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
const int MAX_PID = 472;
int sig = 0;

void error_and_exit(const char *msg)
{
    perror(msg);
    sprintf(buf, "Causa: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(const char *progname)
{
    sprintf(buf, "Usage: %s <nhijos1> <nhijos2>\n", progname);
    write(2, buf, strlen(buf));
    sprintf(buf, "With 1 <= nhijos1 <= 10 and 1 <= nhijos2 <= 5\n");
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int validar_enter(const char *arg, int min, int max) 
{
    char *endptr;
    errno = 0;
    long val = strtol(arg, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || val < min || val > max) 
    {
        return -1;
    }
    return (int)val;
}

void trat_func(int s)
{
    sig = s;
}

int main(int argc, char **argv)
{
    // 1. Validar paràmetres argv
    if (argc != 3) usage(argv[0]);
    int n1 = validar_enter(argv[1], 1, 10);
    int n2 = validar_enter(argv[2], 1, 5);
    if (n1 == -1 || n2 == -1) usage(argv[0]);

    // 2. Bloqueig i reprogramació de SIGUSR1 i SIGALRM (SIGUSR2 només es reprograma sense bloquejar-se)
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1); 
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");
    
    struct sigaction t;
    t.sa_handler = trat_func;
    t.sa_flags = SA_RESETHAND;
    sigemptyset(&t.sa_mask);
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGUSR2, &t, NULL) < 0) error_and_exit("sigaction");

    sigfillset(&mask);
    sigdelset(&mask, SIGALRM);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);

    // 3. El programa (procés pare) escriu per pantalla
    sprintf(buf, "Soy el proceso %d, raiz de la jerarquia\n", getpid());
    write(1, buf, strlen(buf));

    // 4. Es creen <nhijos1> processos concurrents de 1r nivell
    char pids1[n1];
    char pids2[n2];
    for (int i = 0; i < n1; ++i)
    {
        pids1[i] = fork(); 
        if (pids1[i] < 0) error_and_exit("fork");
        if (pids1[i] == 0)
        {
            // 5. Es creen <nhijos2> processos concurrents de 2n nivell
            for (int j = 0; j < n2; ++j)
            {
                pids2[j] = fork();  
                if (pids2[j] < 0) error_and_exit("fork");
                if (pids2[j] == 0)
                {
                    sigsuspend(&mask);
                    int sigtype = (sig == SIGUSR1 ? 1 : 2);
                    sprintf(buf, "Soy el proceso %d (hijo del proceso %d):  Recibido signal User defined signal %d (%d)\n", getpid(), getppid(), sigtype, sig);
                    write(1, buf, strlen(buf));
                }
            }

            sigsuspend(&mask);
            int sigtype = (sig == SIGUSR1 ? 1 : 2);
            sprintf(buf, "Soy el proceso %d (hijo del proceso %d):  Recibido signal User defined signal %d (%d)\n", getpid(), getppid(), sigtype, sig);
            write(1, buf, strlen(buf));
        }
    }
    
    // 6. Programació d'alarma i suspensió passiva de SIGALRM, SIGUSR1 o SIGUSR2
    alarm(60); 
    sigsuspend(&mask);

    // 7. Quan arriba l'alarma s'envia un SIGUSR1 a tots els fills del primer nivell
    for (int i = 0; i < n1; ++i)
    {
        if (kill(pids1[i], SIGUSR1) < 0) error_and_exit("kill");
    }

    // 8. Esperam a la finalització de tots els fills
    while (waitpid(-1, NULL, 0) > 0);

    // 9. Acabament del procés ./crearJP
    exit(EXIT_SUCCESS);
}