#ifndef GAME_H
#define GAME_H

#include <locale.h>
#include <string.h>

#include "burger.h"
#include "estoque.h"
#include "pedidos.h"


// Estrutura para guardar o estado atual do jogo
typedef struct estado
{
	int dia; // Dia atual
	float dinheiro;
} Estado;


// inicializa a fila e define os valores iniciais do estado do jogo
void inicializarEstado(Estado *estado)
{
	estado->dia = 1; // O jogo começa no dia 1
	estado->dinheiro = 100.0; // O jogador começa com 100.0 de dinheiro
}


//Menu de pedidos. Gera pedidos aleatórios.
void telaDePedidos(Estado *estadoDoPlayer, Burger *hamburgueres) {

	char input[10] = {'~'};

	while (input[0] != 's') { //Loop principal.
		printf("\nTecle (f) para gerar/mudar os pedidos do dia, e (s) para voltar ao menu inicial.\n\n");

		fgets(input, sizeof(input), stdin);
		input[strcspn(input, "\n")] = '\0'; //Remove newLine char.


		if (strlen(input) == 1 && input[0] == 'f') { //Gera e muda os pedidos do dia a cada iteração.

			FilaPedidos filaDePedidos;

			inicializaFilaPedidos(&filaDePedidos);

			geraPedidos(&filaDePedidos, 8); //Gera uma lista de 10 a 12 pedidos.

			imprimeFilaPedidos(&filaDePedidos, hamburgueres);
		}
		else if (strlen(input) == 1 && input[0] == 's'){
			return;
		}
		else {
			printf("Comando inválido\n\n");
		}
	}
}


void gameplayLoop() {

	setlocale (LC_ALL, "Portuguese"); //Muda a linguagem do terminal para português.

	Burger hamburgueres[MAX_BURGERS];
	Estado estadoDoPlayer;
	Estoque estoque;

	char input[10] = {'~'};
	char returnInput[10] = {'~'};

	int valido = 0; //Usado para a validação do input inicial.

	inicializarEstado(&estadoDoPlayer); //Inicializa estado do player atual.
	inicializarHamburgueres(hamburgueres); //Inicializa hamburgueres no cardápio.
	inicializarEstoque(&estoque);

	while (input[0] != '0') { //Loop principal.

		valido = 0;

		printf("\nBem vindo à Pato Burger!! \n\n Nós temos comida.\n");
		printf("Você deseja?:\n");
		printf("(1) -> Começar o jogo.\n");
		printf("(2) -> Exibir lista de ingredientes.\n");
		printf("(3) -> Exibir cardápio.\n");
		printf("(0) -> Sair do jogo.\n\n");

		while (!valido) {
			fgets(input, sizeof(input), stdin); //Lê o input do usuário.

			input[strcspn(input, "\n")] = '\0'; //Remove newLine char.

			if (strlen(input) == 1 && (input[0] == '1' || input[0] == '2' || input[0] == '3' || input[0] == '0')) { //Verifica se o input tem apenas 1 caractere,
				valido = 1;																	//E se esse caractere é um dos que podem ser colocados ou não.
			} else {
				printf("Não é um dos números válidos. Selecione novamente.\n");
			}
		}

		if (input[0] == '1') {

			telaDePedidos(&estadoDoPlayer, hamburgueres);
		}
		else if (input[0] == '2') {
			exibirEstoque(&estoque);

			printf("\nDigite qualquer tecla para voltar, digite 0 para sair: ");
			fgets(input, sizeof(input), stdin);
		}
		else if (input[0] == '3') {
			exibirCardapio(hamburgueres);

			printf("\nDigite qualquer tecla para voltar, digite 0 para sair: ");
			fgets(input, sizeof(input), stdin);
		}
	}

}

#endif
