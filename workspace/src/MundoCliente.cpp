// Mundo.cpp: implementation of the CMundo class.
//
//////////////////////////////////////////////////////////////////////
#include <fstream>
#include "MundoCliente.h"
#include "glut.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMundo::CMundo()
{
	Init();
}

CMundo::~CMundo()
{
    if (fd_estado != -1) close(fd_estado);
    if (fd_teclas != -1) {
        close(fd_teclas);
        unlink("/tmp/fifo_teclas");
    }
    if (dat_mem != nullptr && dat_mem != MAP_FAILED) {
        munmap(dat_mem, sizeof(DatosMemCompartida));
    }
}

void CMundo::InitGL()
{
	//Habilitamos las luces, la renderizacion y el color de los materiales
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective( 40.0, 1600/800.0f, 0.1, 150);
}

void print(char *mensaje, int x, int y, float r, float g, float b)
{
	glDisable (GL_LIGHTING);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT) );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColor3f(r,g,b);
	glRasterPos3f(x, glutGet(GLUT_WINDOW_HEIGHT)-18-y, 0);
	int len = strlen (mensaje );
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[i]);
	}
		
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
}
void CMundo::OnDraw()
{
	//Borrado de la pantalla	
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
	gluLookAt(0.0, 0, 17,  // posicion del ojo
		0.0, 0.0, 0.0,      // hacia que punto mira  (0,0,0) 
		0.0, 1.0, 0.0);      // definimos hacia arriba (eje Y)    

	/////////////////
	///////////
	//		AQUI EMPIEZA MI DIBUJO
	char cad[100];
	sprintf(cad,"Jugador1: %d",puntos1);
	print(cad,10,0,1,1,1);
	sprintf(cad,"Jugador2: %d",puntos2);
	print(cad,650,0,1,1,1);
	int i;
	for(i=0;i<paredes.size();i++)
		paredes[i].Dibuja();

	fondo_izq.Dibuja();
	fondo_dcho.Dibuja();
	jugador1.Dibuja();
	jugador2.Dibuja();
	esfera.Dibuja();

	/////////////////
	///////////
	//		AQUI TERMINA MI DIBUJO
	////////////////////////////

	//Al final, cambiar el buffer
	glutSwapBuffers();
}

void CMundo::OnTimer(int value)
{
    if (fd_estado != -1)
    {
        char cad[200] = {0};
        int bytes = read(fd_estado, cad, sizeof(cad));
        if (bytes <= 0) {
            exit(0); // El servidor ha cerrado la partida
        }
        sscanf(cad, "%f %f %f %f %f %f %f %f %f %f %d %d",
               &esfera.centro.x, &esfera.centro.y,
               &jugador1.x1, &jugador1.y1, &jugador1.x2, &jugador1.y2,
               &jugador2.x1, &jugador2.y1, &jugador2.x2, &jugador2.y2,
               &puntos1, &puntos2);
    }

    if (dat_mem != nullptr && dat_mem != MAP_FAILED)
    {
        dat_mem->esfera = esfera;
        dat_mem->raqueta1 = jugador1;

        if (dat_mem->accion == 1) {
            OnKeyboardDown('w', 0, 0);
        } else if (dat_mem->accion == -1) {
            OnKeyboardDown('s', 0, 0);
        } else {
            OnKeyboardDown('x', 0, 0);
        }
    }
}

void CMundo::OnKeyboardDown(unsigned char key, int x, int y)
{
    if (fd_teclas != -1) {
        char cad[100] = {0};
        sprintf(cad, "%c", key);
        write(fd_teclas, cad, sizeof(cad));
    }
}

void CMundo::Init()
{
	Plano p;
	
	//pared inferior
	p.x1=-7;p.y1=-5;
	p.x2=7;p.y2=-5;
	paredes.push_back(p);

	//pared superior
	p.x1=-7;p.y1=5;
	p.x2=7;p.y2=5;
	paredes.push_back(p);

	fondo_izq.r=0;
	fondo_izq.x1=-7;fondo_izq.y1=-5;
	fondo_izq.x2=-7;fondo_izq.y2=5;

	fondo_dcho.r=0;
	fondo_dcho.x1=7;fondo_dcho.y1=-5;
	fondo_dcho.x2=7;fondo_dcho.y2=5;

	//a la izq
	jugador1.g=0;
	jugador1.x1=-6;jugador1.y1=-1;
	jugador1.x2=-6;jugador1.y2=1;

	//a la dcha
	jugador2.g=0;
	jugador2.x1=6;jugador2.y1=-1;
	jugador2.x2=6;jugador2.y2=1;
	
	// mov de la esfera
	esfera.velocidad.x = 3.0f;
	esfera.velocidad.y = 3.0f;
	
	// Memoria compartida para el Bot
        dat_mem = nullptr;
        int fd_mem = open("/tmp/mem_tenis", O_CREAT | O_RDWR | O_TRUNC, 0666);
        if (fd_mem != -1) {
            ftruncate(fd_mem, sizeof(DatosMemCompartida));
            dat_mem = (DatosMemCompartida*)mmap(NULL, sizeof(DatosMemCompartida), PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, 0);
            close(fd_mem);
        }

        // Crear FIFO de teclas ANTES de desbloquear al servidor
        unlink("/tmp/fifo_teclas");
        mkfifo("/tmp/fifo_teclas", 0666);

        // Abrir FIFO de estado (desbloquea al Servidor)
        fd_estado = open("/tmp/fifo_estado", O_RDONLY);

        // Abrir FIFO de teclas en escritura (se empareja con el hilo del servidor)
        fd_teclas = open("/tmp/fifo_teclas", O_WRONLY);
}
