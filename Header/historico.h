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

#endif