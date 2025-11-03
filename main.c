#include <stdio.h>
#include "Header/burger.h"
#include "Header/estoque.h"
#include "Header/pedidos.h"
#include "Header/game.h"
#include "Header/gameplayEtapa2.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>


int main()
{
	char input[10] = {'~'};
	char returnInput[10] = {'~'};

	GameState state;

	int valido = 0; //Usado para a validação do input inicial.

	while (input[0] != '0') {
		//Loop principal.

		valido = 0;

		printf("Quer jogar a versão da etapa 1 ou 2?\n");
		printf("Escreva [1] se 1 [2] se 2: \n");

		while (!valido) {
			fgets(input, sizeof(input), stdin); //Lê o input do usuário.

			input[strcspn(input, "\n")] = '\0'; //Remove newLine char.

			if (strlen(input) == 1 && (input[0] == '1' || input[0] == '2')) { //Verifica se o input tem apenas 1 caractere,
				valido = 1;																	//E se esse caractere é um dos que podem ser colocados ou não.
			} else {
				printf("Numero invalido, Selecione novamente:\n");
			}
		}

		if (input[0] == '1') {
			gameplayLoop();
		}
		else if (input[0] == '2') {
			telaPrincipalEtapa2(&state);
		}
	}

	#ifdef _WIN32
	system("cls");
	#else
	system("clear");
	#endif

	printf("Obrigado por jogar!");


	return 0;
}