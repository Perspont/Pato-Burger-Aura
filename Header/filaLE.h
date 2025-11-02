#ifndef FILALE_H
#define FILALE_H

#include "burger.h"

#define MAX_PEDIDOS 20

typedef struct pedido
{
	int id;
	int id_burger;
	int quantidade;
} Pedido;

typedef struct noPedido
{
	Pedido info;
	struct noPedido *prox;
} NoPedido;

typedef struct filaLEPedidos
{
	NoPedido *inicio;
	NoPedido *fim;
	int tamanho;
} FilaLEPedidos;

// protótipos
void inicializaFilaLEPedidos(FilaLEPedidos *f);
int filaLEPedidosVazia(FilaLEPedidos *f);
int filaLEPedidosCheia(FilaLEPedidos *f);
int enfileiraPedido(FilaLEPedidos *f, Pedido elem);
int desenfileiraPedido(FilaLEPedidos *f, Pedido *elem);
void imprimeFilaLEPedidos(FilaLEPedidos *f, Burger *cardapio);
void geraPedidos(FilaLEPedidos *f, int num_dia);

#endif
