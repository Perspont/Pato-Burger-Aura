#include <stdio.h>
#include "burger.h"
#include "estoque.h"
#include "pedidos.h"
#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main()
{
	Burger cardapio[MAX_BURGERS];
	inicializarCardapio(cardapio);
	FilaPedidos pedidos;
	inicializaFilaPedidos(&pedidos);
	Estoque estoque;
	inicializarEstoque(&estoque);
	Estado estado;
	inicializarEstado(&estado);

	for (int i = 0; i < estado.dia; i++) {
		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif


		printf("Iniciando Dia %d\n", estado.dia);

		sleep(1);

		exibirCardapio(cardapio);

		sleep(1);

		exibirEstoque(&estoque);

		sleep(1);

		geraPedidos(&pedidos, estado.dia);

		sleep(1);

		imprimeFilaPedidos(&pedidos, cardapio);
	}

		return 0;
}