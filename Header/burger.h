/* Conteúdo de burger.h (ADAPTADO PARA PILHA DINÂMICA) */

#ifndef BURGER_H
#define BURGER_H

/* 1. Inclui a Pilha Dinâmica (LE) */
#include "PilhaLE.h" 

#define MAX_BURGERS 10

typedef struct
{
	int id;
	char nome[30];
	float preco;
    
    /* 2. 'ingredientes' agora é um PONTEIRO para a pilha */
	tp_pilha *ingredientes; 
} Burger;

void inicializarCardapio(Burger *cardapio);
void exibirCardapio(Burger *cardapio);

/* 3. Todas as funções agora retornam um PONTEIRO para a pilha */
tp_pilha *inicializa_BitAndBacon();
tp_pilha *inicializa_DuckCheese();
tp_pilha *inicializa_Quackteirao();
tp_pilha *inicializa_BigPato();
tp_pilha *inicializa_ZeroUm();
tp_pilha *inicializa_ChickenDuckey();
tp_pilha *inicializa_PatoSobreRodas();
tp_pilha *inicializa_Recursivo();
tp_pilha *inicializa_PatoVerde();
tp_pilha *inicializa_PicklesAndMayo();


#endif