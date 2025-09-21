/*

#include <stdio.h>
#include <string.h>
#include "game.h"

#include "burger.h"

// inicializa a fila e define os valores iniciais do estado do jogo
void inicializarEstado(Estado *estado)
{
	estado->dia = 1; // O jogo começa no dia 1
	estado->dinheiro = 100.0; // O jogador começa com 100.0 de dinheiro
}

void gameplayLoop() {

	Burger hamburgueres[MAX_BURGERS];

	char input = '~';
	char returnInput = '~';

	inicializarHamburgueres(hamburgueres); //Inicializa hamburgueres no cardápio.

	printf("Bem vindo à Pato Burger!! \n\n Nós temos SIM comida.\n");
	printf("Você deseja?:\n");
	printf("(1) -> Começar o jogo.\n");
	printf("(2) -> Exibir lista de ingredientes.\n");
	printf("(3) -> Exibir cardápio.\n\n");

	while (input != '1' && input != '2' && input != '3') {
		scanf("%c", &input);
	}

	if (input == '1') {
		return; //Exibir 1 dia de pedidos (Fila de pedidos).
	}
	else if (input == '2') {
		return; //Exibir 1 dia de pedidos (Fila de pedidos).
	}
	else if (input == '3') {
		exibirCardapio(hamburgueres);
	}
}

*/