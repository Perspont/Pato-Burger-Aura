#ifndef PEDIDOS_H
#define PEDIDOS_H

#include "burger.h"

#define MAX 20

typedef struct
{
	int id;
	int burg_id;
} Pedido;

typedef struct
{
	Pedido pedidos[MAX];
	int frente;	 
	int tras;		 
	int tamanho;
} FilaPedidos;

// protótipos
void inicializaFilaPedidos(FilaPedidos *f);
int filaPedidosVazia(FilaPedidos *f);
int filaPedidosCheia(FilaPedidos *f);
int enfileiraPedido(FilaPedidos *f, Pedido elem);
int desenfileiraPedido(FilaPedidos *f, Pedido *elem);
void imprimeFilaPedidos(FilaPedidos *f, Burger *cardapio);
void geraPedidos(FilaPedidos *f, int num_dia);

#endif