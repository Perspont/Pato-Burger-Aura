#include <stdio.h>
#include <string.h>
#include "../Header/estoque.h"

void inicializa_Pao(Ingrediente *ing)
{
	*ing = (Ingrediente){1, "Pao", 2.0, 10};
}

void inicializa_Hamburguer(Ingrediente *ing)
{
	*ing = (Ingrediente){2, "Hamburguer", 5.0, 10};
}

void inicializa_Queijo(Ingrediente *ing)
{
	*ing = (Ingrediente){3, "Queijo", 3.0, 10};
}

void inicializa_Alface(Ingrediente *ing)
{
	*ing = (Ingrediente){4, "Alface", 4.0, 10};
}

void inicializa_Tomate(Ingrediente *ing)
{
	*ing = (Ingrediente){5, "Tomate", 3.0, 10};
}

void inicializa_Bacon(Ingrediente *ing)
{
	*ing = (Ingrediente){6, "Bacon", 4.0, 10};
}

void inicializa_Picles(Ingrediente *ing)
{
	*ing = (Ingrediente){7, "Picles", 4.0, 10};
}

void inicializa_Cebola(Ingrediente *ing)
{
	*ing = (Ingrediente){8, "Cebola", 3.0, 10};
}

void inicializa_Falafel(Ingrediente *ing)
{
	*ing = (Ingrediente){9, "Falafel", 7.0, 10};
}

void inicializa_MolhoDoPato(Ingrediente *ing)
{
	*ing = (Ingrediente){10, "Molho do Pato", 2.0, 10};
}

void inicializa_OnionRings(Ingrediente *ing)
{
	*ing = (Ingrediente){11, "Onion Rings", 6.0, 10};
}

void inicializa_Maionese(Ingrediente *ing)
{
	*ing = (Ingrediente){12, "Maionese", 3.0, 10};
}

void inicializa_Frango(Ingrediente *ing)
{
	*ing = (Ingrediente){13, "Frango", 4.0, 10};
}
// inicializa a fila
void inicializarEstoque(Estoque *estoque)
{
    // --- TEXTO MODIFICADO ---
		inicializa_Pao(&estoque->ingredientes[0]);
		inicializa_Hamburguer(&estoque->ingredientes[1]);
		inicializa_Queijo(&estoque->ingredientes[2]);
		inicializa_Alface(&estoque->ingredientes[3]);
		inicializa_Tomate(&estoque->ingredientes[4]);
		inicializa_Bacon(&estoque->ingredientes[5]);
		inicializa_Picles(&estoque->ingredientes[6]);
		inicializa_Cebola(&estoque->ingredientes[7]);
		inicializa_Falafel(&estoque->ingredientes[8]);
		inicializa_MolhoDoPato(&estoque->ingredientes[9]);
		inicializa_OnionRings(&estoque->ingredientes[10]);
		inicializa_Maionese(&estoque->ingredientes[11]);
		inicializa_Frango(&estoque->ingredientes[12]);
}

void adicionarIngrediente(Estoque *estoque, int id, int quantidade) {
	int i = id - 1;

	if (i < 0 || i >= MAX_INGREDIENTES) {
        // --- TEXTO MODIFICADO ---
		printf("Erro: ID do ingrediente %d e invalido.\n", id);
		return;
	}

	if (estoque->ingredientes[i].id == id)
	{
		estoque->ingredientes[i].quantidade += quantidade;
	} else {
        // --- TEXTO MODIFICADO ---
		printf("Ingrediente com ID %d nao encontrado no estoque.\n", id);
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
        // --- TEXTO MODIFICADO ---
		printf("Erro: ID do ingrediente %d e invalido.\n", id);
		return;
	}

	if (estoque->ingredientes[i].id == id)
	{
		if (estoque->ingredientes[i].quantidade >= quantidade) {
			estoque->ingredientes[i].quantidade -= quantidade;
		} else {
            // --- TEXTO MODIFICADO ---
			printf("Erro: Quantidade insuficiente do ingrediente %s no estoque.\n", estoque->ingredientes[i].nome);
		}
	} else {
        // --- TEXTO MODIFICADO ---
		printf("Ingrediente com ID %d nao encontrado no estoque.\n", id);
	}
}
