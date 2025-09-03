#include <stdio.h>
#include <string.h>
#include "game.h"

// inicializa a fila
void inicializarEstado(Estado *estado)
{
	estado->dia = 1;
	estado->dinheiro = 100.0;
}
