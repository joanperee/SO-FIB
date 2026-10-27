#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

char buf[1024];

void error(const char *msg) {
    perror(msg); 
    exit(EXIT_FAILURE);
}

void Usage(const char *progname)
{
    sprintf(buf, "Usage: %s <filename>\n", progname);
    write(1, buf, strlen(buf));
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        Usage(argv[0]);
    }

    const char *filename = argv[1];
    
    // Abrir el fichero en modo lectura/escritura
    int fd = open(filename, O_RDWR);
    if (fd == -1) error("open()");

    // Obtener el tamaño del fichero
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) error("lseek()");

    // Si el fichero tiene menos de dos caracteres, no podemos insertar 'X'
    if (file_size < 2) {
        close(fd);
        error("File is too short to insert an 'X'");
    }

    // Volver al principio del fichero
    lseek(fd, 0, SEEK_SET);

    // Leer el contenido del fichero
    char *content = malloc(file_size + 2); // +2 para el nuevo carácter 'X' y el terminador nulo
    if (content == NULL) error("malloc()");

    ssize_t bytes_read = read(fd, content, file_size);
    if (bytes_read == -1) error("Error reading file");

    content[bytes_read] = '\0'; // Aseguramos que el contenido esté bien terminado

    // Insertar 'X' entre el penúltimo y último carácter
    content[file_size] = content[file_size - 1];  // Desplazamos el último carácter
    content[file_size - 1] = 'X';                  // Insertamos 'X' en la penúltima posición

    // Volver al principio del fichero para sobrescribirlo
    lseek(fd, 0, SEEK_SET);
    ssize_t bytes_written = write(fd, content, file_size + 1);  // Escribimos el contenido modificado
    if (bytes_written == -1) error("Error writing to file");

    // Output del contenido actualizado
    sprintf(buf, "Char 'X' successfully added to file %s between last and second to last chars\n", filename);
    write(1, buf, strlen(buf));

    sprintf(buf, "The new content is: %s\n", content);
    write(1, buf, strlen(buf));

    // Limpiar y cerrar
    free(content);
    close(fd);

    return 0;
}
