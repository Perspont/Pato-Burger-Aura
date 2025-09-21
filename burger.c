#include <stdio.h>
#include "burger.h"

// inicializa a fila
void inicializarCardapio(Burger *cardapio)
{
	cardapio[0] = (Burger){1, "Bit and Bacon", 16.0, {1, 2, 3, 8, 1}};
	cardapio[1] = (Burger){2, "Duck Cheese", 16.0, {1, 3, 5, 4, 1}};
	cardapio[2] = (Burger){3, "Quackteirao", 16.0, {1, 4, 3, 2, 3, 1}};
	cardapio[3] = (Burger){4, "Big Pato", 27.0, {1, 2, 4, 1, 2, 3, 4, 1}};
	cardapio[4] = (Burger){5, "Zero e Um", 13.0, {1, 3, 2, 1}};
	cardapio[5] = (Burger){6, "Chicken Duckey", 21.0, {1, 10, 7, 13, 4, 1}};
	cardapio[6] = (Burger){7, "Pato sobre rodas", 24.0, {1, 10, 11, 6, 5, 4, 3, 2, 1}};
	cardapio[7] = (Burger){8, "Recursivo", 35.0, {1, 10, 6, 3, 2, 3, 2, 4, 8, 5, 8, 3, 2, 10, 1}};
	cardapio[8] = (Burger){9, "Pato Verde", 21.0, {1, 10, 7, 8, 3, 9, 1}};
	cardapio[9] = (Burger){10, "Pickles and MAYO", 25.0, {1, 7, 10, 6, 7, 10, 1}};
}

void exibirCardapio(Burger *cardapio)
{
	printf("\n======== Cardapio ========\n\n");
	for (int i = 0; i < MAX_BURGERS; i++)
	{
		printf("%d. %s - R$ %.2f\n", cardapio[i].id, cardapio[i].nome, cardapio[i].preco);
	}
}
