#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../Header/filaLE.h"
#include "../Header/burger.h"

// Inicializa a fila encadeada
void inicializaFilaLEPedidos(FilaLEPedidos *f)
{
	f->inicio = NULL;
	f->fim = NULL;
	f->tamanho = 0;
}

// Retorna 1 se a fila estiver vazia
int filaLEPedidosVazia(FilaLEPedidos *f)
{
	return (f->inicio == NULL);
}

// Retorna 0 sempre, pois a fila encadeada não tem limite prático
int filaLEPedidosCheia(FilaLEPedidos *f)
{
	return 0;
}

// Insere no final da fila
int enfileiraPedido(FilaLEPedidos *f, Pedido elem)
{
	NoPedido *novo = (NoPedido *)malloc(sizeof(NoPedido));
	if (novo == NULL)
		return 0; // erro de alocação

	novo->info = elem;
	novo->prox = NULL;

	if (filaLEPedidosVazia(f))
		f->inicio = novo;
	else
		f->fim->prox = novo;

	f->fim = novo;
	f->tamanho++;
	return 1;
}

// Remove do início da fila
int desenfileiraPedido(FilaLEPedidos *f, Pedido *elem)
{
	if (filaLEPedidosVazia(f))
		return 0;

	NoPedido *removido = f->inicio;
	*elem = removido->info;

	f->inicio = removido->prox;

	if (f->inicio == NULL)
		f->fim = NULL;

	free(removido);
	f->tamanho--;
	return 1;
}

// Imprime os pedidos
void imprimeFilaLEPedidos(FilaLEPedidos *f, Burger *cardapio)
{
	NoPedido *atual = f->inicio;
	printf("\n--- FILA DE PEDIDOS ---\n");

	while (atual != NULL)
	{
		printf("Pedido #%d | Burger ID: %d | Quantidade: %d\n",
					 atual->info.id,
					 atual->info.id_burger,
					 atual->info.quantidade);
		atual = atual->prox;
	}

	if (f->inicio == NULL)
		printf("(Fila vazia)\n");
}

// Gera pedidos aleatórios e insere na fila
void geraPedidos(FilaLEPedidos *f, int num_dia)
{
	srand(time(NULL));

	int qtd_pedidos = rand() % 5 + 3; // entre 3 e 7 pedidos
	for (int i = 0; i < qtd_pedidos; i++)
	{
		Pedido p;
		p.id = num_dia * 100 + i + 1;
		p.id_burger = rand() % 5 + 1;	 // assume 5 tipos no cardápio
		p.quantidade = rand() % 3 + 1; // entre 1 e 3 unidades

		enfileiraPedido(f, p);
	}
}
