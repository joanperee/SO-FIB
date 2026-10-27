#include <unistd.h>
#include <stdio.h>
#include <string.h> 

#define MAX_SIZE 8

// PRE: '0' <= c <= '9'
// POST: returns the integer which c represented
unsigned int char2int(char c)
{
    return c - '0';
}

// PRE: str is a pointer to a valid string
// POST: returns 1 if the string represented a valid number, 0 otherwise
int esNumero(char *str)
{
    if (str)
    {
		int i = 0;
        if (str[0] == '-')
		{
			++str; 
		}  
        for (i = 0; str[i] != '\0'; ++i)
        {
            if (str[i] < '0' || str[i] > '9') return 0;
        }
        return (i > 0) && (i <= MAX_SIZE);  
    }
    return 0;
}

// PRE: s is a pointer to a valid string
// POST: returns the integer represented by s, 0 otherwise
int mi_atoi(char *s)
{
    if (esNumero(s) == 0) return 0;
        
    int result = 0;
    int signe = 1;
    int i = 0;

    if (s[0] == '-') 
    {
        signe = -1;
        ++s;    
    }
    while (s[i] != '\0')
    {
        result = result*10 + char2int(s[i]);
        ++i;
    }
    return signe*result;
}

int main(int argc, char *argv[])
{
    int suma = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (esNumero(argv[i]) == 0)
        {
            printf("Error: el parámetro \"%s\" no es un número\n", argv[i]);
            return 1;
        }
        else
        {
            int valor = mi_atoi(argv[i]);
            suma = suma + valor;
        }
    }
    printf("La suma es %d\n", suma);
    return 0;
}
