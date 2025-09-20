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
	// Declaração das principais estruturas de dados do jogo e inicialização dos dados do jogo
	Burger cardapio[MAX_BURGERS];
	inicializarHamburgueres(cardapio);
	FilaPedidos pedidos;
	inicializaFilaPedidos(&pedidos);
	Estoque estoque;
	inicializarEstoque(&estoque);
	Estado estado;
	inicializarEstado(&estado);

	// Loop principal do jogo, simula a passagem dos dias
	for (int i = 0; i < estado.dia; i++) {
		// Limpa a tela do console para uma nova exibição
		#ifdef _WIN32
			system("cls"); //Windows
		#else
			system("clear") //Linux  ou Mac
		#endif


		printf("Iniciando Dia %d\n", estado.dia);

		sleep(1); // Pausa a execução por 1 segundo

		exibirCardapio(cardapio); // Mostra o cardápio de hambúrgueres

		sleep(1);

		exibirEstoque(&estoque); // Mostra a quantidade atual de ingredientes

		sleep(1);

		geraPedidos(&pedidos, estado.dia); // Gera novos pedidos para o dia

		sleep(1);

		imprimeFilaPedidos(&pedidos, cardapio); // Exibe a fila de pedidos gerada
	}

		return 0;
}
