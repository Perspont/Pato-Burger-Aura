#include <stdio.h>
#include "burger.h"

// inicializa a fila e define o cardápio inicial com os hambúrgueres disponíveis, seus preços e ingredientes
void inicializarCardapio(Burger *cardapio)
{
	cardapio[0] = (Burger){1, "Bit and Bacon", 16.0, {1, 2, 3, 8, 1}};
	cardapio[1] = (Burger){2, "Duck Cheese", 14.0, {1, 3, 5, 4, 1}};
	cardapio[2] = (Burger){3, "Quackteirão", 19.0, {1, 2, 3, 1}};
	cardapio[3] = (Burger){4, "Big Pato", 27.0, {1, 2, 4, 1, 2, 3, 4, 1}};
	cardapio[4] = (Burger){5, "Zero e Um", 16.0, {1, 2, 3, 1}};
	cardapio[5] = (Burger){6, "Burg TESTE", 5.0, {1, 2, 3, 1}};
	cardapio[6] = (Burger){7, "Burg TESTE 2", 5.0, {1, 2, 3, 1}};
	cardapio[7] = (Burger){8, "Burg TESTE 3", 5.0, {1, 2, 3, 1}};
	cardapio[8] = (Burger){9, "Burg TESTE 4", 5.0, {1, 2, 3, 1}};
	cardapio[9] = (Burger){10, "Burg TESTE 5", 5.0, {1, 2, 3, 1}};
}
// Exibe o cardápio completo para o player
void exibirCardapio(Burger *cardapio)
{
	printf("\n======== Cardápio ========\n\n");
	for (int i = 0; i < MAX_BURGERS; i++)
	{
		printf("%d. %s - R$ %.2f\n", cardapio[i].id, cardapio[i].nome, cardapio[i].preco);
	}
}
