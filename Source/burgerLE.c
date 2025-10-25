#include <stdio.h>
#include <math.h>
#include "../Header/burgerLE.h"


void inicializar_BurgerLE_Player(BurgerLE_Player *burger) { //Ingredientes -> Possui a pilha.

    burger->ingredientes = inicializa_pilha_LE();

}

void adicionarIngredienteLE(BurgerLE_Player *burger, tp_item_pilhaLE e) {

    if (burger->ingredientes == NULL) {
        printf("Hambúrguer não possui memória alocada");
        return;
    }

    push_pilha_LE(burger->ingredientes, e);

}

float comparaHamburgueresLE(BurgerLE_Player *burgerPlayer, BurgerLE *burgerPedido) { //Retorna as moedas. Pega os dois hambúrgueres por referência para que possa deleta-los após a verificação.

    int contadorDeErros = 0; //Conta os erros cometidos no hambúrguer.
    float moedas = 0; //Moedas ganhas pelo player após montagem.
    float multiplicadorDeErro = (burgerPedido->preco/burgerPedido->ingredientes->tamanho); //A perda de moeda por cada erro == preço do hamburguer pedido/tamanho do hamburguer pedido.

    if (burgerPlayer->ingredientes->tamanho != burgerPedido->ingredientes->tamanho) {

        if (burgerPlayer->ingredientes->tamanho > burgerPedido->ingredientes->tamanho) {

            while (burgerPedido->ingredientes->tamanho != 0) {

            }

        } //Dar while loop até que tamanho de burgerPedido seja 0, então contabilizar o resto de burgerPlayer como erros cometidos.
        else {

            while (burgerPlayer->ingredientes->tamanho != 0) {

            }

        } //Dar while loop até que tamanho de burgerPlayer seja 0, então contabilizar o resto de burgerPlayer como erros cometidos.

    }


    while (burgerPlayer->ingredientes->topo != NULL) { //Comparar os hambúrgueres (Se eles forem iguais).

    }

    return roundf((moedas-(multiplicadorDeErro*contadorDeErros)*100)/100.f);

}