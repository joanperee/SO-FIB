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
    sprintf(buf, "Usage: %s <PID_1> [PID_2] ... [PID_N]\n", prog_name);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 2) usage(argv[0]);

    int nproc = argc - 1;

    for (int i = 0; i < nproc; ++i)
    {
        int pid = fork();
        if (pid < 0) error_and_exit("fork");

        if (pid == 0)
        {
            int state_pid = atoi(argv[i + 1]);

            // Comprovem si el PID existeix mitjançant la crida a "grep"
            sprintf(buf, "/proc/%d/status", state_pid);
            int grep_status = execlp("grep", "grep", "State", buf, (char *)NULL);

            if (grep_status != 0)
            {
                // Si "grep" retorna un codi d'error, vol dir que el PID no existeix
                sprintf(buf, "Proceso %d no existe\n", state_pid);
                write(1, buf, strlen(buf));
                exit(EXIT_FAILURE);  // Sortim immediatament del fill
            }
            else
            {
                // Si el PID existeix, cridem "grep" per obtenir l'estat
                sprintf(buf, "FILL <%d>: mutaré per obtenir informació del PID %d\n", getpid(), state_pid);
                write(1, buf, strlen(buf));
                
                sprintf(buf, "/proc/%d/status", state_pid);
                if (execlp("grep", "grep", "State", buf, (char *)NULL) < 0) error_and_exit("execlp");
            }
        }
    }

    // Esperem que tots els fills acabin, però si un fill ha acabat amb un error, sortim ràpidament
    int status;
    while (waitpid(-1, &status, 0) > 0)
    {
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                // Si algun fill ha acabat amb error, el procés pare termina immediatament
                exit(EXIT_FAILURE);
            }
        }
    }

    sprintf(buf, "PARE: tots els fills han acabat\n");
    write(1, buf, strlen(buf));
}
