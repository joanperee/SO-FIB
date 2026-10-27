#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

char buf[256];

void error_and_exit(char *msg) {
    perror(msg);
    sprintf(buf, "Valor de l'errno: %s\n", strerror(errno));
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void usage(char *progname) {
    sprintf(buf, "Usage: %s <secs> <numprocs>, with secs >= 0 and numprocs > 0\n", progname);
    write(2, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

void func_handler(int s) {
    if (s == SIGCHLD) {
        sprintf(buf, "Signal %d recibido (SIGCHLD)\n", s);
        write(1, buf, strlen(buf));
    } else if (s == SIGUSR1) {
        sprintf(buf, "Signal %d recibido (SIGUSR1)\n", s);
        write(1, buf, strlen(buf));
    }
}

int main(int argc, char **argv) {

    // 1. Validación de argumentos
    if (argc != 3) usage(argv[0]);
    int secs = atoi(argv[1]);
    int numprocs = atoi(argv[2]);
    if (secs <= 0 || numprocs <= 0) usage(argv[0]);

    // 2. Bloquear señales SIGUSR1 y SIGCHLD antes de reprogramarlas
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    // 3. Configurar manejadores de señales
    struct sigaction t;
    t.sa_flags = 0;
    t.sa_handler = func_handler;
    sigemptyset(&t.sa_mask); // Solo bloquea la señal que maneja
    if (sigaction(SIGUSR1, &t, NULL) < 0) error_and_exit("sigaction");
    if (sigaction(SIGCHLD, &t, NULL) < 0) error_and_exit("sigaction");

    // 4. Crear el proceso hijo que ejecuta activo.c
    int pid = fork();
    if (pid < 0) error_and_exit("fork");
    if (pid == 0) {
        sprintf(buf, "%d", getppid());
        execlp("./activo", "activo", argv[1], buf, (char *)NULL);
        error_and_exit("execlp");
    }

    // 5. Desbloquear SIGUSR1 y SIGCHLD y esperar una señal
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) error_and_exit("sigprocmask");

    sigset_t waitmask;
    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGUSR1);
    sigdelset(&waitmask, SIGCHLD);
    sigsuspend(&waitmask);
    
    if (waitpid(pid, NULL, 0) < 0) error_and_exit("waitpid");

    // 7. Decrementar numprocs y volver a ejecutar padre.c si aún quedan procesos
    numprocs--;
    if (numprocs == 0) {
        sprintf(buf, "No quedan más procesos, terminando ejecución.\n");
        write(1, buf, strlen(buf));
        exit(EXIT_SUCCESS);
    }

    sprintf(buf, "%d", numprocs);
    execlp("./padre", "padre", argv[1], buf, (char *)NULL);
    error_and_exit("execlp");
}
