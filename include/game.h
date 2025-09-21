#ifndef GAME_H
#define GAME_H

#include "include/burger.h"
#include "include/pedidos.h"

typedef struct
{
	int dia;
	float dinheiro;
} Estado;

void inicializarEstado(Estado *estado);
void gameplayLoop();
void telaDePedidos(Estado *estadoDoPlayer, Burger *hamburgueres, FilaPedidos *pedidos);

#endif
