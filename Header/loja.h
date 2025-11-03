#ifndef LOJA_H
#define LOJA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

/*estruturas de inventario do jogador*/

// no para cada ingrediente
typedef struct IngredienteNode {
    struct IngredienteNode *prox;
    struct IngredienteNode *ant;
} IngredienteNode;

// struct para gerenciar a lista de ingredientes
typedef struct ListaIngrediente {
    IngredienteNode *cabeca;
    IngredienteNode *cauda;
    int quantidade; 
} ListaIngrediente;

// struct do inventario principal do player
typedef struct InventarioJogador {
    ListaIngrediente paes;        // ID 1
    ListaIngrediente carnes;       // ID 2
    ListaIngrediente queijos;      // ID 3
    ListaIngrediente alfaces;     // ID 4
    ListaIngrediente tomates;      // ID 5
    ListaIngrediente bacons;       // ID 6
    ListaIngrediente picles;       // ID 7
    ListaIngrediente cebolas;      // ID 8
    ListaIngrediente falafels;     // ID 9
    ListaIngrediente molhos;       // ID 10 (
    ListaIngrediente onionRings;   // ID 11
    ListaIngrediente maioneses;    // ID 12
    ListaIngrediente frangos;      // ID 13
    float dinheiro;
} InventarioJogador;


/*
 structs da loja
 */

//no para produto da loja
typedef struct ProdutoLojaNode {
    int id;
    char nome[50];
    float precoBase;
    float precoAtual; 
    struct ProdutoLojaNode *prox; 
    struct ProdutoLojaNode *ant; 
} ProdutoLojaNode;

// struct principal da loja
typedef struct Loja {
    ProdutoLojaNode *cabeca;
    ProdutoLojaNode *cauda;
    int numProdutos;
} Loja;


/*
 * ===================================================================
 * PROTÓTIPOS DAS FUNÇÕES PÚBLICAS
 * ===================================================================
 */

// --- Funções do Inventário ---

/**
 * @brief Inicializa o inventário do jogador (listas vazias, dinheiro inicial).
 */
void inicializarInventario_loja(InventarioJogador *inv, float dinheiroInicial);

/**
 * @brief Libera toda a memória alocada para os nós de ingredientes.
 */
void liberarInventario(InventarioJogador *inv);

/**
 * @brief Adiciona um item (1 nó) ao inventário do jogador.
 * Usado pela função de compra.
 */
void adicionarItemInventario(InventarioJogador *inv, int ingredienteID);

/**
 * @brief Remove um item (1 nó) do inventário do jogador.
 * Deve ser usado pela lógica do jogo (ex: 'processCommand').
 * @return 1 se o item foi usado com sucesso, 0 se não havia estoque.
 */
int usarItemInventario(InventarioJogador *inv, int ingredienteID);

/**
 * @brief Retorna a quantidade atual de um ingrediente.
 * Substitui o acesso direto a 'state->pao_count'.
 * @return A quantidade de itens.
 */
int getQuantidadeInventario(InventarioJogador *inv, int ingredienteID);


// --- Funções da Loja ---

/**
 * @brief Inicializa a loja (lista vazia).
 */
void inicializarLoja(Loja *loja);

/**
 * @brief Libera toda a memória alocada para os produtos da loja.
 */
void liberarLoja(Loja *loja);

/**
 * @brief Adiciona todos os produtos à loja em ORDEM ALFABÉTICA.
 * Não usa variáveis globais.
 */
void popularLoja(Loja *loja);

/**
 * @brief Altera os preços dos produtos na loja aleatoriamente (flutuação).
 */
void flutuarPrecos(Loja *loja);

/**
 * @brief Ponto de entrada principal para a interface da loja.
 * Esta é a função que 'gameplayEtapa2.c' deve chamar quando o dia acabar.
 */
void loopPrincipalLoja(Loja *loja, InventarioJogador *inv);


#endif // LOJA_H
