#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char buff[100];

void suma(int op1, int op2, int *res)
{
  *res = op1 + op2;
}

int 
main(int argc, char *argv[])
{
  int i, j;
  int *s; // 's' és un punter a un enter

  if (argc < 3)
  {
    printf("Usage: ./suma <op1> <op2>\n");
    exit(EXIT_FAILURE);
  }

  i = atoi(argv[1]); // Convierte el primer argumento a entero
  j = atoi(argv[2]); // Convierte el segundo argumento a entero

  s = (int *)malloc(sizeof(int)); 
  if (s == NULL)
  {
    printf("Error al asignar memoria para 's'.\n");
    exit(EXIT_FAILURE);
  }

  suma(i, j, s);

  sprintf(buff, "Suma de %d y %d es %d\n", i, j, *s);
  write(1, buff, strlen(buff));
  free(s);
}