#include "../Header/historico.h"

void inicializa_arvore(NoHistorico **raiz) {
    *raiz = NULL;
}

//Insere ou atualiza vendas na árvore (Recursiva).
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
            //Se o ID ja existe, apenas incrementamos a quantidade
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


//FUNÇÕES AUXILIARES DA AVL
int altura(NO_AVL *n) {
    return (n == NULL ? 0 : n->altura);
}

int max_arvoreAVL(int a, int b) {
    return (a > b ? a : b);
}

// ROTAÇÕES AVL
NO_AVL* rotacaoDireita(NO_AVL *y) {
    NO_AVL *x = y->esq;
    NO_AVL *T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    y->altura = max_arvoreAVL(altura(y->esq), altura(y->dir)) + 1;
    x->altura = max_arvoreAVL(altura(x->esq), altura(x->dir)) + 1;

    return x;
}

NO_AVL* rotacaoEsquerda(NO_AVL *x) {
    NO_AVL *y = x->dir;
    NO_AVL *T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    x->altura = max_arvoreAVL(altura(x->esq), altura(x->dir)) + 1;
    y->altura = max_arvoreAVL(altura(y->esq), altura(y->dir)) + 1;

    return y;
}

int getBalance(NO_AVL *n) {
    return (n == NULL ? 0 : altura(n->esq) - altura(n->dir));
}

// INSERÇÃO ORDENADA POR QUANTIDADE
NO_AVL* inserirOrdenado(NO_AVL *no, int quantidade, char nome[]) {
    if (no == NULL) {
        NO_AVL *novo = (NO_AVL*) malloc(sizeof(NO_AVL));
        strncpy(novo->ingrediente, nome, 49);
        novo->ingrediente[49] = '\0';
        novo->quantidade = quantidade;
        novo->altura = 1;
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (quantidade < no->quantidade)
        no->esq = inserirOrdenado(no->esq, quantidade, nome);

    else if (quantidade > no->quantidade)
        no->dir = inserirOrdenado(no->dir, quantidade, nome);

    else {
        //desempate por nome.
        if (strcmp(nome, no->ingrediente) < 0)
            no->esq = inserirOrdenado(no->esq, quantidade, nome);
        else
            no->dir = inserirOrdenado(no->dir, quantidade, nome);
    }

    no->altura = 1 + max_arvoreAVL(altura(no->esq), altura(no->dir));

    int balance = getBalance(no);

    //Esquerda-Esquerda (Left-Left)
    if (balance > 1 &&
       (quantidade < no->esq->quantidade ||
       (quantidade == no->esq->quantidade && strcmp(nome, no->esq->ingrediente) < 0))) {
        return rotacaoDireita(no);
       }

    //Direita-Direita (Right-Right)
    if (balance < -1 &&
       (quantidade > no->dir->quantidade ||
       (quantidade == no->dir->quantidade && strcmp(nome, no->dir->ingrediente) > 0))) {
        return rotacaoEsquerda(no);
       }

    //Esquerda-Direita (Left-Right)
    if (balance > 1 &&
       (quantidade > no->esq->quantidade ||
       (quantidade == no->esq->quantidade && strcmp(nome, no->esq->ingrediente) > 0))) {
        no->esq = rotacaoEsquerda(no->esq);
        return rotacaoDireita(no);
       }

    //Direita-Esquerda (Right-Left)
    if (balance < -1 &&
       (quantidade < no->dir->quantidade ||
       (quantidade == no->dir->quantidade && strcmp(nome, no->dir->ingrediente) < 0))) {
        no->dir = rotacaoDireita(no->dir);
        return rotacaoEsquerda(no);
       }

    return no;
}


// REMOVER NÓ DA AVL
NO_AVL* removerNo(NO_AVL* root, int quantidade, char nome[]) {
    if (root == NULL)
        return NULL;

    if (quantidade < root->quantidade)
        root->esq = removerNo(root->esq, quantidade, nome);

    else if (quantidade > root->quantidade)
        root->dir = removerNo(root->dir, quantidade, nome);

    else {
        int cmp = strcmp(nome, root->ingrediente);

        if (cmp < 0)
            root->esq = removerNo(root->esq, quantidade, nome);

        else if (cmp > 0)
            root->dir = removerNo(root->dir, quantidade, nome);

        else {
            if (root->esq == NULL) {
                NO_AVL *temp = root->dir;
                free(root);
                return temp;
            }
            else if (root->dir == NULL) {
                NO_AVL *temp = root->esq;
                free(root);
                return temp;
            }

            NO_AVL *temp = root->dir;
            while (temp->esq != NULL)
                temp = temp->esq;

            strcpy(root->ingrediente, temp->ingrediente);
            root->quantidade = temp->quantidade;

            root->dir = removerNo(root->dir, temp->quantidade, temp->ingrediente);
        }
    }

    root->altura = 1 + max_arvoreAVL(altura(root->esq), altura(root->dir));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->esq) >= 0)
        return rotacaoDireita(root);

    if (balance > 1 && getBalance(root->esq) < 0) {
        root->esq = rotacaoEsquerda(root->esq);
        return rotacaoDireita(root);
    }

    if (balance < -1 && getBalance(root->dir) <= 0)
        return rotacaoEsquerda(root);

    if (balance < -1 && getBalance(root->dir) > 0) {
        root->dir = rotacaoDireita(root->dir);
        return rotacaoEsquerda(root);
    }

    return root;
}


