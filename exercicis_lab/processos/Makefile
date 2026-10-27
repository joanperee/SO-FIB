# Buscar todos los archivos .c en la carpeta
SRC := $(wildcard *.c)

# Generar la lista de ejecutables a partir de los nombres de los archivos fuente
EXE := $(SRC:.c=)

# Regla para compilar todos los programas
all: $(EXE)

# Regla genérica para compilar cada ejecutable
%: %.c
	gcc -Wall -o $@ $<

# Regla para limpiar los ejecutables generados
clean:
	rm -f $(EXE)
