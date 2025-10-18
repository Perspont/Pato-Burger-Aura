#include <stdio.h>
#include <stdlib.h>
#include "../Header/pilhaLE.h"



tp_pilhaLE *inicializa_pilha_LE(){
   tp_pilhaLE *pilha=(tp_pilhaLE*) malloc(sizeof(tp_pilhaLE));   
   pilha->topo = NULL;   
   return pilha;
}  

tp_no_pilha *alocaPilhaLE() {
	tp_no_pilha* pt;
	pt=(tp_no_pilha*) malloc(sizeof(tp_no_pilha));
	return pt;
}

int pilha_vazia_LE(tp_pilhaLE *pilha){
 if (pilha->topo == NULL) return 1;
 return 0;
}

int push_pilha_LE(tp_pilhaLE *pilha, tp_item_pilhaLE e){
  tp_no_pilha *novo;   
  novo=alocaPilhaLE();
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

int pop_pilha_LE(tp_pilhaLE *pilha, tp_item_pilhaLE *e){
  tp_no_pilha *aux;

  if (pilha_vazia_LE(pilha)) return 0;
  *e=pilha->topo->info;
  aux=pilha->topo;
  pilha->topo=pilha->topo->prox;
  free(aux);
  return 1;   
}        

int top_pilha_LE(tp_pilhaLE *pilha, tp_item_pilhaLE *e){
  tp_no_pilha *aux;

  if (pilha_vazia_LE(pilha)) return 0;
  *e=pilha->topo->info;
  return 1;   
}


tp_pilhaLE *destroi_pilha_LE(tp_pilhaLE *pilha) {
	 tp_no_pilha *atu=pilha->topo, *aux;
     tp_item_pilhaLE e;
	 while (atu != NULL)
	       {
			aux = atu->prox;
            pop(pilha, &e);  
			atu= aux; 
            }
	pilha->topo = NULL;
	free(pilha);
}

void imprime_pilha_LE(tp_pilhaLE *pilha) {

	tp_no_pilha *atu=pilha->topo;

	while (atu != NULL)
	{
		printf("%d ", atu->info);
		atu= atu->prox;
	}

	printf("\n");
}


/*
void imprime_pilha(tp_pilhaLE *pilha) {
     tp_pilhaLE *pilha_aux;
     tp_item_pilhaLE e;
     //cria e inicializa uma pilha auxiliar
	 pilha_aux=inicializa_pilha();
     pilha_aux->topo = pilha->topo; //atribui o topo M de pilha para pilha_aux
     
	 //tp_no_pilha *atu=pilha->topo;
     
	while (!pilha_vazia(pilha))
	{
        pop(pilha,&e);
        printf("%d ", e);
        push(pilha_aux, e);
	}

     pilha->topo = pilha_aux->topo; //atribui o topo de pilha_aux para pilha. Transfere a pilha_aux pra pilha, recuperando a pilha original.
     printf("\n");
     free(pilha_aux);
}
*/
