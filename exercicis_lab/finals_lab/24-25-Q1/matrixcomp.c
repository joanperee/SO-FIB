#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

// PROGRAMA INFUMABLE
// FALTA COMPLETAR EL CODI DEL PARE PERQUE IMPRIMEIXI ELS FILLS EN ORDRE

int permis1 = 0;
int permis2 = 0;
int ret1;
int ret2;
int esperafill1 = 0;
int esperafill2 = 0;
int fills_acabats = 0;
int permis_otorgat = 0; // 0 es ningu , 1 es el primer, 2 el segon
void Usage()
{
    char buffer[256];
    sprintf(buffer, "Usage: ./matrixcomp <file.out>\n");
    write(2, buffer, strlen(buffer));
    exit(EXIT_FAILURE);
}

void error_and_exit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void tractament_sigusr1()
{
    if (permis_otorgat == 0)
    {
        kill(ret1, SIGUSR1);
        permis_otorgat = 1;
    }

    else
    {
        esperafill1 = 1;
    }
}

void tractament_sigusr2()
{
    if (permis_otorgat == 0)
    {
        kill(ret2, SIGUSR2);
        permis_otorgat = 2;
    }

    else
    {
        esperafill2 = 1;
    }
}

void tractament_sigusr1_fill1()
{
    permis1 = 1;
}

void tractament_sigusr2_fill2()
{
    permis2 = 1;
}

void tractament_sigalrm()
{
    int stat;
    int pid = waitpid(-1, &stat, WNOHANG);
    if (pid > 0)
    {
        ++fills_acabats;
        int code;
        if (WIFEXITED(stat))
        {
            code = WEXITSTATUS(stat);
        }
        else
        {
            code = -1;
        }

        char buffer[256];
        sprintf(buffer, "Fill acabat: PID= %d, codi de finalització %d\n", pid, code);
        write(1, buffer, strlen(buffer));

        permis_otorgat = 0;

        if (esperafill1)
        {
            kill(ret1, SIGUSR1);
            esperafill1 = 0;
            permis_otorgat = 1;
        }

        else if (esperafill2)
        {
            kill(ret2, SIGUSR2);
            esperafill2 = 0;
            permis_otorgat = 2;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage();
    }

    int sumad1 = 0;
    int sumad2 = 0;

    //---------- REPROGRAMACIÓ ----------//

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGALRM);

    struct sigaction t_usr1;
    t_usr1.sa_flags = 0;
    t_usr1.sa_handler = tractament_sigusr1;
    sigfillset(&t_usr1.sa_mask);
    if (sigaction(SIGUSR1, &t_usr1, 0) < 0)
    {
        error_and_exit("sigaction");
    }

    struct sigaction t_usr2;
    t_usr2.sa_flags = 0;
    t_usr2.sa_handler = tractament_sigusr2;
    sigfillset(&t_usr2.sa_mask);
    if (sigaction(SIGUSR2, &t_usr2, 0) < 0)
    {
        error_and_exit("sigaction");
    }

    struct sigaction t_alrm;
    t_alrm.sa_flags = 0;
    t_alrm.sa_handler = tractament_sigalrm;
    sigfillset(&t_alrm.sa_mask);
    if (sigaction(SIGALRM, &t_alrm, 0) < 0)
    {
        error_and_exit("sigaction");
    }

    //---------- FILL1 ----------//

    ret1 = fork();
    if (ret1 < 0)
    {
        error_and_exit("fork ret1");
    }

    if (ret1 == 0)
    {
        struct sigaction t_usr1_fill;
        t_usr1_fill.sa_flags = 0;
        t_usr1_fill.sa_handler = tractament_sigusr1_fill1;
        sigfillset(&t_usr1_fill.sa_mask);
        if (sigaction(SIGUSR1, &t_usr1_fill, 0) < 0)
        {
            error_and_exit("sigaction");
        }

        int fd_lect_d1 = open(argv[1], O_RDONLY, 0666);
        if (fd_lect_d1 < 0)
        {
            error_and_exit("open lectura diagonal 1");
        }

        // LECTURA DE LA DIAGONAL1
        for (int i = 0; i < 5; ++i)
        {
            // Hem de demanar permis per llegir l'element.
            kill(getppid(), SIGUSR1);
            while (!permis1){};
            permis1 = 0;
            int offset_d1 = (5 * i + i) * sizeof(int);
            lseek(fd_lect_d1, offset_d1, SEEK_SET);
            int num;
            if (read(fd_lect_d1, &num, sizeof(int)) != sizeof(int))
            {
                error_and_exit("read");
            }

            sumad1 += num;
        }

        kill(getppid(), SIGALRM);
        exit(sumad1);
    }

    //---------- FILL2 ----------//

    ret2 = fork();
    if (ret2 < 0)
    {
        error_and_exit("fork ret2");
    }

    if (ret2 == 0)
    {
        struct sigaction t_usr2_fill;
        t_usr2_fill.sa_flags = 0;
        t_usr2_fill.sa_handler = tractament_sigusr2_fill2;
        sigfillset(&t_usr2_fill.sa_mask);
        if (sigaction(SIGUSR2, &t_usr2_fill, 0) < 0)
        {
            error_and_exit("sigaction");
        }

        int fd_lect_d2 = open(argv[1], O_RDONLY, 0666);
        if (fd_lect_d2 < 0)
        {
            error_and_exit("open lectura diagonal 1");
        }

        // LECTURA DE LA DIAGONAL2
        for (int i = 0; i < 5; ++i)
        {
            // Hem de demanar permis per llegir l'element.
            kill(getppid(), SIGUSR2);
            while (!permis2)
                ; // Espera activa a rebre el permis.
            permis2 = 0;
            int offset_d2 = (5 * i + (4 - i)) * sizeof(int);
            lseek(fd_lect_d2, offset_d2, SEEK_SET);
            int num;
            if (read(fd_lect_d2, &num, sizeof(int)) != sizeof(int))
            {
                error_and_exit("read");
            }

            sumad2 += num;
        }

        kill(getppid(), SIGALRM);
        exit(sumad2);
    }

    //---------- PARE ----------//

    // Falta la part dels permisos
    while (fills_acabats < 2)
    {
        sigsuspend(&mask);
    }
}