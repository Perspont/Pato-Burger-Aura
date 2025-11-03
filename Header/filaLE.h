#ifndef FILALE_H
#define FILALE_H

#include "burger.h"

#define MAX_PEDIDOS 20

typedef struct pedido
{
	int id;
	int id_burger;
	int quantidade;
} Pedido_FilaLE;

typedef struct noPedido
{
	Pedido info;
	struct noPedido *prox;
} NoPedido_FilaLE;

typedef struct filaLEPedidos
{
	NoPedido_FilaLE *inicio;
	NoPedido_FilaLE *fim;
	int tamanho;
} FilaLEPedidos;

// protótipos
void inicializaFilaLEPedidos(FilaLEPedidos *f);
int filaLEPedidosVazia(FilaLEPedidos *f);
int filaLEPedidosCheia(FilaLEPedidos *f);
int enfileiraPedido_FilaLE(FilaLEPedidos *f, Pedido elem);
int desenfileiraPedido_FilaLE(FilaLEPedidos *f, Pedido *elem);
void imprimeFilaLEPedidos(FilaLEPedidos *f, Burger *cardapio);
void geraPedidos_FilaLE(FilaLEPedidos *f, int num_dia);

#endif
