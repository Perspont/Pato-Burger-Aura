#include <stdio.h>
#include <stdlib.h>
#include "pilhaLE.h"



tp_pilha *inicializa_pilha (){
   tp_pilha *pilha=(tp_pilha*) malloc(sizeof(tp_pilha));   
   pilha->topo = NULL;   
   return pilha;
}  

tp_no *aloca() {
	tp_no* pt;
	pt=(tp_no*) malloc(sizeof(tp_no));
	return pt;
}

int pilha_vazia (tp_pilha *pilha){   
 if (pilha->topo == NULL) return 1;
 return 0;
}

int push (tp_pilha *pilha, tp_item e){   
  tp_no *novo;   
  novo=aloca();
  if (!novo) return 0;

  novo->info = e;  
  if ( (pilha->topo == NULL) ) { //Se for o primeiro elemento da lista
     novo->prox = NULL;   
     //pilha->topo = novo;
     }  
  else {
     novo->prox = pilha->topo;   
     //pilha->topo = novo; 
     }  
  pilha->topo = novo; 
  return 1;   
}        

int pop (tp_pilha *pilha, tp_item *e){   
  tp_no *aux;

  if (pilha_vazia(pilha)) return 0;
  *e=pilha->topo->info;
  aux=pilha->topo;
  pilha->topo=pilha->topo->prox;
  free(aux);
  return 1;   
}        

int top (tp_pilha *pilha, tp_item *e){   
  tp_no *aux;

  if (pilha_vazia(pilha)) return 0;
  *e=pilha->topo->info;
  return 1;   
}        


void imprime_pilha(tp_pilha *pilha) {
    
	 tp_no *atu=pilha->topo;
     
	 while (atu != NULL)
	       {
            printf("%d ", atu->info);  
			atu= atu->prox; 
            }

     printf("\n");
}


void destroi_pilha(tp_pilha *pilha) {    
	 tp_no *atu=pilha->topo, *aux;
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

void imprime_pilha(tp_pilha *pilha) {
     tp_pilha *pilha_aux;
     tp_item e;
     //cria e inicializa uma pilha auxiliar
	 pilha_aux=inicializa_pilha();
     pilha_aux->topo = pilha->topo; //atribui o topo M de pilha para pilha_aux
     
	 tp_no *atu=pilha->topo;
     
	 while (!pilha_vazia(pilha))
	       {
            pop(pilha,&e);
            printf("%d ", e);
            push(pilha_aux, e);           
            }

     pilha->topo = pilha_aux->topo; //atribui o topo de pilha_aux para pilha
     printf("\n");
     free(pilha_aux);
}