//Função auxiliar para encontrar nó pelo nome (necessária pois a árvore é ordenada por quantidade)
NO_AVL* buscarPorNome(NO_AVL* raiz, char* nome) {
    if (raiz == NULL) return NULL;
    if (strcmp(raiz->ingrediente, nome) == 0) return raiz;

    NO_AVL* res = buscarPorNome(raiz->esq, nome);
    if (res != NULL) return res;

    return buscarPorNome(raiz->dir, nome);
}

//FUNÇÃO PRINCIPAL DE REGISTRO.
void registrarIngredienteUsado(NO_AVL **raiz, char nome[]) {
    //Busca via função anterior.
    NO_AVL *encontrado = buscarPorNome(*raiz, nome);

    if (encontrado != NULL) {
        //Se achou, pega a quantidade antiga.
        int quantidadeVelha = encontrado->quantidade;

        //Remove o nó antigo (necessário pois a chave de ordenação mudou).
        *raiz = removerNo(*raiz, quantidadeVelha, nome);

        //Insere com a nova quantidade.
        *raiz = inserirOrdenado(*raiz, quantidadeVelha + 1, nome);
    } else {
        //Se não achou, insere novo com quantidade 1.
        *raiz = inserirOrdenado(*raiz, 1, nome);
    }
}

//Função principal para atualizar ou inserir ingrediente (Quantidade acumulada. YIPEEE).
void atualizar_quantidade_ingrediente(NO_AVL **raiz, char *nome, int qtdAdicional) {
    if (qtdAdicional <= 0) return;

    NO_AVL *encontrado = buscarPorNome(*raiz, nome);
    int novaQuantidade = qtdAdicional;

    if (encontrado != NULL) {
        //Pega a quantidade antiga e soma, caso já exista.
        int qtdAntiga = encontrado->quantidade;
        novaQuantidade += qtdAntiga;

        //Removemos o nó antigo pois a quantidade mudou.
        *raiz = removerNo(*raiz, qtdAntiga, nome);
    }

    //Insere o nó.
    *raiz = inserirOrdenado(*raiz, novaQuantidade, nome);
}

// DESTRUIR E REINICIAR AVL
void destruirAVL(NO_AVL *no) {
    if (no == NULL) return;
    destruirAVL(no->esq);
    destruirAVL(no->dir);
    free(no);
}

void inicializarArvoreIngredientesDia(NO_AVL **raizIngredientes) {
    destruirAVL(*raizIngredientes);
    *raizIngredientes = NULL;
}


// EXIBIÇÃO DE ESTATÍSTICAS
void exibirEstatisticasRec(NO_AVL *no) {
    if (no == NULL) return;

    exibirEstatisticasRec(no->esq);

    printf("%-12s | Usado: %d vezes\n",
        no->ingrediente,
        no->quantidade
    );

    exibirEstatisticasRec(no->dir);
}

void exibirEstatisticasIngredientes(NO_AVL *raizIngredientes) {
    printf("*ESTATÍSTICAS DO DIA*\n");

    if (raizIngredientes == NULL)
        printf("Nenhum ingrediente usado hoje.\n");
    else
        exibirEstatisticasRec(raizIngredientes);

    printf("\nPressione ENTER para prosseguir...");
    getchar();
}

void telaEstatisticasIngredientes(NO_AVL *raizIngredientes) {
    system("cls");
    printf("*ESTATÍSTICAS DE INGREDIENTES*\n");

    if (raizIngredientes == NULL)
        printf("Nenhum ingrediente foi utilizado hoje!\n\n");
    else
        exibirEstatisticasRec(raizIngredientes);

    printf("\n1 - Prosseguir para a loja\n");
    printf("Digite qualquer tecla para continuar...\n");
    getchar();
}