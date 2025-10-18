#include <stdio.h>
#include <string.h>
#include "../Header/burger.h"
#include "../Header/estoque.h"
#include "../Header/pedidos.h"
#include "../Header/game.h"

#include <stdlib.h>

// inicializa a fila
void inicializarEstado(Estado *estado)
{
	estado->dia = 1;
	estado->dinheiro = 100.0;
}

//Menu de pedidos. Gera pedidos aleatórios.
void telaDePedidos(Estado *estado, Burger *cardapio, FilaPedidos *pedidos) {

	inicializarCardapio(cardapio);
	inicializaFilaPedidos(pedidos);
	inicializarEstado(estado);

	char input[10] = {'~'};

	while (input[0] != 's') { //Loop principal.
		printf("\nDigite (f) para gerar os pedidos de 1 semana, e (s) para voltar ao menu inicial.\n");

		fgets(input, sizeof(input), stdin);
		input[strcspn(input, "\n")] = '\0'; //Remove newLine char.


		if (strlen(input) == 1 && input[0] == 'f') { //Gera e muda os pedidos do dia a cada iteração.

			for (int i = 0; i < 7; i++) { //Roda por 7 dias.

				inicializarCardapio(cardapio);
				inicializaFilaPedidos(pedidos);

				printf("\nIniciando Dia %d\n", estado->dia);

				geraPedidos(pedidos, estado->dia);

				imprimeFilaPedidos(pedidos, cardapio);

				estado->dia++;

				printf("\nDigite qualquer tecla ou ENTER para continuar para o proximo dia ");

				fgets(input, sizeof(input), stdin);


			}

			return;
		}
		else if (strlen(input) == 1 && input[0] == 's'){
			return;
		}
		else {
			printf("Comando invalido\n\n");
		}
	}
}

void gameplayLoop() {

	Burger cardapio[MAX_BURGERS];
	inicializarCardapio(cardapio);
	FilaPedidos pedidos;
	inicializaFilaPedidos(&pedidos);
	Estoque estoque;
	inicializarEstoque(&estoque);
	Estado estado;
	inicializarEstado(&estado);

	char input[10] = {'~'};
	char returnInput[10] = {'~'};

	int valido = 0; //Usado para a validação do input inicial.

	while (input[0] != '0') { //Loop principal.

		valido = 0;

		printf("\nBem vindo ao Pato Burger! Os melhores burgers de Salvador!\n");
		printf("\nVoce deseja...\n");
		printf("\n(1) -> Iniciar o jogo.\n");
		printf("(2) -> Exibir lista de ingredientes.\n");
		printf("(3) -> Exibir cardapio.\n");
		printf("(0) -> Sair do jogo.\n\n");

		while (!valido) {
			fgets(input, sizeof(input), stdin); //Lê o input do usuário.

			input[strcspn(input, "\n")] = '\0'; //Remove newLine char.

			if (strlen(input) == 1 && (input[0] == '1' || input[0] == '2' || input[0] == '3' || input[0] == '0')) { //Verifica se o input tem apenas 1 caractere,
				valido = 1;																	//E se esse caractere é um dos que podem ser colocados ou não.
			} else {
				printf("Numero invalido, Selecione novamente:\n");
			}
		}

		if (input[0] == '1') {

			#ifdef _WIN32 // Limpa o terminal
			system("cls");
			#else
			system("clear");
			#endif

			telaDePedidos(&estado, cardapio, &pedidos);
		}
		else if (input[0] == '2') {

			#ifdef _WIN32
			system("cls");
			#else
			system("clear");
			#endif

			exibirEstoque(&estoque);

			printf("\nDigite qualquer tecla para voltar, digite 0 para sair:\n");
			fgets(input, sizeof(input), stdin);
		}
		else if (input[0] == '3') {
			#ifdef _WIN32
			system("cls");
			#else
			system("clear");
			#endif

			exibirCardapio(cardapio);

			printf("\nDigite qualquer tecla para voltar, digite 0 para sair:\n");
			fgets(input, sizeof(input), stdin);
		}
	}

}
