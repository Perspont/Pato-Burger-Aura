#include <stdio.h>
#include <stdlib.h>

typedef int tp_item; 

typedef struct tp_no_aux {   
  tp_item info;   
  struct tp_no_aux *prox;   
} tp_no; 


typedef struct {   
  tp_no *topo;   
} tp_pilha;


tp_pilha *inicializa_pilha ();
int pilha_vazia (tp_pilha *pilha);
tp_no *aloca();
int push (tp_pilha *pilha, tp_item e);
int pop (tp_pilha *pilha, tp_item *e);
int top (tp_pilha *pilha, tp_item *e);
tp_pilha *destroi_pilha(tp_pilha *pilha);
void imprime_pilha(tp_pilha *pilha);



