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


//Funções auxiliares
int altura(NO_AVL *n)
{
    return (n == NULL ? 0 : n->altura);
}

int max(int a, int b)
{
    return (a > b ? a : b);
}

NO_AVL* novoNo(char ingrediente[])
{
    // Aloca memória para o tamanho da ESTRUTURA, mas atribui a um PONTEIRO
    NO_AVL *n = (NO_AVL*) malloc(sizeof(NO_AVL));

    if (n == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }

    // Copia segura (evita buffer overflow se a string for gigante)
    strncpy(n->ingrediente, ingrediente, 49);
    n->ingrediente[49] = '\0'; // Garante o fim da string

    n->quantidade = 1;
    n->altura = 1;
    n->esq = n->dir = NULL;

    return n;
}

//Rotações
NO_AVL* rotacaoDireita(NO_AVL *y)
{
    NO_AVL *x = y->esq;
    NO_AVL *T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    y->altura = max(altura(y->esq), altura(y->dir)) + 1;
    x->altura = max(altura(x->esq), altura(x->dir)) + 1;

    return x;
}

NO_AVL* rotacaoEsquerda(NO_AVL *x)
{
    NO_AVL *y = x->dir;
    NO_AVL *T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    x->altura = max(altura(x->esq), altura(x->dir)) + 1;
    y->altura = max(altura(y->esq), altura(y->dir)) + 1;

    return y;
}

int getBalance(NO_AVL *n)
{
    return (n == NULL ? 0 : altura(n->esq) - altura(n->dir));
}

//Inserção e contagem
NO_AVL* inserirIngrediente(NO_AVL *no, char ingrediente[])
{

    if (no == NULL)
	{
        return novoNo(ingrediente);
    }

    int cmp = strcmp(ingrediente, no->ingrediente);

    if (cmp == 0)
	{
        no->quantidade += 1;
        return no;
    }

    if (cmp < 0)
	{
        no->esq = inserirIngrediente(no->esq, ingrediente);
    }

    else
	{
        no->dir = inserirIngrediente(no->dir, ingrediente);
    }

    no->altura = 1 + max(altura(no->esq), altura(no->dir));

    int balance = getBalance(no);

    if (balance > 1 && strcmp(ingrediente, no->esq->ingrediente) < 0)
        return rotacaoDireita(no);

    if (balance < -1 && strcmp(ingrediente, no->dir->ingrediente) > 0)
        return rotacaoEsquerda(no);

    if (balance > 1 && strcmp(ingrediente, no->esq->ingrediente) > 0)
	{
        no->esq = rotacaoEsquerda(no->esq);
        return rotacaoDireita(no);
    }

    if (balance < -1 && strcmp(ingrediente, no->dir->ingrediente) < 0)
	{
        no->dir = rotacaoDireita(no->dir);
        return rotacaoEsquerda(no);
    }

    return no;
}

//Contagem dos ingredientes
void registrarIngredienteUsado(NO_AVL *raizIngredientes, char ingrediente[])
{
    raizIngredientes = inserirIngrediente(raizIngredientes, ingrediente);
}

//Reiniciar estatísticas do dia
void destruirAVL(NO_AVL *no)
{
    if (no == NULL) return;

    destruirAVL(no->esq);
    destruirAVL(no->dir);
    free(no);
}

void inicializarArvoreIngredientesDia(NO_AVL *raizIngredientes)
{
    destruirAVL(raizIngredientes);
    raizIngredientes = NULL;
}

//Inprimir estatísticas do jogador
void exibirEstatisticasRec(NO_AVL *no)
{
    if (no == NULL) return;

    exibirEstatisticasRec(no->esq);

    printf("Ingrediente: %-20s  Usado: %d\n", no->ingrediente, no->quantidade);

    exibirEstatisticasRec(no->dir);
}

void exibirEstatisticasIngredientes(NO_AVL *raizIngredientes)
{
    printf("**ESTATÍSTICAS DO DIA**\n");

    if (raizIngredientes == NULL)
	{
        printf("Nenhum ingrediente usado hoje.\n");
    }

	else
	{
        exibirEstatisticasRec(raizIngredientes);
    }

    printf("\nPressione ENTER para prosseguir...");

    getchar();
}

//Tela de estatísticas
void telaEstatisticasIngredientes(NO_AVL *raizIngredientes)
{
    system("cls");
    printf("**ESTATÍSTICAS DE INGREDIENTES**\n");

    if (raizIngredientes == NULL)
	{
        printf("Nenhum ingrediente foi utilizado hoje!\n\n");
    }

	else
	{
        exibirEstatisticasRec(raizIngredientes);

        printf("\n");
    }

    printf("1 - Prosseguir para a loja\n");
    printf("Digite qualquer tecla para continuar...\n");

    getchar();
}