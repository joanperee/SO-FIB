#include <unistd.h> // write() function
#include <stdio.h>	// sprintf() function
#include <string.h> // strlen() function

int main(int argc, char *argv[])
{
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
