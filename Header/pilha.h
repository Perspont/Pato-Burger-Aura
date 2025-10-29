/*

#ifndef PILHA_H
#define PILHA_H


#define MAX 100
typedef int tp_item;

typedef struct{
    int topo;
    tp_item item[MAX];
}tp_pilha;

void inicializa_pilha(tp_pilha *p);
int pilha_vazia(tp_pilha *p);
int pilha_cheia(tp_pilha *p);
int push(tp_pilha *p, tp_item e);
int pop(tp_pilha *p, tp_item *e);
int top (tp_pilha *p, tp_item *e);
int altura_pilha (tp_pilha *p);
void imprime_pilha (tp_pilha p);
void retira_impar(tp_pilha *p);
int compara_pilha (tp_pilha p, tp_pilha p1);
int empilha_pilha(tp_pilha *p, tp_pilha *p1);

#endif

*/