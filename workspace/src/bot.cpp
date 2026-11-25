#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "DatosMemCompartida.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    DatosMemCompartida* dat;
    char *direccion;
    int fd;
    
    // X1: Abrir el archivo en memoria que creará el juego Tenis
    fd = open("/tmp/mem_tenis", O_RDWR);
    if (fd == -1) {
        perror("Error al abrir el archivo. Inicia el juego Tenis primero."); 
        return 1;
    }
    
    // X2: Proyectar el archivo en memoria
    direccion = (char*) mmap(NULL, sizeof(DatosMemCompartida), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (direccion == MAP_FAILED) {
        perror("Error en la proyeccion mmap"); 
        return 1;
    }
    close(fd);
    
    dat = (DatosMemCompartida*)direccion;
    printf("Bot iniciado. Controlando raqueta 1 de forma automática...\n");

    while(1)
    {
        usleep(25000);
        
        // X3: Lógica del bot (sigue la coordenada Y de la pelota)
        if (dat->esfera.centro.y > dat->raqueta1.y2) {
            dat->accion = 1;  // Pelota arriba, subir raqueta
        } else if (dat->esfera.centro.y < dat->raqueta1.y1) {
            dat->accion = -1; // Pelota abajo, bajar raqueta
        } else {
            dat->accion = 0;  // Misma altura, detenerse
        }
    }
    return 0;
}
