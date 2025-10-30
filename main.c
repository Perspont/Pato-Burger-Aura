#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   
#include <string.h>

/* --- HEADERS DO JOGO (Caminhos corrigidos) --- */
#include "burger.h"  /* Usa PilhaLE.h (Dinâmico) */
#include "pedidos.h" 
/* ------------------------------------------- */

/* --- HEADER DA LOJA --- */
#include "listade.h" 
/* -------------------- */

// ==================================================================
// --- DEFINIÇÕES VINDAS DO 'game.h' (AGORA DENTRO DO MAIN) ---
// ==================================================================

/*
 * O 'game.h' não é mais necessário. 
 * A struct Estado foi movida para cá.
 */
typedef struct
{
	int dia;
	float dinheiro;
} Estado;

/*
 * A definição de 'inicializarEstado' (de game.c)
 * foi movida para cá para resolver o erro de linker.
 */
void inicializarEstado(Estado *estado)
{
	estado->dia = 1;
	estado->dinheiro = 100.0;
}

// ==================================================================
// --- ESTRUTURAS DA LOJA ---
// ==================================================================

// Dados do Produto - A ID será a 'info' na sua lista
typedef struct {
    int id;
    char nome[50];
    int quantidade_loja; // Estoque DISPONÍVEL NA LOJA (para compra)
    int quantidade_hamb; // Estoque NA HAMBURGUERIA (ingredientes comprados)
    float preco;
    int estoque_max_loja; // Máximo que a Loja pode ter
} Produto;

// Catálogo global de produtos
#define MAX_PRODUTOS 13 // Total de produtos
Produto catalogo[MAX_PRODUTOS] = {
    // ID, Nome, Qtd_Loja_Inicial, Qtd_Hamb_Inicial, Preço, Max_Loja
    {1, "Pao", 50, 10, 2.00, 100}, 
    {2, "Carne", 30, 10, 5.00, 100},
    {3, "Queijo", 40, 10, 3.00, 100},
    {4, "Alface", 20, 10, 4.00, 100},
    {5, "Tomate", 20, 10, 3.00, 100},
    {6, "Bacon", 15, 10, 4.00, 100},
    {7, "Picles", 10, 10, 4.00, 100},
    {8, "Cebola", 25, 10, 3.00, 100},
    {9, "Falafel", 5, 10, 7.00, 50},
    {10, "Molho do Pato", 10, 10, 2.00, 50},
    {11, "Onion Rings", 10, 10, 6.00, 50},
    {12, "Maionese", 30, 10, 3.00, 100},
    {13, "Frango", 15, 10, 4.00, 75}
};


// --- Funções Auxiliares (do main.c Loja) ---

Produto *busca_produto_por_id(int id) {
    if (id > 0 && id <= MAX_PRODUTOS) {
        return &catalogo[id - 1];
    }
    return NULL;
}

int produto_existe(int id) {
    return (id > 0 && id <= MAX_PRODUTOS);
}

// Garante que a lista dinâmica reflita o estado de um estoque específico (loja ou hamburgueria)
void gerenciar_lista_dinamica(tp_listad *lista, Produto *p, int tipo_estoque) {
    int quantidade = (tipo_estoque == 1) ? p->quantidade_loja : p->quantidade_hamb;
    tp_no *no = busca_listade(lista, p->id);

    if (quantidade > 0 && no == NULL) {
        insere_listad_no_fim(lista, p->id);
    } else if (quantidade == 0 && no != NULL) {
        remove_listad(lista, p->id);
        printf("[INFO] Produto '%s' removido da lista (Estoque zerado).\n", p->nome);
    }
}

// --- Funções do Sistema de Loja e Hamburgueria (do main.c Loja) ---

/**
 * @brief Modifica o estoque. Retorna 1 em sucesso, 0 em falha (falta de estoque).
 */
int modificar_estoque(tp_listad *lista_loja, tp_listad *lista_hamb, int id, int delta_loja, int delta_hamb) {
    Produto *p = busca_produto_por_id(id);

    if (p == NULL) {
        printf("[ERRO] Produto com ID %d nao encontrado.\n", id);
        return 0;
    }

    // --- LOGICA DA LOJA ---
    int nova_qtd_loja = p->quantidade_loja + delta_loja;
    if (nova_qtd_loja < 0) {
        printf("[ERRO] Nao ha estoque suficiente na LOJA (apenas %d disponiveis).\n", p->quantidade_loja);
        return 0; // Falha
    }
    if (nova_qtd_loja > p->estoque_max_loja) {
        nova_qtd_loja = p->estoque_max_loja;
    }
    
    // --- LOGICA DA HAMBURGUERIA ---
    int nova_qtd_hamb = p->quantidade_hamb + delta_hamb;
    if (nova_qtd_hamb < 0) {
         printf("[ERRO] Nao ha %s suficiente na HAMBURGUERIA (apenas %d disponiveis).\n", p->nome, p->quantidade_hamb);
         return 0; // Falha
    }
    
    // Se chegou aqui, a transação é válida. Efetiva as mudanças.
    p->quantidade_loja = nova_qtd_loja;
    p->quantidade_hamb = nova_qtd_hamb;
    
    gerenciar_lista_dinamica(lista_loja, p, 1);
    gerenciar_lista_dinamica(lista_hamb, p, 2);
    
    return 1; // Sucesso
}


