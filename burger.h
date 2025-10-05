#ifndef BURGER_H
#define BURGER_H

#include "pilha.h"

#define MAX_BURGERS 10

typedef struct
{
	int id;
	char nome[30];
	float preco;
	tp_pilha ingredientes;
} Burger;

void inicializarCardapio(Burger *cardapio);
void exibirCardapio(Burger *cardapio);


#endif
