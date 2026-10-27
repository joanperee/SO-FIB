#include <unistd.h>
#include <string.h> 

#define MAX_SIZE 8

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

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (esNumero(argv[i]))
        {
            write(1, "IT IS A NUMBER\n", strlen("IT IS A NUMBER\n"));
        }
        else
        {
            write(1, "IT IS NOT A NUMBER\n", strlen("IT IS NOT A NUMBER\n"));
        }
    }
    return 0;
}