/**
 * @brief Funcao wrapper para compra (Loja -> Hamburgueria).
 */
void comprar_para_hamburgueria(tp_listad *lista_loja, tp_listad *lista_hamb, int id, int quantidade) {
    Produto *p = busca_produto_por_id(id);
    if (p == NULL) {
        printf("[ERRO] ID de produto invalido.\n");
        return;
    }
    if (quantidade < 1) {
         printf("[ERRO] A quantidade deve ser maior que zero.\n");
        return;
    }
    
    // Tenta modificar o estoque. A função modificar_estoque tratará a falta de estoque.
    // Compra: Diminui LOJA (-quantidade) e Aumenta HAMBURGUERIA (+quantidade)
    if (modificar_estoque(lista_loja, lista_hamb, id, -quantidade, quantidade)) {
        printf("[SUCESSO] Compra de %d unidades de %s realizada. Custo: R$ %.2f\n", 
               quantidade, p->nome, (quantidade * p->preco));
        // TODO: Deduzir estado.dinheiro
    }
}


/**
 * @brief Lista um estoque específico (Loja ou Hamburgueria)
 */
void listar_estoque_hamburgueria(tp_listad *lista_hamb) {
    const char *titulo = "ESTOQUE DA HAMBURGUERIA (Seus Ingredientes)";
    
    if (lista_hamb == NULL || listad_vazia(lista_hamb)) {
        printf("\n[Estoque] %s: Lista vazia.\n", titulo);
        return;
    }

    printf("\n--- %s ---\n", titulo);
    printf("| %-3s | %-20s | %-7s |\n", "ID", "Ingrediente", "QTD");
    printf("|-----|----------------------|---------|\n");

    tp_no *atu = lista_hamb->ini;
    while (atu != NULL) {
        Produto *p = busca_produto_por_id(atu->info);
        if (p != NULL) {
            int quantidade = p->quantidade_hamb;
            if (quantidade > 0) { // Dupla verificação (lista deveria estar correta)
                printf("| %-3d | %-20s | %-7d |\n",
                       p->id, p->nome, quantidade);
            }
        }
        atu = atu->prox;
    }
    printf("-----------------------------------------\n");
}

void listar_estoque_loja(tp_listad *lista_loja) {
    const char *titulo = "ESTOQUE DA LOJA (Disponível para Compra)";
    
    if (lista_loja == NULL || listad_vazia(lista_loja)) {
        printf("\n[Estoque] %s: Lista vazia.\n", titulo);
        return;
    }

    printf("\n--- %s ---\n", titulo);
    printf("| %-3s | %-20s | %-7s | %-5s |\n", "ID", "Ingrediente", "QTD", "Preco");
    printf("|-----|----------------------|---------|-------|\n");

    tp_no *atu = lista_loja->ini;
    while (atu != NULL) {
        Produto *p = busca_produto_por_id(atu->info);
        if (p != NULL) {
            int quantidade = p->quantidade_loja;
            float preco_unitario = p->preco;
            if (quantidade > 0) {
                printf("| %-3d | %-20s | %-7d | R$%.2f |\n",
                       p->id, p->nome, quantidade, preco_unitario);
            }
        }
        atu = atu->prox;
    }
    printf("----------------------------------------------------\n");
}


/**
 * @brief Simula a reposição/entrega aleatória de itens na LOJA.
 */
void simular_reposicao_loja(tp_listad *lista_loja) {
    printf("\n======================================================\n");
    printf("            SIMULANDO REPOSICAO DA LOJA\n");
    printf("======================================================\n");
    
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        Produto *p = &catalogo[i];
        
        int min_reposicao = p->estoque_max_loja / 5; 
        int max_reposicao = p->estoque_max_loja * 7 / 10; 
        
        if (min_reposicao > max_reposicao) min_reposicao = max_reposicao;
        if (min_reposicao < 1) min_reposicao = 1;
        
        int reposicao_aleatoria = (rand() % (max_reposicao - min_reposicao + 1)) + min_reposicao;
        int reposicao_efetiva = reposicao_aleatoria;
        int nova_quantidade = p->quantidade_loja + reposicao_aleatoria;

        if (nova_quantidade > p->estoque_max_loja) {
             reposicao_efetiva = p->estoque_max_loja - p->quantidade_loja;
             nova_quantidade = p->estoque_max_loja;
        }

        if (reposicao_efetiva > 0) {
            p->quantidade_loja = nova_quantidade;
        }
        
        gerenciar_lista_dinamica(lista_loja, p, 1);
    }
    printf("----------------------------------------------------\n");
    printf("Novos itens chegaram na loja!\n");
}

