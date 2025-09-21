#ifndef GAME_H
#define GAME_H

#include <locale.h>
#include <string.h>


// Estrutura para guardar o estado atual do jogo
typedef struct
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

void gameplayLoop() {

	setlocale (LC_ALL, "Portuguese"); //Muda a linguagem do terminal para português.

	Burger hamburgueres[MAX_BURGERS];

	char input[10] = {'~'};
	char returnInput[10] = {'~'};

	int valido = 0; //Usado para a validação do input inicial.

	inicializarHamburgueres(hamburgueres); //Inicializa hamburgueres no cardápio.

	while (returnInput[0] != '0') {

		valido = 0;

		printf("\nBem vindo à Pato Burger!! \n\n Nós temos SIM comida.\n");
		printf("Você deseja?:\n");
		printf("(1) -> Começar o jogo.\n");
		printf("(2) -> Exibir lista de ingredientes.\n");
		printf("(3) -> Exibir cardápio.\n\n");

		while (!valido) {
			fgets(input, sizeof(input), stdin); //Lê o input do usuário.

			input[strcspn(input, "\n")] = '\0'; //Remove newLine char.

			if (strlen(input) == 1 && (input[0] == '1' || input[0] == '2' || input[0] == '3')) { //Verifica se o input tem apenas 1 caractere,
				valido = 1;																	//E se esse caractere é um dos que podem ser colocados ou não.
			} else {
				printf("Não é um dos números válidos. Selecione novamente.\n");
			}
		}

		if (input[0] == '1') {
			//exibirDiaDePedidos();
			return;
		}
		else if (input[0] == '2') {
			//exibirEstoque(Estoque *estoque);
			return;
		}
		else if (input[0] == '3') {
			exibirCardapio(hamburgueres);

			printf("\nDigite qualquer tecla para voltar, digite 0 para sair: ");
			fgets(returnInput, sizeof(input), stdin); //Pega o returnInput (Input que sai do while loop maior.
		}
	}

}

#endif
