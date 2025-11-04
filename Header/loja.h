#ifndef LOJA_H
#define LOJA_H

//estruturas de inventario

//no para cada ingrediente
typedef struct ingredino {
    struct ingredino *prox;
    struct ingredino *ant;
} ingredino;

//struct para gerenciar a lista de ingredientes
typedef struct listaingredientes {
    ingredino *topo;
    ingredino *base;
    int quantidade; 
} listaingredientes;

//no para produto da loja
typedef struct noprodutoloja {
    int id;
    char nome[50]; // menos que isso tava dando problema
    float precoBase; 
    struct noprodutoloja *prox; 
    struct noprodutoloja *ant; 
} noprodutoloja;

//struct principal da loja
typedef struct Loja {
    noprodutoloja *topo;
    noprodutoloja *base;
    int numprodutos;
} Loja;

// struct do inventario principal do player
typedef struct Inventarioplayer {
    listaingredientes paes;        // ID 1
    listaingredientes carnes;       // ID 2
    listaingredientes queijos;      // ID 3
    listaingredientes alfaces;     // ID 4
    listaingredientes tomates;      // ID 5
    listaingredientes bacons;       // ID 6
    listaingredientes picles;       // ID 7
    listaingredientes cebolas;      // ID 8
    listaingredientes falafels;     // ID 9
    listaingredientes maionese_de_pato;       // ID 10 
    listaingredientes onionRings;   // ID 11
    listaingredientes maioneses;    // ID 12
    listaingredientes frangos;      // ID 13
    float dinheiro;
} Inventarioplayer;

//funcoes de inventario




void inicializarInventario_loja(Inventarioplayer *inv, float dinheiroInicial);

//libera a memoria alocada dos nos no inventario
void liberarInventario(Inventarioplayer *inv);

//adicionai item ao inventario do player
void adicionarItemInventario(Inventarioplayer *inv, int ingredienteID);

// pra remover o item do inventario
int usarItemInventario(Inventarioplayer *inv, int ingredienteID);

//retorna a quantidade atual de um ingrediente.
int qntitem(Inventarioplayer *inv, int ingredienteID);


//funcoes pra loja





void inicializarLoja(Loja *loja);

// libera a memoria dos nos da loja
void liberarLoja(Loja *loja);

//vai adicionar os produtos na loja em ordem alfabetica
void organizaloja(Loja *loja);

//em tese vai ser usado pra a loja funcionar
void loopfuncionaloja(Loja *loja, Inventarioplayer *inv);


#endif // LOJA_H
