#pragma once
#include <vector>
#include "Plano.h"
#include "Esfera.h"
#include "Raqueta.h"
#include "DatosMemCompartida.h"

class CMundo  
{
public:
	void Init();
	CMundo();
	virtual ~CMundo();	

	void InitGL();	
	void OnKeyboardDown(unsigned char key, int x, int y);
	void OnTimer(int value);
	void OnDraw();	

	Esfera esfera;
	std::vector<Plano> paredes;
	Plano fondo_izq;
	Plano fondo_dcho;
	Raqueta jugador1;
	Raqueta jugador2;

	int puntos1;
	int puntos2;
	DatosMemCompartida* dat_mem;
	int fd_estado;
	int fd_teclas;
};
