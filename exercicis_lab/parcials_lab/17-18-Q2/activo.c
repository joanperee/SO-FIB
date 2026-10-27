#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

char buf[256];
volatile int signaled = 0;

void error_and_exit(char *msg) {
    perror(msg);
    sprintf(buf, "Valor de l'errno: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname) {
    sprintf(buf, "Usage: %s <secs> <pid>\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void trat_func(int s) {
    if (s == SIGALRM) {
        sprintf(buf, "Ha arribat un SIGALRM\n");
        write(1, buf, strlen(buf));
        signaled = 1;
    }
}

int main(int argc, char **argv) {
    
    // 1. Validació d'arguments
    if (argc != 3) usage(argv[0]);
    int secs = atoi(argv[1]);
    int pid = atoi(argv[2]);
    if (secs <= 0 || pid < 0) usage(argv[0]);

    // 2. Bloquejar SIGALRM
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    // 3. Configurar el manejador de SIGALRM
    struct sigaction t;
    t.sa_flags = 0;
    t.sa_handler = trat_func;
    sigfillset(&t.sa_mask);
    if (sigaction(SIGALRM, &t, NULL) < 0) error_and_exit("sigaction");

    // 4. Desbloquejar SIGALRM i fer l'espera activa amb el bucle while
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");
    alarm(secs);
    while (signaled != 1);

    // 5. Un cop rebuda l'alarma, enviar SIGUSR1 si escau
    if (pid != 0) {
        if (kill(pid, SIGUSR1) < 0) error_and_exit("kill"); 
        sprintf(buf, "S'ha enviat un SIGUSR1 al procés %d\n", pid);
        write(1, buf, strlen(buf));
    }

    // 6. Mostrar el missatge final
    sprintf(buf, "Programa finalitzat.\n");
    write(1, buf, strlen(buf));
    exit(secs);
}