/* Conteúdo de PilhaLE.c (CORRIGIDO) */

#include <stdio.h>
#include <stdlib.h>
#include "PilhaLE.h" 



tp_pilha *inicializa_pilha (){
   tp_pilha *pilha=(tp_pilha*) malloc(sizeof(tp_pilha));   
   pilha->topo = NULL;   
   return pilha;
}  

/* RENOMEADO AQUI e o TIPO DE RETORNO ATUALIZADO */
tp_pilha_no *aloca_pilha() {
	tp_pilha_no* pt; /* TIPO ATUALIZADO AQUI */
	pt=(tp_pilha_no*) malloc(sizeof(tp_pilha_no)); /* TIPO ATUALIZADO AQUI */
	return pt;
}

int pilha_vazia (tp_pilha *pilha){   
 if (pilha->topo == NULL) return 1;
 return 0;
}

int push (tp_pilha *pilha, tp_item e){   
  tp_pilha_no *novo;   /* TIPO ATUALIZADO AQUI */
  novo = aloca_pilha(); /* FUNÇÃO ATUALIZADA AQUI */
  if (!novo) return 0;

  novo->info = e;  
  if ( (pilha->topo == NULL) ) { 
     novo->prox = NULL;   
     }  
  else {
     novo->prox = pilha->topo;   
     }  
  pilha->topo = novo; 
  return 1;   
}        

int pop (tp_pilha *pilha, tp_item *e){   
  tp_pilha_no *aux; /* TIPO ATUALIZADO AQUI */

  if (pilha_vazia(pilha)) return 0;
  *e=pilha->topo->info;
  aux=pilha->topo;
  pilha->topo=pilha->topo->prox;
  free(aux);
  return 1;   
}        

int top (tp_pilha *pilha, tp_item *e){   
  /* tp_pilha_no *aux;  TIPO ATUALIZADO (mas aux não é usado) */
  if (pilha_vazia(pilha)) return 0;
  *e=pilha->topo->info;
  return 1;   
}        


void imprime_pilha(tp_pilha *pilha) {
	 tp_pilha_no *atu=pilha->topo; /* TIPO ATUALIZADO AQUI */
     
	 while (atu != NULL)
	       {
            printf("%d ", atu->info);  
			atu= atu->prox; 
            }
     printf("\n");
}


void destroi_pilha(tp_pilha *pilha) {    
	 tp_pilha_no *atu=pilha->topo, *aux; /* TIPO ATUALIZADO AQUI */
     tp_item e;
	 while (atu != NULL)
	       {
			aux = atu->prox;
            pop(pilha, &e);  
			atu= aux; 
            }
	pilha->topo = NULL;
	free(pilha);
}