#include <stdio.h>
#include <stdlib.h>

typedef int tp_item_pilhaLE; 

typedef struct tp_no_aux {   
  tp_item_pilhaLE info;   
  struct tp_no_aux *prox;   
} tp_no_pilha;


typedef struct {
  int tamanho;
  tp_no_pilha *topo;
} tp_pilhaLE;


tp_pilhaLE *inicializa_pilha_LE();
tp_no_pilha *alocaPilhaLE();
int pilha_vazia_LE(tp_pilhaLE *pilha);
int push_pilha_LE(tp_pilhaLE *pilha, tp_item_pilhaLE e);
int pop_pilha_LE(tp_pilhaLE *pilha, tp_item_pilhaLE *e);
int top_pilha_LE(tp_pilhaLE *pilha, tp_item_pilhaLE *e);
void destroi_pilha_LE(tp_pilhaLE **pilha);
void imprime_pilha_LE(tp_pilhaLE *pilha);



