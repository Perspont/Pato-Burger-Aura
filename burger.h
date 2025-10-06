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
tp_pilha inicializaDuckCheese();
tp_pilha inicializa_Quackteirao();
tp_pilha inicializa_BigPato();
tp_pilha inicializa_ZeroUm();
tp_pilha inicializa_ChickenDuckey();
tp_pilha inicializa_PatoSobreRodas();
tp_pilha inicializa_Recursivo();
tp_pilha inicializa_PatoVerde();
tp_pilha inicializa_PicklesAndMayo();


#endif
