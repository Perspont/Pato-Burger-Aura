#include "../Header/loja.h" // Inclui o novo loja.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h> // Para system("cls") e Sleep
#else
#include <unistd.h> // Para system("clear") e sleep
#define Sleep(ms) usleep(ms * 1000)
#endif


/*
 * ===================================================================
 * FUNÇÕES AUXILIARES INTERNAS (static)
 * ===================================================================
 */

/**
 * @brief Limpa o buffer de entrada (stdin) para scanf.
 */
static void limparBufferInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Limpa o terminal de forma cross-platform.
 */
static void limparTela() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

/**
 * @brief Libera todos os nós de uma lista de ingrediente específica.
 */
static void _liberarListaIngrediente(ListaIngrediente *lista) {
    IngredienteNode *atual = lista->cabeca;
    IngredienteNode *temp;
    while (atual != NULL) {
        temp = atual;
        atual = atual->prox;
        free(temp);
    }
    lista->cabeca = NULL;
    lista->cauda = NULL;
    lista->quantidade = 0;
}

/**
 * @brief Adiciona um nó (uma unidade) à lista de ingredientes do jogador.
 * (Esta é a lógica do "novo nó à lista de queijo")
 */
static void _adicionarNoIngrediente(ListaIngrediente *lista) {
    IngredienteNode *novoNo = (IngredienteNode*) malloc(sizeof(IngredienteNode));
    if (novoNo == NULL) {
        perror("Erro ao alocar memoria para ingrediente");
        return;
    }
    novoNo->prox = NULL;
    novoNo->ant = lista->cauda; // Aponta para a antiga cauda

    if (lista->cabeca == NULL) { // Se a lista está vazia
        lista->cabeca = novoNo;
    } else { // Se a lista já tem itens
        lista->cauda->prox = novoNo; // A antiga cauda aponta para o novo nó
    }
    lista->cauda = novoNo; // O novo nó é a nova cauda
    lista->quantidade++;
}

/**
 * @brief Remove um nó (uma unidade) da lista de ingredientes.
 * Remove da cauda (LIFO/FIFO não importa, cauda é eficiente).
 * @return 1 em sucesso, 0 se a lista estava vazia.
 */
static int _removerNoIngrediente(ListaIngrediente *lista) {
    if (lista->quantidade == 0 || lista->cauda == NULL) {
        return 0; // Falha, sem estoque
    }

    IngredienteNode *noRemovido = lista->cauda;

    if (lista->cabeca == lista->cauda) { // Se for o último item
        lista->cabeca = NULL;
        lista->cauda = NULL;
    } else { // Se houver mais itens
        lista->cauda = noRemovido->ant; // A nova cauda é o item anterior
        lista->cauda->prox = NULL; // A nova cauda não aponta para frente
    }

    free(noRemovido);
    lista->quantidade--;
    return 1; // Sucesso
}


/**
 * @brief Retorna um ponteiro para a lista de ingredientes correta no inventário
 * com base no ID do produto.
 * @return Ponteiro para a ListaIngrediente, ou NULL se o ID for inválido.
 */
static ListaIngrediente* getListaPorID(InventarioJogador *inv, int id) {
    switch (id) {
        case 1:  return &inv->paes;
        case 2:  return &inv->carnes;
        case 3:  return &inv->queijos;
        case 4:  return &inv->alfaces;
        case 5:  return &inv->tomates;
        case 6:  return &inv->bacons;
        case 7:  return &inv->picles;
        case 8:  return &inv->cebolas;
        case 9:  return &inv->falafels;
        case 10: return &inv->molhos;
        case 11: return &inv->onionRings;
        case 12: return &inv->maioneses;
        case 13: return &inv->frangos;
        default: return NULL;
    }
}

/**
 * @brief Adiciona um produto à loja.
 * A inserção é feita em ORDEM ALFABÉTICA (por nome).
 */
static void _inserirProdutoOrdenado(Loja *loja, int id, const char *nome, double preco) {
    ProdutoLojaNode *novoProduto = (ProdutoLojaNode*) malloc(sizeof(ProdutoLojaNode));
    if (novoProduto == NULL) {
        perror("Erro ao alocar memoria para produto da loja");
        return;
    }
    strcpy(novoProduto->nome, nome);
    novoProduto->id = id;
    novoProduto->precoBase = preco;
    novoProduto->precoAtual = preco;
    novoProduto->prox = NULL;
    novoProduto->ant = NULL;

    // Caso 1: Lista vazia
    if (loja->cabeca == NULL) {
        loja->cabeca = novoProduto;
        loja->cauda = novoProduto;
        loja->numProdutos++;
        return;
    }

    // Caso 2: Inserir antes da cabeça (novo item é o primeiro alfabeticamente)
    if (strcmp(nome, loja->cabeca->nome) < 0) {
        novoProduto->prox = loja->cabeca;
        loja->cabeca->ant = novoProduto;
        loja->cabeca = novoProduto;
        loja->numProdutos++;
        return;
    }

    // Caso 3: Inserir no meio ou no fim
    ProdutoLojaNode *atual = loja->cabeca;
    // Procura o local correto para inserção
    // (Avança enquanto 'atual->prox' existir E o 'nome' for maior que 'atual->prox->nome')
    while (atual->prox != NULL && strcmp(nome, atual->prox->nome) > 0) {
        atual = atual->prox;
    }

    // 'atual' é o nó ANTES do ponto de inserção
    novoProduto->prox = atual->prox;
    novoProduto->ant = atual;

    if (atual->prox != NULL) { // Inserção no meio
        atual->prox->ant = novoProduto;
    } else { // Inserção no fim
        loja->cauda = novoProduto;
    }
    
    atual->prox = novoProduto;
    loja->numProdutos++;
}

