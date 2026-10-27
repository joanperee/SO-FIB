#include <unistd.h>
#include <stdio.h>
#include <string.h>

void Usage(char *progName)
{
    char buf[120]; 
    sprintf(buf, "Usage: %s arg1 [arg2..argn]\n", progName);
    write(1, buf, strlen(buf));
    write(1, "Este programa escribe por su salida la lista de argumentos que recibe\n", 71); 
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        Usage(argv[0]);
        return -1;
    }

    char buf[80];
    for (int i = 0; i < argc; i++)
    {
        if (i == 0)
        {
            sprintf(buf, "El argumento %d es %s (es el nombre del ejecutable)\n", i, argv[i]);
        }
        else
        {
            sprintf(buf, "El argumento %d es %s\n", i, argv[i]);
        }
        write(1, buf, strlen(buf));
    }
    return 0;
}