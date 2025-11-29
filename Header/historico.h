#ifndef HISTORICO_H
#define HISTORICO_H

#include <stdio.h>
#include <stdlib.h>

// Estrutura da arvore binaria para o historico (Unidade 3)
typedef struct NoHistorico {
    int id_burger;
    int quantidade_vendida;
    struct NoHistorico *esq;
    struct NoHistorico *dir;
} NoHistorico;

void inicializa_arvore(NoHistorico **raiz);
void registrar_venda_arvore(NoHistorico **raiz, int id);
void liberar_arvore(NoHistorico *raiz);

#endif