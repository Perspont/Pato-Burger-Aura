#include "../Header/loja.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h> 
#else
#include <unistd.h> 
#define Sleep(ms) usleep(ms * 1000)
#endif


//funcoes internas
static void limparBufferInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//liimpa o terminal 

static void limparTela() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

// libera os nos de uma lista de ingredientes

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

//adiciona um no (uma unidade) a lista de ingredientes do jogador.
static void _adicionarNoIngrediente(ListaIngrediente *lista) {
    IngredienteNode *novoNo = (IngredienteNode*) malloc(sizeof(IngredienteNode));
    if (novoNo == NULL) {
        perror("Erro ao alocar memoria para ingrediente");
        return;
    }
    novoNo->prox = NULL;
    novoNo->ant = lista->cauda; 

    if (lista->cabeca == NULL) { 
        lista->cabeca = novoNo;
    } else { 
        lista->cauda->prox = novoNo; //
    }
    lista->cauda = novoNo; //
    lista->quantidade++;
}

//remove um no (uma unidade) da lista de ingredientes.

static int _removerNoIngrediente(ListaIngrediente *lista) {
    if (lista->quantidade == 0 || lista->cauda == NULL) {
        return 0; // sem estoquee
    }

    IngredienteNode *noRemovido = lista->cauda;

    if (lista->cabeca == lista->cauda) { 
        lista->cabeca = NULL;
        lista->cauda = NULL;
    } else { 
        lista->cauda = noRemovido->ant; 
        lista->cauda->prox = NULL; 
    }

    free(noRemovido);
    lista->quantidade--;
    return 1; // Sucesso
}


//retorna um ponteiro para a lista de ingredientes correta no inventario com base no ID do produto
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

/**adiciona um produto a loja, a insercao eh feita em ordem alfebetica ok*/
static void _inserirProdutoOrdenado(Loja *loja, int id, const char *nome, double preco) {
    ProdutoLojaNode *novoProduto = (ProdutoLojaNode*) malloc(sizeof(ProdutoLojaNode));
    if (novoProduto == NULL) {
        perror("Erro ao alocar memoria para produto da loja");
        return;
    }
    // isso previne um problema de buffer se o nome for maior que 49 caracteres.
    strncpy(novoProduto->nome, nome, 49);
    novoProduto->nome[49] = '\0'; // faz a string sempre terminar nula
    
    novoProduto->id = id;
    novoProduto->precoBase = preco;
    novoProduto->precoAtual = preco;
    novoProduto->prox = NULL;
    novoProduto->ant = NULL;

    // "case 1" lista vazia 
    if (loja->cabeca == NULL) {
        loja->cabeca = novoProduto;
        loja->cauda = novoProduto;
        loja->numProdutos++;
        return;
    }

    // "case 2" insere antes da cabeça, quando eh o primeiro do alfabeto
    if (strcmp(nome, loja->cabeca->nome) < 0) {
        novoProduto->prox = loja->cabeca;
        loja->cabeca->ant = novoProduto;
        loja->cabeca = novoProduto;
        loja->numProdutos++;
        return;
    }

    // "case 3" insere no meio ou no fim
    ProdutoLojaNode *atual = loja->cabeca;
   
    while (atual->prox != NULL && strcmp(nome, atual->prox->nome) > 0) {
        atual = atual->prox;
    }

    // atual eh o no antes do ponto de que divide
    novoProduto->prox = atual->prox;
    novoProduto->ant = atual;

    if (atual->prox != NULL) { 
        atual->prox->ant = novoProduto;
    } else {
        loja->cauda = novoProduto;
    }
    
    atual->prox = novoProduto;
    loja->numProdutos++;
}

