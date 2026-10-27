#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

char buf[256];
int target_pid;

void Error(const char *msg)
{
    perror(msg);
    if (errno != 0) {
        sprintf(buf, "Causa de l'error: %s", strerror(errno));
        write(2, buf, strlen(buf));
    }
    exit(1);
}

void Usage(const char *prog_name)
{
    sprintf(buf, "Usage: %s <num> <paraula> <nom_fitxer>\n", prog_name);
    write(1, buf, strlen(buf));
    sprintf(buf, "Exemple: ./busqueda_parcial 100 paraula busqueda_parcial.c\n");
    write(1, buf, strlen(buf));
    sprintf(buf, "Descripció: el programa muta a grep i cerca la paraula en el fitxer\n");
    write(1, buf, strlen(buf));
    exit(1);
}

void signal_handler(int signal)
{
    if (signal == SIGUSR1) {
        sprintf(buf,"Signal rebut: %d\n", signal);
        write(1, buf, strlen(buf));
    }
    if (signal == SIGUSR2) {
        sprintf(buf,"Signal rebut: %d\n", signal);
        write(1, buf, strlen(buf));
        if (target_pid != 0) {
            kill(target_pid, SIGUSR1);
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        Usage(argv[0]);
    }

    target_pid = atoi(argv[1]);

    // PAS 1. Bloquejar signals per evitar finestra de vulnerabilitat
    sigset_t mask, oldmask, waitmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) < 0) {
        Error("Ha fallat el sigprocmask()");
    }

    // PAS 2. Configurar handler
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) < 0 || sigaction(SIGUSR2, &sa, NULL) < 0) {
        Error("Ha fallat sigaction()");
    }

    // PAS 3. Suspendre l'execució sense consumir CPU mentres no arribin signals
    sigemptyset(&waitmask);
    sigsuspend(&waitmask);

    int pid = fork();
    if (pid == 0) {
        execlp("grep", "grep", argv[2], argv[3], (char *)NULL);
        Error("Ha fallat l'execlp()");
    }
    else if (pid < 0) {
        Error("Ha fallat el fork()");
    }
    else {
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            Error("Ha fallat el waitpid()");
        }
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            sprintf(buf, "Finalitza grep paraula %s fitxer %s : exit(%d)\n", argv[2], argv[3], exit_code);
            write(1, buf, strlen(buf));

            if (argv[1] != "0") {
                if (exit_code == 1) {
                    kill(target_pid, SIGUSR1);
                }
                else if (exit_code == 0) {
                    kill(target_pid, SIGUSR2);
                }
            }
        }
        else {
            sprintf(buf, "El fill no ha finalitzat el procés correctament\n");
            write(1, buf, strlen(buf));
        }
        exit(0);
    }
}