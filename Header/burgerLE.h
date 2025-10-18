#ifndef BURGERDE_H
#define BURGERDE_H

#include "pilhaLE.h"

typedef struct
{
    tp_pilhaLE *ingredientes;
} BurgerLE_Player; //Versão do hambúrguer que o player monta. Apenas possui ingredientes.

typedef struct
{
    int id;
    char nome[30];
    float preco;
    tp_pilhaLE *ingredientes;
} BurgerLE; //Instância literal do hambúrguer no cardápio. Utilizada para contabilizar moedas, nome e id.


BurgerLE_Player *inicializar_BurgerLE_Player();
void adicionarIngredienteLE(BurgerLE_Player **burger, tp_item_pilhaLE e); //Quando o jogador adicionar um ingrediente ao hambúrguer.
void comparaHamburgueresLE(BurgerLE_Player **burgerPlayer, BurgerLE **burgerPedido, int moedas); //Compara o hambúrguer criado pelo jogador com o hambúrguer pedido. Além disso, retorna o número de moedas que o player conseguiu com seu hambúrguer.
void deletaBurgerLE(BurgerLE_Player **burger); //Quando o player deletar seu hambúrguer montado até agora.

void inicializaDuckCheese_LE(BurgerLE **burgerPedido);
void inicializa_Quackteirao_LE(BurgerLE **burgerPedido);
void inicializa_BigPato_LE(BurgerLE **burgerPedido);
void inicializa_ZeroUm_LE(BurgerLE **burgerPedido);
void inicializa_ChickenDuckey_LE(BurgerLE **burgerPedido);
void inicializa_PatoSobreRodas_LE(BurgerLE **burgerPedido);
void inicializa_Recursivo_LE(BurgerLE **burgerPedido);
void inicializa_PatoVerde_LE(BurgerLE **burgerPedido);
void inicializa_PicklesAndMayo_LE(BurgerLE **burgerPedido);  //Funções de inicialização para cada hambúrguer (Quando chega a hora de comparar o criado pelo
                                                             //player com o original.

#endif
