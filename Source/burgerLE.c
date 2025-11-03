#include <stdio.h>
#include <math.h>
#include "../Header/burgerLE.h"


void inicializar_BurgerLE_Player(BurgerLE_Player *burger) { //Ingredientes -> Possui a pilha.

    burger->ingredientes = inicializa_pilha_LE();

}

void adicionarIngredienteLE(BurgerLE_Player *burger, tp_item_pilhaLE e) {

    if (burger == NULL) {

        printf("Erro: Tentativa de adicionar ingrediente a um hamburger nulo.\n");
        return;

    }

    if (burger->ingredientes == NULL) {

        printf("Erro: Pilha de ingredientes do hamburger não inicializada.\n");
        return;

    }

    push_pilha_LE(burger->ingredientes, e);

}

//Função para deletar burguer
void deletaBurgerLE(BurgerLE_Player *burger)
{
    if (burger && burger->ingredientes)
    {
        tp_item_pilhaLE e;
        int qtd = burger->ingredientes->tamanho;
        for (int i = 0; i < qtd; i++)
        {
            pop_pilha_LE(burger->ingredientes, &e);
        }
    }
}

float comparaHamburgueresLE(BurgerLE_Player *burgerPlayer, BurgerLE *burgerPedido) {

    if (burgerPlayer == NULL || burgerPedido == NULL ||
        burgerPlayer->ingredientes == NULL || burgerPedido->ingredientes == NULL) { //Se quaisquer um destes for um ponteiro nulo.
        return 0.0f;
    }

    int totalIngredientesPedido = burgerPedido->ingredientes->tamanho; //Total de ingredientes no pedido.

    if (totalIngredientesPedido == 0) {
        return burgerPedido->preco;
    } //Retorna 0 caso o hambúrguer pedido tenha 0 ingredientes (Isto deve contabilizar todos os casos de erro).

    float moedas = burgerPedido->preco; //Número total de moedas ganhas.
    float multiplicadorDeErro = moedas / totalIngredientesPedido; //Valor subtraído das moedas para cada erro.

    //Pesos para cada tipo de erro.
    float penalidadePosicaoErrada = 0.5f;    //Caso o ingrediente esteja certo, mas na posição errada (50% do valor do erro normal).
    float penalidadeIngredienteErrado = 1.0f;  // Caso haja ingrediente errado (Não pertencente ao pedido) (100% do valor do erro normal).
    float penalidadeIngredienteExtra = 0.8f;  //Caso haja ingrediente extra (Incluso no pedido, mas mais do que o necessário) (80% do valor do erro normal).
    float penalidadeIngredienteFaltante = 0.9f; //Caso haja ingrediente faltando (90% do valor do erro normal).

    float totalPenalidade = 0.0f;

    //Estas listas guardam ingredientes usados (Ingredientes iguais e em posição correta entre os 2 hambúrgueres).
    tp_pilhaLE *ingredientesUsadosPlayer = inicializa_pilha_LE();
    tp_pilhaLE *ingredientesUsadosPedido = inicializa_pilha_LE();

    //Nós para percorrer as pilhas.
    tp_no_pilha *noPlayer = burgerPlayer->ingredientes->topo;
    tp_no_pilha *noPedido = burgerPedido->ingredientes->topo;

    //Inicializa as listas de usados com 0 (não usado)

    for (int i = 0; i < burgerPlayer->ingredientes->tamanho; i++) { //Enquanto [i] for menor que o tamanho do hambúrguer do jogador.
        push_pilha_LE(ingredientesUsadosPlayer, 0); // Popula pilha com 0.
        noPlayer = noPlayer->prox;
    }

    for (int i = 0; i < burgerPedido->ingredientes->tamanho; i++) { //Enquanto [i] for menor que o tamanho do hambúrguer pedido.
        push_pilha_LE(ingredientesUsadosPedido, 0); // Popula pilha com 0.
        noPedido = noPedido->prox;
    }

    //Primeira passada (Marca acertos exatos).
    //Esta sessão vai contabilizar cada ingrediente posicionado corretamente como 1 nas duas listas de Usados.

    noPlayer = burgerPlayer->ingredientes->topo;
    noPedido = burgerPedido->ingredientes->topo;
    tp_no_pilha *noUsadoPlayer = ingredientesUsadosPlayer->topo;
    tp_no_pilha *noUsadoPedido = ingredientesUsadosPedido->topo;


    //Se tamanho do hambúrguer do player for menor que o pedido, minTamanho = burgerPlayer. Senão, minTamanho = burgerPedido.

    int minTamanho = (burgerPlayer->ingredientes->tamanho < burgerPedido->ingredientes->tamanho)
                   ? burgerPlayer->ingredientes->tamanho : burgerPedido->ingredientes->tamanho; //O tamanho percorrido inicialmente é o mínimo possível entre os dois.
                                                                                                //(Para que não ocorra de uma lista acabar antes da outra na comparação inicial).
    for (int i = 0; i < minTamanho; i++) {
        if (noPlayer->info == noPedido->info) {
            noUsadoPlayer->info = 1; // Marca como usado (acerto exato)
            noUsadoPedido->info = 1;
        }
        noPlayer = noPlayer->prox;
        noPedido = noPedido->prox;
        noUsadoPlayer = noUsadoPlayer->prox;
        noUsadoPedido = noUsadoPedido->prox;
    }

    //Segunda passada (Conta ingredientes corretos em posições erradas (penalidade menor)).

    noPlayer = burgerPlayer->ingredientes->topo;
    noUsadoPlayer = ingredientesUsadosPlayer->topo;

    for (int i = 0; i < burgerPlayer->ingredientes->tamanho; i++) {
        if (noUsadoPlayer->info == 0) { // Não apareceu na lista de usados na posição correta.

            noPedido = burgerPedido->ingredientes->topo;
            noUsadoPedido = ingredientesUsadosPedido->topo;

            for (int j = 0; j < burgerPedido->ingredientes->tamanho; j++) {
                if (noUsadoPedido->info == 0 && noPlayer->info == noPedido->info) { //Se o nó no pedido não estiver preenchido corretamente, e este ponto tiver o ingrediente faltante anteriormente.

                    totalPenalidade += multiplicadorDeErro * penalidadePosicaoErrada; //Penalizar jogador.
                    noUsadoPlayer->info = 1;
                    noUsadoPedido->info = 1; //Marcar como usado (Ou seja, o ingrediente está presente, e não será mais verificado nas próximas rodadas do loop principal).
                    break;

                }

                noPedido = noPedido->prox;
                noUsadoPedido = noUsadoPedido->prox;
            }
        }
        noPlayer = noPlayer->prox;
        noUsadoPlayer = noUsadoPlayer->prox;
    }

    //Terceira passada (Conta ingredientes a mais e faltantes).

    noPlayer = burgerPlayer->ingredientes->topo;
    noPedido = burgerPedido->ingredientes->topo;
    noUsadoPlayer = ingredientesUsadosPlayer->topo;
    noUsadoPedido = ingredientesUsadosPedido->topo;

    //Ingredientes extras (A mais), na receita e não.
    for (int i = 0; i < burgerPlayer->ingredientes->tamanho; i++) {
        if (noUsadoPlayer->info == 0) { // Ingrediente no burger do player não constado no pedido.
            int ingredienteExtraNaReceita = 0;

            // Verifica se este ingrediente existe na receita original.
            noPedido = burgerPedido->ingredientes->topo;
            while (noPedido != NULL) {
                if (noPlayer->info == noPedido->info) {
                    ingredienteExtraNaReceita = 1;
                    break;
                }
                noPedido = noPedido->prox;
            }

            if (ingredienteExtraNaReceita) {
                totalPenalidade += multiplicadorDeErro * penalidadeIngredienteExtra;
            } else {
                totalPenalidade += multiplicadorDeErro * penalidadeIngredienteErrado;
            }
        }
        noPlayer = noPlayer->prox;
        noUsadoPlayer = noUsadoPlayer->prox;
    }

    //Ingredientes faltantes.
    for (int i = 0; i < burgerPedido->ingredientes->tamanho; i++) {
        if (noUsadoPedido->info == 0) {
            totalPenalidade += multiplicadorDeErro * penalidadeIngredienteFaltante;
        } //Para cada ingrediente faltando -> penalidade.

        noPedido = noPedido->prox;
        noUsadoPedido = noUsadoPedido->prox;
    }

    //Libera as pilhas temporárias.
    destroi_pilha_LE(&ingredientesUsadosPlayer);
    destroi_pilha_LE(&ingredientesUsadosPedido);

    destroi_pilha_LE(&(burgerPedido->ingredientes));
    deletaBurgerLE(burgerPlayer); //Deleta apenas a pilha de ingredientes.


    float moedasFinais = moedas - totalPenalidade;
    moedasFinais = (moedasFinais < 0) ? 0 : moedasFinais;

    return roundf(moedasFinais * 100) / 100.f;
}

