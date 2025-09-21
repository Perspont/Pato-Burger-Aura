#ifndef BURGER_H
#define BURGER_H

#define MAX_BURGERS 10

typedef struct
{
	int id;
	char nome[30];
	float preco;
	int ingredientes[10];
} Burger;

void inicializarCardapio(Burger *cardapio);
void exibirCardapio(Burger *cardapio);


#endif
