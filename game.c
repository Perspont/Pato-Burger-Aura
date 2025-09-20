#include <stdio.h>
#include <string.h>
#include "game.h"

// inicializa a fila e define os valores iniciais do estado do jogo
void inicializarEstado(Estado *estado)
{
	estado->dia = 1; // O jogo começa no dia 1
	estado->dinheiro = 100.0; // O jogador começa com 100.0 de dinheiro
}
