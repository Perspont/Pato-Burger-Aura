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

	int valido = 0; //Usado para a validação do input inicial.

	telaPrincipalEtapa2();

	#ifdef _WIN32
	system("cls");
	#else
	system("clear");
	#endif

	printf("Obrigado por jogar!");


	return 0;
}