// ==================================================================
// --- FUNÇÕES DO JOGO (Movidas do game.c para cá) ---
// ==================================================================

/**
 * @brief Verifica e consome ingredientes do estoque para um hambúrguer.
 * Retorna 1 se for bem-sucedido, 0 se faltar estoque.
 */
int prepararBurger(Burger *burger, tp_listad *lista_loja, tp_listad *lista_hamb) {
    
    // Como a pilha (PilhaLE) não tem uma função "altura" ou "iterar",
    // precisamos de uma pilha auxiliar para verificar e depois consumir.
    
    tp_pilha *pilha_aux = inicializa_pilha();
    tp_item ingrediente_id;
    int sucesso = 1; // Começa como verdadeiro
    
    // Fase 1: VERIFICAÇÃO. Desempilha o burger na pilha aux.
    while (pop(burger->ingredientes, &ingrediente_id)) {
        push(pilha_aux, ingrediente_id);
        
        Produto *p = busca_produto_por_id(ingrediente_id);
        if (p == NULL) {
            printf("[ERRO PREPARO] Ingrediente ID %d nao existe no catalogo!\n", ingrediente_id);
            sucesso = 0;
            break; // Sai do while
        }
        
        // Esta é uma verificação simples, não conta múltiplos do mesmo ingrediente.
        // TODO: Melhorar esta verificação para contar ingredientes.
        if (p->quantidade_hamb <= 0) {
            printf("[ERRO PREPARO] Falta de estoque de: %s\n", p->nome);
            sucesso = 0;
            break; // Sai do while
        }
    }
    
    // Fase 2: AÇÃO (Consumir ou Reverter)
    if (sucesso) {
        // Sucesso! Agora, desempilha da aux e gasta o estoque
        while (pop(pilha_aux, &ingrediente_id)) {
            // Gasta o estoque (delta_loja=0, delta_hamb=-1)
            modificar_estoque(lista_loja, lista_hamb, ingrediente_id, 0, -1);
            
            // Re-empilha na pilha original do burger
            push(burger->ingredientes, ingrediente_id);
        }
        destroi_pilha(pilha_aux);
        return 1; // Burger feito!
    } else {
        // Falha. Apenas devolve os ingredientes para a pilha original.
        printf("[ERRO PREPARO] Nao foi possivel fazer o burger: %s\n", burger->nome);
        while (pop(pilha_aux, &ingrediente_id)) {
            push(burger->ingredientes, ingrediente_id);
        }
        destroi_pilha(pilha_aux);
        return 0; // Falha
    }
}


/**
 * @brief Simula um dia de trabalho. Gera pedidos e os processa.
 */
void simularDiaDeTrabalho(Estado *estado, Burger *cardapio, tp_listad *lista_loja, tp_listad *lista_hamb) {

	FilaPedidos pedidos;
	inicializaFilaPedidos(&pedidos);

	printf("\n============================================\n");
	printf("           INICIANDO DIA %d\n", estado->dia);
	printf("============================================\n");

	// 1. Gerar Pedidos
	geraPedidos(&pedidos, estado->dia);
	imprimeFilaPedidos(&pedidos, cardapio);

	// 2. Processar Pedidos
    printf("\n--- Processando Pedidos ---\n");
    Pedido p_atual;
    
    while (desenfileiraPedido(&pedidos, &p_atual)) {
        // Encontra o burger no cardápio
        Burger *burger_pedido = NULL;
        for (int i = 0; i < MAX_BURGERS; i++) {
            if (cardapio[i].id == p_atual.burg_id) {
                burger_pedido = &cardapio[i];
                break;
            }
        }

        if (burger_pedido == NULL) {
             printf("Pedido %d: [ERRO] Burger ID %d nao encontrado!\n", p_atual.id, p_atual.burg_id);
             continue;
        }
        
        printf("Preparando Pedido %d: %s\n", p_atual.id, burger_pedido->nome);
        
        // --- LÓGICA DE CONSUMO DE ESTOQUE ---
        // A função prepararBurger agora usa a pilha dinâmica (PilhaLE)
        
        if (prepararBurger(burger_pedido, lista_loja, lista_hamb)) {
            // Sucesso!
            printf("Pedido %d (%s) concluido! Voce ganhou R$ %.2f\n", p_atual.id, burger_pedido->nome, burger_pedido->preco);
            estado->dinheiro += burger_pedido->preco;
        } else {
            // Falha (prepararBurger já imprimiu a falha de estoque)
            printf("Pedido %d (%s) CANCELADO por falta de ingredientes!\n", p_atual.id, burger_pedido->nome);
            // TODO: Penalidade por falha?
        }
    }

	// 3. Fim do Dia
	printf("\n--- FIM DO DIA %d ---\n", estado->dia);
    printf("Dinheiro atual: R$ %.2f\n", estado->dinheiro);
	estado->dia++;

    // 4. Reposição da Loja
    simular_reposicao_loja(lista_loja);

	printf("\nPressione ENTER para continuar...");
	getchar(); // Limpa o buffer
    getchar(); // Espera o usuário
}