/**
 * @brief Busca um produto na loja pelo ID.
 * @return Ponteiro para o ProdutoLojaNode se encontrado, ou NULL.
 */
static ProdutoLojaNode* _buscarProdutoPorID(Loja *loja, int id) {
    ProdutoLojaNode *atual = loja->cabeca;
    while (atual != NULL) {
        if (atual->id == id) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL; // Não encontrado
}

/**
 * @brief Tenta comprar um item.
 */
static void _comprarItem(Loja *loja, InventarioJogador *inv, int produtoID) {
    ProdutoLojaNode *produto = _buscarProdutoPorID(loja, produtoID);

    if (produto == NULL) {
        printf("\n[ERRO] Produto com ID %d nao encontrado.\n", produtoID);
        Sleep(1500);
        return;
    }

    if (inv->dinheiro < produto->precoAtual) {
        printf("\n[ERRO] Dinheiro insuficiente! Voce precisa de $%.2f.\n", produto->precoAtual);
        Sleep(1500);
        return;
    }

    // Acha a lista de inventário correta (ex: inv->queijos)
    ListaIngrediente *listaDestino = getListaPorID(inv, produto->id);
    if (listaDestino == NULL) {
        printf("\n[ERRO] O produto '%s' nao pode ser armazenado (ID %d invalido).\n", produto->nome, produto->id);
        Sleep(1500);
        return;
    }

    // --- Transação ---
    inv->dinheiro -= produto->precoAtual;
    _adicionarNoIngrediente(listaDestino); // Adiciona 1 nó à lista do jogador

    printf("\n[SUCESSO] Voce comprou 1 %s.\n", produto->nome);
    printf("Novo saldo: $%.2f. Total no inventario: %d\n", inv->dinheiro, listaDestino->quantidade);
    Sleep(1500);
}


/*
 * ===================================================================
 * FUNÇÕES PÚBLICAS (Implementação de loja.h)
 * ===================================================================
 */

// --- Funções do Inventário ---

void inicializarInventario(InventarioJogador *inv, double dinheiroInicial) {
    // Itera por todos os IDs de ingredientes possíveis e zera suas listas
    // IDs 1-13
    for (int i = 1; i <= 13; i++) { 
        ListaIngrediente *lista = getListaPorID(inv, i);
        if (lista != NULL) {
            lista->cabeca = NULL;
            lista->cauda = NULL;
            lista->quantidade = 0;
        }
    }
    inv->dinheiro = dinheiroInicial;
}

void liberarInventario(InventarioJogador *inv) {
    // Itera e libera todas as listas
    for (int i = 1; i <= 13; i++) {
        ListaIngrediente *lista = getListaPorID(inv, i);
        if (lista != NULL) {
            _liberarListaIngrediente(lista);
        }
    }
}

void adicionarItemInventario(InventarioJogador *inv, int ingredienteID) {
    ListaIngrediente *lista = getListaPorID(inv, ingredienteID);
    if (lista != NULL) {
        _adicionarNoIngrediente(lista);
    }
}

int usarItemInventario(InventarioJogador *inv, int ingredienteID) {
    ListaIngrediente *lista = getListaPorID(inv, ingredienteID);
    if (lista != NULL) {
        return _removerNoIngrediente(lista); // Retorna 1 (sucesso) ou 0 (falha)
    }
    return 0; // Falha (ID inválido)
}

int getQuantidadeInventario(InventarioJogador *inv, int ingredienteID) {
    ListaIngrediente *lista = getListaPorID(inv, ingredienteID);
    if (lista != NULL) {
        return lista->quantidade;
    }
    return 0;
}


// --- Funções da Loja ---

void inicializarLoja(Loja *loja) {
    loja->cabeca = NULL;
    loja->cauda = NULL;
    loja->numProdutos = 0;
    // Inicializa o gerador de números aleatórios
    srand((unsigned int)time(NULL));
}

void liberarLoja(Loja *loja) {
    ProdutoLojaNode *atual = loja->cabeca;
    ProdutoLojaNode *temp;
    while (atual != NULL) {
        temp = atual;
        atual = atual->prox;
        free(temp);
    }
    loja->cabeca = NULL;
    loja->cauda = NULL;
    loja->numProdutos = 0;
}

void popularLoja(Loja *loja) {
    // Os produtos são inseridos aqui, sem uma variável global.
    // A função _inserirProdutoOrdenado garante que a lista
    // ficará ordenada alfabeticamente.
    // IDs baseados em estoque.c
    _inserirProdutoOrdenado(loja, 4, "Alface", 4.0);
    _inserirProdutoOrdenado(loja, 6, "Bacon", 4.0);
    _inserirProdutoOrdenado(loja, 2, "Carne", 5.0);
    _inserirProdutoOrdenado(loja, 8, "Cebola", 3.0);
    _inserirProdutoOrdenado(loja, 9, "Falafel", 7.0);
    _inserirProdutoOrdenado(loja, 13, "Frango", 4.0);
    _inserirProdutoOrdenado(loja, 12, "Maionese", 3.0);
    _inserirProdutoOrdenado(loja, 10, "Molho do Pato", 2.0); // Nome de estoque.c
    _inserirProdutoOrdenado(loja, 11, "Onion Rings", 6.0);
    _inserirProdutoOrdenado(loja, 1, "Pao", 2.0);
    _inserirProdutoOrdenado(loja, 7, "Picles", 4.0);
    _inserirProdutoOrdenado(loja, 3, "Queijo", 3.0);
    _inserirProdutoOrdenado(loja, 5, "Tomate", 3.0);
}

void flutuarPrecos(Loja *loja) {
    ProdutoLojaNode *atual = loja->cabeca;
    while (atual != NULL) {
        // Gera variação de -20% a +20%
        // (rand() % 41) -> 0 a 40
        // (rand() % 41 - 20) -> -20 a 20
        // (rand() % 41 - 20) / 100.0 -> -0.20 a +0.20
        double variacao = ((rand() % 41) - 20) / 100.0;
        double novoPreco = atual->precoBase * (1.0 + variacao);

        // Garante que o preço não seja negativo (mínimo 0.10)
        atual->precoAtual = (novoPreco < 0.10) ? 0.10 : novoPreco;
        
        atual = atual->prox;
    }
}

/**
 * @brief Exibe a interface da loja (itens, preços, inventário).
 */
static void _exibirInterfaceLoja(Loja *loja, InventarioJogador *inv) {
    limparTela();
    printf("=========================================================\n");
    printf("                  BEM-VINDO A LOJA                       \n");
    printf("         (Os precos de hoje sao uma loucura!)          \n");
    printf("=========================================================\n\n");
    printf("Seu saldo: $%.2f\n\n", inv->dinheiro);

    printf("--- Seu Inventario Atual ---\n");
    printf(" Paes: %-3d | Carnes: %-3d | Queijos: %-3d | Alfaces: %-3d\n",
        getQuantidadeInventario(inv, 1), getQuantidadeInventario(inv, 2),
        getQuantidadeInventario(inv, 3), getQuantidadeInventario(inv, 4));
    printf(" Tomates: %-3d | Bacons: %-3d | Picles: %-3d | Cebolas: %-3d\n",
        getQuantidadeInventario(inv, 5), getQuantidadeInventario(inv, 6),
        getQuantidadeInventario(inv, 7), getQuantidadeInventario(inv, 8));
    printf(" Falafels: %-3d | Molhos: %-3d | O.Rings: %-3d | Maioneses: %-3d\n",
        getQuantidadeInventario(inv, 9), getQuantidadeInventario(inv, 10),
        getQuantidadeInventario(inv, 11), getQuantidadeInventario(inv, 12));
     printf(" Frangos: %-3d\n", getQuantidadeInventario(inv, 13));
    printf("\n");

    printf("--- Itens a Venda (Ordenados por Nome) ---\n");
    ProdutoLojaNode *atual = loja->cabeca;
    while (atual != NULL) {
        // Ex: "[ 1] Alface          - $3.89"
        printf(" [%2d] %-15s - $%.2f\n", atual->id, atual->nome, atual->precoAtual);
        atual = atual->prox;
    }
    printf("\n=========================================================\n");
}


void loopPrincipalLoja(Loja *loja, InventarioJogador *inv) {
    // Flutua os preços assim que o jogador entra na loja
    flutuarPrecos(loja);

    int running = 1;
    int inputID = -1;

    while (running) {
        _exibirInterfaceLoja(loja, inv);
        printf("Digite o ID do item para comprar, ou [0] para Sair: ");
        
        if (scanf("%d", &inputID) != 1) {
            // Se o input não for um número
            printf("\nEntrada invalida. Por favor, digite um numero.\n");
            limparBufferInput(); // Limpa o buffer
            Sleep(1500);
            continue; // Volta ao início do loop
        }
        
        limparBufferInput(); // Limpa o '\n' deixado pelo scanf

        if (inputID == 0) {
            running = 0; // Sair
        } else {
            // Tenta comprar o item
            _comprarItem(loja, inv, inputID);
            // A função _comprarItem já inclui um Sleep para feedback
        }
    }
    limparTela();
    printf("Voltando ao jogo...\n");
    Sleep(1000);
}