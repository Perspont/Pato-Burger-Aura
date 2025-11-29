#include "../Header/historico.h"

void inicializa_arvore(NoHistorico **raiz) {
    *raiz = NULL;
}

// Funcao recursiva para inserir ou atualizar vendas na arvore
void registrar_venda_arvore(NoHistorico **raiz, int id) {
    if (*raiz == NULL) {
        NoHistorico *novo = (NoHistorico*) malloc(sizeof(NoHistorico));
        novo->id_burger = id;
        novo->quantidade_vendida = 1;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
    } else {
        if (id < (*raiz)->id_burger) {
            registrar_venda_arvore(&(*raiz)->esq, id);
        } else if (id > (*raiz)->id_burger) {
            registrar_venda_arvore(&(*raiz)->dir, id);
        } else {
            // Se o ID ja existe, apenas incrementamos a quantidade
            (*raiz)->quantidade_vendida++;
        }
    }
}

void liberar_arvore(NoHistorico *raiz) {
    if (raiz != NULL) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        free(raiz);
    }
}