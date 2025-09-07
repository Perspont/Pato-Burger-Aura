#ifndef GAME_H
#define GAME_H

typedef struct
{
	int dia;
	float dinheiro;
} Estado;

void inicializarEstado(Estado *estado);

#endif