// --- Função Principal e Menu ---

/*
 * Esta é a ÚNICA função 'main' que o projeto deve ter.
 * Ela NÃO chama 'gameplayLoop()'.
 */
int main() {
    srand(time(NULL));
    
    // --- Inicialização dos Módulos do Jogo ---
	Burger cardapio[MAX_BURGERS];
	inicializarCardapio(cardapio); // Usa PilhaLE
	
    Estado estado_jogo;
    
    /* A função 'inicializarEstado' agora está definida neste arquivo */
	inicializarEstado(&estado_jogo); // Define dia=1, dinheiro=100
    // -----------------------------------------
    
    
    // --- Inicialização do Sistema de Loja ---
    tp_listad *estoque_loja = inicializa_listad();
    tp_listad *estoque_hamburgueria = inicializa_listad(); 

    // Inicializa as listas com o estoque inicial do catálogo
    for(int i = 0; i < MAX_PRODUTOS; i++) {
        gerenciar_lista_dinamica(estoque_loja, &catalogo[i], 1);
        gerenciar_lista_dinamica(estoque_hamburgueria, &catalogo[i], 2);
    }
    printf("Pato Burger iniciado. Bem-vindo!\n");
    // ----------------------------------------


    int opcao, id, quantidade;

    do {
		#ifdef _WIN32 // Limpa o terminal
		system("cls");
		#else
		system("clear");
		#endif

        printf("\n--- PATO BURGER - Gerenciamento --- Dia: %d | Dinheiro: R$%.2f ---\n", estado_jogo.dia, estado_jogo.dinheiro);
        printf("\n--- LOJA E ESTOQUE ---\n");
        printf("1. Listar Estoque ATIVO da LOJA (Para comprar)\n");
        printf("2. Listar meus Ingredientes (Estoque da Hamburgueria)\n");
        printf("3. COMPRAR Ingredientes (Loja -> Hamburgueria)\n");
        
        printf("\n--- JOGO ---\n");
        printf("4. Iniciar o Dia de Trabalho (Processar Pedidos)\n");
        printf("5. Ver Cardapio\n");

        printf("\n0. Sair do Jogo\n");
        printf("Escolha uma opcao: ");
        
        if (scanf("%d", &opcao) != 1) {
             while(getchar() != '\n'); // Limpa buffer em caso de input invalido
             opcao = -1; 
        }

        switch (opcao) {
            case 1:
                listar_estoque_loja(estoque_loja);
                break;
                
            case 2:
                listar_estoque_hamburgueria(estoque_hamburgueria);
                break;

            case 3:
                printf("\n--- Comprar Ingredientes ---\n");
                listar_estoque_loja(estoque_loja);
                printf("ID do Produto a comprar (1 a %d): ", MAX_PRODUTOS);
                if (scanf("%d", &id) != 1) {
                    printf("[ERRO] ID invalido.\n");
                    while(getchar() != '\n');
                    break;
                }
                
                printf("Quantidade a comprar: ");
                if (scanf("%d", &quantidade) != 1) {
                    printf("[ERRO] Quantidade invalida.\n");
                    while(getchar() != '\n');
                    break;
                }
                
                comprar_para_hamburgueria(estoque_loja, estoque_hamburgueria, id, quantidade);
                break;

            case 4:
                simularDiaDeTrabalho(&estado_jogo, cardapio, estoque_loja, estoque_hamburgueria);
                continue; // Evita o "Pressione Enter" do final do loop
                
            case 5:
                exibirCardapio(cardapio); // Usa PilhaLE
                break;

            case 0:
                printf("Saindo do sistema. Destruindo listas de estoque.\n");
                estoque_loja = Destroi_listad(estoque_loja);
                estoque_hamburgueria = Destroi_listad(estoque_hamburgueria);
                // TODO: Destruir as pilhas de ingredientes nos cardapios
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

        if (opcao != 0 && opcao != 4) { // Pausa para o usuário ler a saída
            printf("\nPressione ENTER para voltar ao menu...");
            getchar(); // Limpa o buffer
            getchar(); // Espera o usuário
        }

    } while (opcao != 0);

    return 0;
}