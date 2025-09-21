#ifndef PEDIDOS_H
#define PEDIDOS_H

#include "burger.h"

#define MAX_PEDIDOS 20
// Estrutura para representar um único pedido
typedef struct
{
	int id;
	int burg_id; // ID do hambúrguer pedido
} Pedido;
// Estrutura da fila de pedidos (implementação circular)
typedef struct
{
	Pedido pedidos[MAX_PEDIDOS];
	int frente;	 // início da fila
	int tras;		// fim da fila 
	int tamanho; 	// Número atual de pedidos na fila
} FilaPedidos;

// Protótipos das funções definidas em pedidos.c
void inicializaFilaPedidos(FilaPedidos *f);
int filaPedidosVazia(FilaPedidos *f);
int filaPedidosCheia(FilaPedidos *f);
int enfileiraPedido(FilaPedidos *f, Pedido elem);
int desenfileiraPedido(FilaPedidos *f, Pedido *elem);
void imprimeFilaPedidos(FilaPedidos *f, Burger *cardapio);
void geraPedidos(FilaPedidos *f, int num_dia);

#endif
