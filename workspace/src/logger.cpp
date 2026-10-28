#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    int fd;
    char buffer[1024];
    const char *fifo_name = "/tmp/tenis_fifo"; 

    unlink(fifo_name);
    
    if (mkfifo(fifo_name, 0666) < 0) {
        perror("No puede crearse el FIFO");
        return(1);
    }
    
    if ((fd = open(fifo_name, O_RDONLY)) < 0) {
        perror("No puede abrirse el FIFO");
        return(1);
    }
    
    printf("Logger iniciado. Esperando datos del juego...\n");

    int bytes_leidos;
    while ((bytes_leidos = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_leidos] = '\0';
        printf("%s", buffer); 
        
        if (strstr(buffer, "total de 3") != NULL) {
            printf("¡3 puntos alcanzados! Cerrando logger...\n");
            break;
        }
    }
    
    close(fd);
    unlink(fifo_name);
    return(0);
}
