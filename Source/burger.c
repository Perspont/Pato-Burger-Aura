#include <stdio.h>
#include "../Header/burger.h"

//Funções para inicializar cada hambúrguer.

//Caso necessário, em futuras atualizações, utilizar estas funções para definir tudo dos hamburgueres ao invés de um vetor
//cardapio para preencher preço, nome, etc.

//Todos os hambúrgueres são montados de baixo pra cima (Como uma pilha).

tp_pilha inicializa_BitAndBacon() { //{1, 2, 3, 6, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 2);
	push(&p, 3);
	push(&p, 6);
	push(&p, 1);

	return p;
}

tp_pilha inicializa_DuckCheese() { //{1, 3, 5, 4, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 3);
	push(&p, 5);
	push(&p, 4);
	push(&p, 1);

	return p;

}

tp_pilha inicializa_Quackteirao() { //{1, 3, 2, 3, 4, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 3);
	push(&p, 2);
	push(&p, 3);
	push(&p, 4);
	push(&p, 1);

	return p;

}

tp_pilha inicializa_BigPato() { //{1, 4, 3, 2, 1, 4, 2, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 4);
	push(&p, 3);
	push(&p, 2);
	push(&p, 1);
	push(&p, 4);
	push(&p, 2);
	push(&p, 1);

	return p;

}

tp_pilha inicializa_ZeroUm() { //{1, 2, 3, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 2);
	push(&p, 3);
	push(&p, 1);

	return p;

}

tp_pilha inicializa_ChickenDuckey() { //{1, 4, 13, 7, 10, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 4);
	push(&p, 13);
	push(&p, 7);
	push(&p, 10);
	push(&p, 1);

	return p;

}

tp_pilha inicializa_PatoSobreRodas() { //{1, 2, 3, 4, 5, 6, 11, 10, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 2);
	push(&p, 3);
	push(&p, 4);
	push(&p, 5);
	push(&p, 6);
	push(&p, 11);
	push(&p, 10);
	push(&p, 1);

	return p;

}

tp_pilha inicializa_Recursivo() { //{1, 10, 2, 3, 8, 5, 8, 4, 2, 3, 2, 3, 6, 10, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 10);
	push(&p, 2);
	push(&p, 3);
	push(&p, 8);
	push(&p, 5);
	push(&p, 8);
	push(&p, 4);
	push(&p, 2);
	push(&p, 3);
	push(&p, 2);
	push(&p, 3);
	push(&p, 6);
	push(&p, 10);
	push(&p, 1);

	return p;

}

tp_pilha inicializa_PatoVerde() { //{1, 9, 3, 8, 7, 10, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 9);
	push(&p, 3);
	push(&p, 8);
	push(&p, 7);
	push(&p, 10);
	push(&p, 1);

	return p;
}

tp_pilha inicializa_PicklesAndMayo() { //{1, 10, 7, 6, 10, 7, 1}

	tp_pilha p;

	inicializa_pilha(&p);

	push(&p, 1);
	push(&p, 10);
	push(&p, 7);
	push(&p, 6);
	push(&p, 10);
	push(&p, 7);
	push(&p, 1);

	return p;

}


// inicializa a fila
void inicializarCardapio(Burger *cardapio)
{
	cardapio[0] = (Burger){1, "Bit and Bacon", 16.0}; //{1, 2, 3, 8, 1}};
	cardapio[1] = (Burger){2, "Duck Cheese", 16.0}; //{1, 3, 5, 4, 1}};
	cardapio[2] = (Burger){3, "Quackteirao", 16.0}; //{1, 4, 3, 2, 3, 1}};
	cardapio[3] = (Burger){4, "Big Pato", 27.0}; //{1, 2, 4, 1, 2, 3, 4, 1}};
	cardapio[4] = (Burger){5, "Zero e Um", 13.0}; //{1, 3, 2, 1}};
	cardapio[5] = (Burger){6, "Chicken Duckey", 21.0}; //{1, 10, 7, 13, 4, 1}};
	cardapio[6] = (Burger){7, "Pato sobre rodas", 24.0}; //{1, 10, 11, 6, 5, 4, 3, 2, 1}};
	cardapio[7] = (Burger){8, "Recursivo", 35.0}; //{1, 10, 6, 3, 2, 3, 2, 4, 8, 5, 8, 3, 2, 10, 1}};
	cardapio[8] = (Burger){9, "Pato Verde", 21.0}; //{1, 10, 7, 8, 3, 9, 1}};
	cardapio[9] = (Burger){10, "Pickles and MAYO", 25.0}; //{1, 7, 10, 6, 7, 10, 1}};

	//Definir uma pilha à cada instância de Burger.

	cardapio[0].ingredientes = inicializa_BitAndBacon(); //Passar por referência exigiria criar 10 pilhas na função principal,
	cardapio[1].ingredientes = inicializa_DuckCheese();	 //uma pra cada hamburguer. Acho que fica muito poluído. Por isso, faço a
	cardapio[2].ingredientes = inicializa_Quackteirao(); //função retornar a pilha para o index em cardapio do qual pertence o hamburguer.
	cardapio[3].ingredientes = inicializa_BigPato();
	cardapio[4].ingredientes = inicializa_ZeroUm();
	cardapio[5].ingredientes = inicializa_ChickenDuckey();
	cardapio[6].ingredientes = inicializa_PatoSobreRodas();
	cardapio[7].ingredientes = inicializa_Recursivo();
	cardapio[8].ingredientes = inicializa_PatoVerde();
	cardapio[9].ingredientes = inicializa_PicklesAndMayo();

}

//imprimaBurgerPorIngrediente() -> Desempilha cada elemento em outra pilha (para que a ordem dos ingredientes esteja correta).
//                                 Então, imprime cada ingrediente correspondente ao id do item da pilha.
void exibirCardapio(Burger *cardapio)
{
	printf("\n======== Cardapio ========\n\n");
	for (int i = 0; i < MAX_BURGERS; i++)
	{
		printf("%d. %s - R$ %.2f\n", cardapio[i].id, cardapio[i].nome, cardapio[i].preco);
	}
}
