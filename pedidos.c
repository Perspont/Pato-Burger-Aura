#include <stdio.h>
#include "pedidos.h"
#include "burger.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// inicializa a fila(Configura os valores iniciais)
void inicializaFilaPedidos(FilaPedidos *f)
{
	f->frente = 0;
	f->tras = -1;
	f->tamanho = 0;
}

// Verifica e retorna 1 se fila vazia
int filaPedidosVazia(FilaPedidos *f)
{
	return (f->tamanho == 0);
}

// Verifica e retorna 1 se fila cheia
int filaPedidosCheia(FilaPedidos *f)
{
	return (f->tamanho == MAX);
}

// insere no final da fila e retorna 1 se o pedido foi adicionado com sucesso
int enfileiraPedido(FilaPedidos *f, Pedido elem)
{
	if (filaPedidosCheia(f))
		return 0;
	f->tras = (f->tras + 1) % MAX; // avanço circular
	f->pedidos[f->tras] = elem;
	f->tamanho++;
	return 1;
}
// Remove o primeiro pedido da fila e retorna 1 se o pedido foi removido com sucesso
int desenfileiraPedido(FilaPedidos *f, Pedido *elem)
{
	if (filaPedidosVazia(f))
		return 0;
	*elem = f->pedidos[f->frente];
	f->frente = (f->frente + 1) % MAX;
	f->tamanho--;
	return 1;
}
// Exibe todos os pedidos atualmente na fila
void imprimeFilaPedidos(FilaPedidos *f, Burger *cardapio)
{
	if (filaPedidosVazia(f))
	{
		printf("Fila vazia!\n");
		return;
	}
	printf("\n======= Fila de pedidos ======\n\n");
	int i = f->frente;
	// Percorre a fila e imprime o nome do hambúrguer de cada pedido
	for (int count = 0; count < f->tamanho; count++)
	{
		int burg_id = f->pedidos[i].burg_id;
		// Procura o burger no cardápio
		for (int j = 0; j < MAX_BURGERS; j++)
		{
			if (cardapio[j].id == burg_id)
			{
				printf("Pedido %d: %s\n", f->pedidos[i].id, cardapio[j].nome);
				break;
			}
		}
		i = (i + 1) % MAX; // Move para o próximo elemento da fila circular
	}
}
// Gera uma quantidade aleatória de pedidos para o dia
void geraPedidos(FilaPedidos *f, int num_dia)
{
	srand(time(NULL)); // Inicializa o gerador de números aleatórios
	// A quantidade base de pedidos aumenta com o passar dos dias
	int base = 3 + (num_dia - 1); 
	// Gera um número de pedidos com uma variação aleatória
	int qtdPedidos = base + (rand() % 3);

	if (qtdPedidos < 1) {
		qtdPedidos = 1;
	}
	// Cria e enfileira cada novo pedido
	for (int i = 0; i < qtdPedidos; i++)
	{
		int burg_id = (rand() % MAX_BURGERS) + 1; // Escolhe um ID de hambúrguer aleatório
		Pedido novo_pedido = {i + 1, burg_id};
		enfileiraPedido(f, novo_pedido);
	}
}
