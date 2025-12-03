#ifndef HISTORICO_H
#define HISTORICO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Estrutura da arvore binaria para o historico de burgers.
typedef struct NoHistorico {
    int id_burger;
    int quantidade_vendida;
    struct NoHistorico *esq;
    struct NoHistorico *dir;
} NoHistorico;

//Estrutura da AVL de ingredientes.
typedef struct noAVL
{
    char ingrediente[50];
    int quantidade;
    int altura;
    struct noAVL *esq;
    struct noAVL *dir;

} NO_AVL;

void inicializa_arvore(NoHistorico **raiz);
void registrar_venda_arvore(NoHistorico **raiz, int id);
void liberar_arvore(NoHistorico *raiz);

NO_AVL* removerNo(NO_AVL* root, int quantidade, char nome[]);
NO_AVL* inserirOrdenado(NO_AVL *no, int quantidade, char nome[]);
int altura(NO_AVL *n);
int max_arvoreAVL(int a, int b);
NO_AVL* rotacaoDireita(NO_AVL *y);
NO_AVL* rotacaoEsquerda(NO_AVL *x);
int getBalance(NO_AVL *n);
void destruirAVL(NO_AVL *no);
void inicializarArvoreIngredientesDia(NO_AVL **raizIngredientes);
void exibirEstatisticasRec(NO_AVL *no);
void exibirEstatisticasIngredientes(NO_AVL *raizIngredientes);
void telaEstatisticasIngredientes(NO_AVL *raizIngredientes);
void atualizar_quantidade_ingrediente(NO_AVL **raiz, char *nome, int qtdAdicional);


#endif