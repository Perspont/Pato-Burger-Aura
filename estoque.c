#include <stdio.h>
#include <string.h>
#include "estoque.h"

// inicializa a fila
void inicializarEstoque(Estoque *estoque)
{
	estoque->ingredientes[0] = (Ingrediente){1, "Pão", 1.0, 10};
	estoque->ingredientes[1] = (Ingrediente){2, "Carne", 2.0, 10};
	estoque->ingredientes[2] = (Ingrediente){3, "Queijo", 2.0, 10};
	estoque->ingredientes[3] = (Ingrediente){4, "Alface", 1.0, 10};
	estoque->ingredientes[4] = (Ingrediente){5, "Tomate", 1.0, 10};
	estoque->ingredientes[5] = (Ingrediente){6, "Cebola", 1.0, 10};
	estoque->ingredientes[6] = (Ingrediente){7, "Picles", 1.0, 10};
	estoque->ingredientes[7] = (Ingrediente){8, "Bacon", 3.0, 10};
	estoque->ingredientes[8] = (Ingrediente){9, "Burger de cogumelo", 2.0, 10};
	estoque->ingredientes[9] = (Ingrediente){10, "Molho Especial", 1.5, 10};
	estoque->ingredientes[10] = (Ingrediente){11, "Frango", 2.5, 10};
	estoque->ingredientes[11] = (Ingrediente){12, "Peixe", 3.0, 10};
}

void adicionarIngrediente(Estoque *estoque, int id, int quantidade) {
	int i = id - 1;

	if (i < 0 || i >= MAX_INGREDIENTES) {
		printf("Erro: ID do ingrediente %d é inválido.\n", id);
		return;
	}

	if (estoque->ingredientes[i].id == id)
	{
		estoque->ingredientes[i].quantidade += quantidade;
	} else {
		printf("Ingrediente com ID %d não encontrado no estoque.\n", id);
	}
}

void exibirEstoque(Estoque *estoque)
{
	printf("\n======== Estoque ========\n\n");
	for (int i = 0; i < MAX_INGREDIENTES; i++)
	{
		if (estoque->ingredientes[i].id != 0) // Exibe apenas ingredientes válidos
		{
			printf("ID: %d, Nome: %s, Quantidade: %d\n",
				   estoque->ingredientes[i].id,
				   estoque->ingredientes[i].nome,
				   estoque->ingredientes[i].quantidade);
		}
	}
}

void removerIngrediente(Estoque *estoque, int id, int quantidade) {
	int i = id - 1;

	if (i < 0 || i >= MAX_INGREDIENTES) {
		printf("Erro: ID do ingrediente %d é inválido.\n", id);
		return;
	}

	if (estoque->ingredientes[i].id == id)
	{
		if (estoque->ingredientes[i].quantidade >= quantidade) {
			estoque->ingredientes[i].quantidade -= quantidade;
		} else {
			printf("Erro: Quantidade insuficiente do ingrediente %s no estoque.\n", estoque->ingredientes[i].nome);
		}
	} else {
		printf("Ingrediente com ID %d não encontrado no estoque.\n", id);
	}
}