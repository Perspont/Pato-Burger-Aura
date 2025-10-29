/* Conteúdo de PilhaLE.h (CORRIGIDO) */

#include <stdio.h>
#include <stdlib.h>

typedef int tp_item; 

/* RENOMEADO AQUI */
typedef struct tp_pilha_no_aux {   
  tp_item info;   
  struct tp_pilha_no_aux *prox;   
} tp_pilha_no; /* RENOMEADO AQUI */


typedef struct {   
  tp_pilha_no *topo; /* TIPO ATUALIZADO AQUI */
} tp_pilha;


tp_pilha *inicializa_pilha ();
int pilha_vazia (tp_pilha *pilha);

/* RENOMEADO AQUI e o TIPO DE RETORNO ATUALIZADO */
tp_pilha_no *aloca_pilha(); 

int push (tp_pilha *pilha, tp_item e);
int pop (tp_pilha *pilha, tp_item *e);
int top (tp_pilha *pilha, tp_item *e);
void destroi_pilha(tp_pilha *pilha); 
void imprime_pilha(tp_pilha *pilha);