//busca de produto na loja pelo ID
static ProdutoLojaNode* _buscarProdutoPorID(Loja *loja, int id) {
    ProdutoLojaNode *atual = loja->cabeca;
    while (atual != NULL) {
        if (atual->id == id) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}


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

    // acha a lista de inventario certa
    ListaIngrediente *listaDestino = getListaPorID(inv, produto->id);
    if (listaDestino == NULL) {
        printf("\n[ERRO] O produto '%s' nao pode ser armazenado (ID %d invalido).\n", produto->nome, produto->id);
        Sleep(1500);
        return;
    }

    // pagamento
    inv->dinheiro -= produto->precoAtual;
    _adicionarNoIngrediente(listaDestino); // vai colocar outro no

    printf("\n[SUCESSO] Voce comprou 1 %s.\n", produto->nome);
    printf("Novo saldo: $%.2f. Total no inventario: %d\n", inv->dinheiro, listaDestino->quantidade);
    Sleep(1500);
}


//Implementação de loja.h)


// funcoes de inventario

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
    // itera e libera todas as listas
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
        return _removerNoIngrediente(lista); 
    }
    return 0; 
}

int getQuantidadeInventario(InventarioJogador *inv, int ingredienteID) {
    ListaIngrediente *lista = getListaPorID(inv, ingredienteID);
    if (lista != NULL) {
        return lista->quantidade;
    }
    return 0;
}


// funcoes da loja

void inicializarLoja(Loja *loja) {
    loja->cabeca = NULL;
    loja->cauda = NULL;
    loja->numProdutos = 0;
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
    // Os produtos sao inseridos aqui ok
    _inserirProdutoOrdenado(loja, 4, "Alface", 4.0);
    _inserirProdutoOrdenado(loja, 6, "Bacon", 4.0);
    _inserirProdutoOrdenado(loja, 2, "Carne", 5.0);
    _inserirProdutoOrdenado(loja, 8, "Cebola", 3.0);
    _inserirProdutoOrdenado(loja, 9, "Falafel", 7.0);
    _inserirProdutoOrdenado(loja, 13, "Frango", 4.0);
    _inserirProdutoOrdenado(loja, 12, "Maionese", 3.0);
    _inserirProdutoOrdenado(loja, 10, "Molho do Pato", 2.0); 
    _inserirProdutoOrdenado(loja, 11, "Onion Rings", 6.0);
    _inserirProdutoOrdenado(loja, 1, "Pao", 2.0);
    _inserirProdutoOrdenado(loja, 7, "Picles", 4.0);
    _inserirProdutoOrdenado(loja, 3, "Queijo", 3.0);
    _inserirProdutoOrdenado(loja, 5, "Tomate", 3.0);
}

void flutuarPrecos(Loja *loja) {
    ProdutoLojaNode *atual = loja->cabeca;
    while (atual != NULL) {
        double variacao = ((rand() % 41) - 20) / 100.0;
        double novoPreco = atual->precoBase * (1.0 + variacao);
        atual->precoAtual = (novoPreco < 0.10) ? 0.10 : novoPreco;
        atual = atual->prox;
    }
}

// exibe interface da loja
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
        printf(" [%2d] %-15s - $%.2f\n", atual->id, atual->nome, atual->precoAtual);
        atual = atual->prox;
    }
    printf("\n=========================================================\n");
}


void loopPrincipalLoja(Loja *loja, InventarioJogador *inv) {
    flutuarPrecos(loja);

    int running = 1;
    int inputID = -1;

    while (running) {
        _exibirInterfaceLoja(loja, inv);
        printf("Digite o ID do item para comprar, ou [0] para Sair: ");
        
        if (scanf("%d", &inputID) != 1) {
            // se o input nao for numero
            printf("\nEntrada invalida. Por favor, digite um numero.\n");
            limparBufferInput(); 
            Sleep(1500);
            continue; 
        }
        
        limparBufferInput(); // Limpa

        if (inputID == 0) {
            running = 0; 
        } else {
            _comprarItem(loja, inv, inputID);

        }
    }
    limparTela();
    printf("Voltando ao jogo...\n");
    Sleep(1000);
}
