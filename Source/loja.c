#include "loja.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // <-- ERRO 2 CORRIGIDO

#ifdef _WIN32
#include <windows.h> 
#else
#include <unistd.h> 
#define Sleep(ms) usleep(ms * 1000)
#endif

//funcoes internas
static void limparBufferInput() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//limpa o terminal 
static void limparTela() 
{
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

// libera os nos da lista de ingredientes
static void liberaringredilista(listaingredientes *lista) 
{
    ingredino *atual = lista->topo; 
    ingredino *temp;
    while (atual != NULL) {
        temp = atual;
        atual = atual->prox;
        free(temp);
    }
    lista->base = NULL; 
    lista->topo = NULL; 
    lista->quantidade = 0;
}
//adiciona um no (uma unidade) a lista de ingredientes do jogador.
static void adicionaingredino(listaingredientes *lista) 
{
    ingredino *novoNo = (ingredino*) malloc(sizeof(ingredino));
    if (novoNo == NULL) {
        perror("Erro ao alocar memoria para ingrediente");
        return;
    }
    novoNo->prox = NULL;
    novoNo->ant = lista->base; 
    if (lista->topo == NULL) { 
        lista->topo = novoNo; 
    } else { 
        lista->base->prox = novoNo; 
    }
    lista->base = novoNo; 
    lista->quantidade++;
}
//remove um no (uma unidade) da lista de ingredientes.
static int removeringredino(listaingredientes *lista) 
{
    if (lista->quantidade == 0 || lista->base == NULL) { 
        return 0; 
    }
    ingredino *noRemovido = lista->base; 
    if (lista->topo == lista->base) { 
        lista->topo = NULL; 
        lista->base = NULL; 
    } else { 
        lista->base = noRemovido->ant; 
        lista->base->prox = NULL; 
    }
    free(noRemovido);
    lista->quantidade--;
    return 1;
}
//aponta um ponteiro para a lista de ingredientes no inventario com base no ID do produto
static listaingredientes* ListaPorID(Inventarioplayer *inv, int id) 
{ 
    switch (id) {
        case 1:  return &inv->paes; // ID 1
        case 2:  return &inv->carnes; // ID 2
        case 3:  return &inv->queijos; // ID 3
        case 4:  return &inv->alfaces; // ID 4
        case 5:  return &inv->tomates; // ID 5
        case 6:  return &inv->bacons; // ID 6
        case 7:  return &inv->picles; // ID 7
        case 8:  return &inv->cebolas; // ID 8
        case 9:  return &inv->falafels; // ID 9
        case 10: return &inv->maionese_de_pato; // ID 10
        case 11: return &inv->onionRings; // ID 11
        case 12: return &inv->maioneses; // ID 12
        case 13: return &inv->frangos; // ID 13
        default: return NULL;
    }
}

//adiciona um produto a loja, a insercao eh feita em ordem alfebetica ok
static void inserereproduto(Loja *loja, int id, const char *nome, float preco) { 
    noprodutoloja *novoProduto = (noprodutoloja*) malloc(sizeof(noprodutoloja));
    if (novoProduto == NULL) {
        perror("Erro ao inserir produto");
        return;
    }
    // isso previne dar problema se for maior que 49 caracteres.
    strncpy(novoProduto->nome, nome, 49);
    novoProduto->nome[49] = '\0'; 
    //

    novoProduto->id = id;
    novoProduto->precoBase = preco;
    novoProduto->prox = NULL;
    novoProduto->ant = NULL;

    // "case 1" lista vazia 
    if (loja->topo == NULL) { 
        loja->topo = novoProduto; 
        loja->base = novoProduto; 
        loja->numprodutos++;
        return;
    }
    // "case 2" insere antes do topo, quando eh o primeiro do alfabeto
    if (strcmp(nome, loja->topo->nome) < 0) { 
        novoProduto->prox = loja->topo; 
        loja->topo->ant = novoProduto; 
        loja->topo = novoProduto; 
        loja->numprodutos++;
        return;
    }

    // "case 3" insere no meio ou no fim
    noprodutoloja *atual = loja->topo; 
   
    while (atual->prox != NULL && strcmp(nome, atual->prox->nome) > 0) {
        atual = atual->prox;
    }
    novoProduto->prox = atual->prox;
    novoProduto->ant = atual;

    if (atual->prox != NULL) { 
        atual->prox->ant = novoProduto;
    } else {
        loja->base = novoProduto; 
    }
    
    atual->prox = novoProduto;
    loja->numprodutos++;
}

//busca de produto na loja pelo ID
static noprodutoloja* buscarID(Loja *loja, int id) 
{ 
    noprodutoloja *atual = loja->topo; 
    while (atual != NULL) {
        if (atual->id == id) {
            return atual;
        }
        atual = atual->prox;
        }
    return NULL;
}   

static void compraritem(Loja *loja, Inventarioplayer *inv, int produtoID) 
{
    noprodutoloja *produto = buscarID(loja, produtoID); 
    if (produto == NULL) {
        printf("\n[ERRO] Produto com ID %d nao encontrado.\n", produtoID);
        Sleep(1500);
        return;
    }

    if (inv->dinheiro < produto->precoBase) { 
        printf("\n[ERRO] Dinheiro insuficiente! Voce precisa de $%.2f.\n", produto->precoBase); 
        Sleep(1500);
        return;
    }

    // acha a lista de inventario certa
    listaingredientes *listaDestino = ListaPorID(inv, produto->id);
    if (listaDestino == NULL) {
        printf("\n[ERRO] O produto '%s' nao pode ser armazenado (ID %d invalido).\n", produto->nome, produto->id);
        Sleep(1500);
        return;
    }

    // pagamento feito de maneira certa
    inv->dinheiro -= produto->precoBase;
    adicionaingredino(listaDestino); 
    printf("\n[SUCESSO] Voce comprou 1 %s.\n", produto->nome);
    printf("Novo saldo: $%.2f. Total no inventario: %d\n", inv->dinheiro, listaDestino->quantidade);
    Sleep(1500);
}

// funcoes de inventario
void inicializarinvloja(Inventarioplayer *inv, float dinheiroInicial) 
{ 
    for (int i = 1; i <= 13; i++) { // passa pelos ids
        listaingredientes *lista = ListaPorID(inv, i);
        if (lista != NULL) {
            lista->topo = NULL; 
            lista->base = NULL; 
            lista->quantidade = 0;
        }
    }
    inv->dinheiro = dinheiroInicial;
}

void liberarInventario(Inventarioplayer *inv) 
{ 
    //libera tudo
    for (int i = 1; i <= 13; i++) {
        listaingredientes *lista = ListaPorID(inv, i);
        if (lista != NULL) {
            liberaringredilista(lista); 
        }
    }
}

void adicionarItemInventario(Inventarioplayer *inv, int ingredienteID) 
{ 
    listaingredientes *lista = ListaPorID(inv, ingredienteID); 
    if (lista != NULL) {
        adicionaingredino(lista);
    }
}

int usarItemInventario(Inventarioplayer *inv, int ingredienteID) 
{ 
    listaingredientes *lista = ListaPorID(inv, ingredienteID); 
    if (lista != NULL) {
        return removeringredino(lista); 
    }
    return 0; 
}

int qntitem(Inventarioplayer *inv, int ingredienteID) 
{ 
    listaingredientes *lista = ListaPorID(inv, ingredienteID); 
    if (lista != NULL) {
        return lista->quantidade;
    }
    return 0;
}


// funcoes da loja

void inicializarLoja(Loja *loja) {
    loja->base = NULL;
    loja->topo = NULL; 
    loja->numprodutos = 0;
    srand((unsigned int)time(NULL));
}

void organizaloja(Loja *loja) { 
    // informacoes dos produtos para serem inseridos
    inserereproduto(loja, 4, "Alface", 4.0);
    inserereproduto(loja, 6, "Bacon", 4.0);
    inserereproduto(loja, 2, "Carne", 5.0);
    inserereproduto(loja, 8, "Cebola", 3.0);
    inserereproduto(loja, 9, "Falafel", 7.0);
    inserereproduto(loja, 13, "Frango", 4.0);
    inserereproduto(loja, 12, "Maionese", 3.0);
    inserereproduto(loja, 10, "Maionese de Pato", 2.0); 
    inserereproduto(loja, 11, "Onion Rings", 6.0);
    inserereproduto(loja, 1, "Pao", 2.0);
    inserereproduto(loja, 7, "Picles", 4.0);
    inserereproduto(loja, 3, "Queijo", 3.0);
    inserereproduto(loja, 5, "Tomate", 3.0);
}

void liberarLoja(Loja *loja) {
    noprodutoloja *atual = loja->topo; 
    noprodutoloja *temp; //ponteiro temporario para ir liberando a loja
    while (atual != NULL) {
        temp = atual;
        atual = atual->prox;
        free(temp);
    }
    loja->topo = NULL; 
    loja->base = NULL; 
    loja->numprodutos = 0;
}


static void exibirinterfaceloja(Loja *loja, Inventarioplayer *inv) 
{ 
    limparTela();
    printf("=========================================================\n");
    printf("https://www.lojapatonica.com.br/app/30901f2ef9998ef3    .\n");         
    printf(".                                                       .\n");
    printf(".                  BEM-VINDO A LOJA.                     .\n");
    printf(".                                                       .\n");
    printf("=========================================================\n");
    printf("Seu saldo: $%.2f\n\n", inv->dinheiro);
    printf("--- Seu Inventario Atual ---\n");
    printf(" Paes: %-3d | Carnes: %-3d | Queijos: %-3d | Alfaces: %-3d\n",
        qntitem(inv, 1), qntitem(inv, 2),
        qntitem(inv, 3), qntitem(inv, 4)); 
    printf(" Tomates: %-3d | Bacons: %-3d | Picles: %-3d | Cebolas: %-3d\n",
        qntitem(inv, 5), qntitem(inv, 6),
        qntitem(inv, 7), qntitem(inv, 8)); 
    printf(" Falafels: %-3d | M. Pato: %-3d | O.Rings: %-3d | Maioneses: %-3d\n", 
        qntitem(inv, 9), qntitem(inv, 10), 
        qntitem(inv, 11), qntitem(inv, 12)); 
     printf(" Frangos: %-3d\n", qntitem(inv, 13)); 
    printf("\n");
    printf("---Itens a Venda---\n");
    noprodutoloja *atual = loja->topo; 
    while (atual != NULL) {
        printf(" [%2d] %-15s - $%.2f\n", atual->id, atual->nome, atual->precoBase); 
        atual = atual->prox;
    }
}

void loopfuncionaloja(Loja *loja, Inventarioplayer *inv) 
{
    int funcionando = 1;
    int IDselecionado = -1;
    while (funcionando) {
        exibirinterfaceloja(loja, inv);
        printf("Digite o ID do item para comprar, ou [0] para Sair: ");
        
        if (scanf("%d", &IDselecionado) != 1) {
            // se o input nao for numero
            printf("\nEntrada invalida. Por favor, digite um numero.\n");
            limparBufferInput(); 
            Sleep(1500);
            continue; 
        }
        limparBufferInput();
        if (IDselecionado == 0) {
            funcionando = 0; 
        } else {
            compraritem(loja, inv, IDselecionado);

        }
    }
    limparTela();
    printf("Voltando ao jogo...\n");
    Sleep(1000);
}