//Função de inicializa o burguerLE
void inicializa_BitAndBacon_LE(BurgerLE *burgerPedido) //{1, 2, 3, 6, 1};
{
    burgerPedido->preco = 16;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 2);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 6);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_DuckCheese_LE(BurgerLE *burgerPedido) //{1, 3, 5, 4, 1}};
{
    burgerPedido->preco = 16;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 5);
    push_pilha_LE(burgerPedido->ingredientes, 4);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_Quackteirao_LE(BurgerLE *burgerPedido)
{
    burgerPedido->preco = 16;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 2);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 4);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_BigPato_LE(BurgerLE *burgerPedido)
{
    burgerPedido->preco = 27;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 4);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 2);
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 4);
    push_pilha_LE(burgerPedido->ingredientes, 2);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_ZeroUm_LE(BurgerLE *burgerPedido)
{
    burgerPedido->preco = 13;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 2);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_ChickenDuckey_LE(BurgerLE *burgerPedido)
{
    burgerPedido->preco = 21;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 4);
    push_pilha_LE(burgerPedido->ingredientes, 13);
    push_pilha_LE(burgerPedido->ingredientes, 7);
    push_pilha_LE(burgerPedido->ingredientes, 10);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_PatoSobreRodas_LE(BurgerLE *burgerPedido)
{
    burgerPedido->preco = 24;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 2);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 4);
    push_pilha_LE(burgerPedido->ingredientes, 5);
    push_pilha_LE(burgerPedido->ingredientes, 6);
    push_pilha_LE(burgerPedido->ingredientes, 11);
    push_pilha_LE(burgerPedido->ingredientes, 10);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_Recursivo_LE(BurgerLE *burgerPedido) {

    burgerPedido->preco = 35;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    int ids[] = {1, 10, 2, 3, 8, 5, 8, 4, 2, 3, 2, 3, 6, 10, 1};
    for (int i = 0; i < 15; i++){
        push_pilha_LE(burgerPedido->ingredientes, ids[i]);
    }
}

void inicializa_PatoVerde_LE(BurgerLE *burgerPedido)
{
    burgerPedido->preco = 21;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 9);
    push_pilha_LE(burgerPedido->ingredientes, 3);
    push_pilha_LE(burgerPedido->ingredientes, 8);
    push_pilha_LE(burgerPedido->ingredientes, 7);
    push_pilha_LE(burgerPedido->ingredientes, 10);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}

void inicializa_PicklesAndMayo_LE(BurgerLE *burgerPedido)
{
    burgerPedido->preco = 25;

    burgerPedido->ingredientes = inicializa_pilha_LE();
    push_pilha_LE(burgerPedido->ingredientes, 1);
    push_pilha_LE(burgerPedido->ingredientes, 10);
    push_pilha_LE(burgerPedido->ingredientes, 7);
    push_pilha_LE(burgerPedido->ingredientes, 6);
    push_pilha_LE(burgerPedido->ingredientes, 10);
    push_pilha_LE(burgerPedido->ingredientes, 7);
    push_pilha_LE(burgerPedido->ingredientes, 1);
}