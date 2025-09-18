#ifndef ESTOQUE_H
#define ESTOQUE_H

#define MAX_INGREDIENTES 12

typedef struct
{
	int id;
	char nome[30];
	float preco;
	int quantidade;
} Ingrediente;

typedef struct
{
	Ingrediente ingredientes[MAX_INGREDIENTES];
} Estoque;

void inicializarEstoque(Estoque *estoque);
void adicionarIngrediente(Estoque *estoque, int id, int quantidade);
void exibirEstoque(Estoque *estoque);
void removerIngrediente(Estoque *estoque, int id, int quantidade);

#endif
