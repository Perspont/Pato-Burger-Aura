#ifndef BURGER_H
#define BURGER_H

#include "include/pilha.h"

#define MAX_BURGERS 10

typedef struct hamburguer
{
	int id;
	char nome[30];
	float preco;
	int ingredientes[30];// Array com os IDs dos ingredientes necessários
	tp_pilha ordem; //Para retirada e colocar dos ingredientes. Na gameplay, o player só pode alcançar o ingrediente de baixo se tirar o de cima.

} Burger;

void inicializarHamburgueres(Burger *cardapio);
void exibirCardapio(Burger *cardapio);


#endif
