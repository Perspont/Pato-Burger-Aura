#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   
#include <string.h>
#include "../Header/listade.h"

// --- Definição das Estruturas e Constantes ---

// Dados do Produto - A ID será a 'info' na sua lista
typedef struct {
    int id;
    char nome[50];
    int quantidade_loja; // Estoque DISPONÍVEL NA LOJA (para compra)
    int quantidade_hamb; // Estoque NA HAMBURGUERIA (ingredientes comprados)
    float preco;
    int estoque_max_loja; // Máximo que a Loja pode ter
} Produto;

// Simulação de um catálogo de produtos com os ingredientes solicitados.
#define MAX_PRODUTOS 13 // Total de produtos
Produto catalogo[MAX_PRODUTOS] = {
    // ID, Nome, Qtd_Loja_Inicial, Qtd_Hamb_Inicial, Preço, Max_Loja
    {1, "Pao", 50, 0, 2.00, 100},
    {2, "Carne", 30, 0, 5.00, 100},
    {3, "Queijo", 40, 0, 3.00, 100},
    {4, "Alface", 20, 0, 4.00, 100},
    {5, "Tomate", 20, 0, 3.00, 100},
    {6, "Bacon", 15, 0, 4.00, 100},
    {7, "Picles", 10, 0, 4.00, 100},
    {8, "Cebola", 25, 0, 3.00, 100},
    {9, "Falafel", 5, 0, 7.00, 50},
    {10, "Molho do Pato", 10, 0, 6.00, 50},
    {11, "Onion Rings", 10, 0, 4.00, 50},
    {12, "Maionese", 30, 0, 3.00, 100},
    {13, "Frango", 15, 0, 4.00, 75}
};


// --- Funções Auxiliares ---

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
    const char *nome_estoque = (tipo_estoque == 1) ? "ESTOQUE DA LOJA" : "ESTOQUE DA HAMBURGUERIA";

    if (quantidade > 0 && no == NULL) {
        // Se o estoque > 0 e o item NAO esta na lista, adiciona o ID
        insere_listad_no_fim(lista, p->id);
        printf("[INFO] Produto '%s' adicionado a lista de %s.\n", p->nome, nome_estoque);
    } else if (quantidade == 0 && no != NULL) {
        // Se o estoque = 0 e o item ESTA na lista, remove o ID
        remove_listad(lista, p->id);
        printf("[INFO] Produto '%s' removido da lista de %s (Estoque zerado).\n", p->nome, nome_estoque);
    }
}

// --- Funções do Sistema de Loja e Hamburgueria ---

/**
 * @brief Move itens do Estoque da Loja para o Estoque da Hamburgueria.
 * O delta pode ser negativo (usar/vender) ou positivo (comprar/adicionar).
 */
void modificar_estoque(tp_listad *lista_loja, tp_listad *lista_hamb, int id, int delta_loja, int delta_hamb) {
    Produto *p = busca_produto_por_id(id);

    if (p == NULL) {
        printf("[ERRO] Produto com ID %d nao encontrado.\n", id);
        return;
    }

    // --- LOGICA DA LOJA ---
    int nova_qtd_loja = p->quantidade_loja + delta_loja;
    
    if (nova_qtd_loja < 0) {
        printf("[ERRO] Nao ha estoque suficiente na LOJA (apenas %d disponiveis).\n", p->quantidade_loja);
        return;
    }
    if (nova_qtd_loja > p->estoque_max_loja) {
        printf("[AVISO] Estoque maximo da LOJA (%d) atingido para %s.\n", p->estoque_max_loja, p->nome);
        nova_qtd_loja = p->estoque_max_loja;
    }
    p->quantidade_loja = nova_qtd_loja;

    // --- LOGICA DA HAMBURGUERIA ---
    int nova_qtd_hamb = p->quantidade_hamb + delta_hamb;
    
    if (nova_qtd_hamb < 0) {
         printf("[ERRO] Nao ha estoque suficiente na HAMBURGUERIA (apenas %d disponiveis).\n", p->quantidade_hamb);
         return;
    }
    p->quantidade_hamb = nova_qtd_hamb;
    
    // 1. Gerencia a lista da Loja
    gerenciar_lista_dinamica(lista_loja, p, 1);
    
    // 2. Gerencia a lista da Hamburgueria
    gerenciar_lista_dinamica(lista_hamb, p, 2);
}


/**
 * @brief Funcao wrapper para compra (Loja -> Hamburgueria).
 */
void comprar_para_hamburgueria(tp_listad *lista_loja, tp_listad *lista_hamb, int id, int quantidade) {
    Produto *p = busca_produto_por_id(id);

    if (p == NULL) return;

    // Compra: Diminui LOJA (delta_loja = -quantidade) e Aumenta HAMBURGUERIA (delta_hamb = +quantidade)
    modificar_estoque(lista_loja, lista_hamb, id, -quantidade, quantidade);
    
    // Se a modificacao foi bem sucedida (sem erro de falta de estoque), imprime a transacao
    if (quantidade <= p->quantidade_loja + quantidade) { // Verifica se a transacao foi autorizada
        printf("[SUCESSO] Compra de %d unidades de %s realizada. Custo: R$ %.2f\n", 
               quantidade, p->nome, (quantidade * p->preco));
    }
}


/**
 * @brief Lista um estoque específico (Loja ou Hamburgueria)
 */
void listar_estoque(tp_listad *lista, int tipo_estoque) {
    const char *titulo = (tipo_estoque == 1) ? "ESTOQUE DA LOJA (Disponível para Compra)" : "ESTOQUE DA HAMBURGUERIA (Ingredientes)";
    
    if (lista == NULL || listad_vazia(lista)) {
        printf("\n[Estoque] %s: Lista vazia.\n", titulo);
        return;
    }

    printf("\n--- %s ---\n", titulo);
    printf("| %-3s | %-20s | %-7s | %-5s |\n", "ID", "Ingrediente", "QTD", "Preco");
    printf("|-----|----------------------|---------|-------|\n");

    tp_no *atu = lista->ini;
    while (atu != NULL) {
        Produto *p = busca_produto_por_id(atu->info);
        if (p != NULL) {
            int quantidade = (tipo_estoque == 1) ? p->quantidade_loja : p->quantidade_hamb;
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
        
        // Define uma variação aleatória para a reposição (20% a 70% do máximo)
        int min_reposicao = p->estoque_max_loja / 5; 
        int max_reposicao = p->estoque_max_loja * 7 / 10; 
        
        if (min_reposicao > max_reposicao) min_reposicao = max_reposicao;
        
        int reposicao_aleatoria = (rand() % (max_reposicao - min_reposicao + 1)) + min_reposicao;
        
        // Adiciona a reposição, garantindo que não ultrapasse o máximo
        int reposicao_efetiva = reposicao_aleatoria;
        int nova_quantidade = p->quantidade_loja + reposicao_aleatoria;

        if (nova_quantidade > p->estoque_max_loja) {
             reposicao_efetiva = p->estoque_max_loja - p->quantidade_loja;
             nova_quantidade = p->estoque_max_loja;
        }

        if (reposicao_efetiva > 0) {
            p->quantidade_loja = nova_quantidade;
            printf("Rep. %-20s: Reposicao de %d unidades. Novo estoque na Loja: %d\n", p->nome, reposicao_efetiva, p->quantidade_loja);
        } else if (p->quantidade_loja == 0) {
             printf("Rep. %-20s: Sem estoque na Loja e sem reposicao hoje.\n", p->nome);
        }
        
        gerenciar_lista_dinamica(lista_loja, p, 1);
    }
    printf("----------------------------------------------------\n");
    listar_estoque(lista_loja, 1);
}


// --- Função Principal e Menu ---

int main() {
    srand(time(NULL));
    
    // 1. Lista Dinâmica 1: Estoque da LOJA (O que a Hamburgueria pode comprar)
    tp_listad *estoque_loja = inicializa_listad();

    // 2. Lista Dinâmica 2: Estoque da HAMBURGUERIA (Ingredientes comprados)
    tp_listad *estoque_hamburgueria = inicializa_listad(); 

    // Inicializa a lista da loja com o estoque inicial do catálogo
    for(int i = 0; i < MAX_PRODUTOS; i++) {
        if (catalogo[i].quantidade_loja > 0) {
            insere_listad_no_fim(estoque_loja, catalogo[i].id);
        }
    }
    printf("Sistema iniciado. Estoque da Hamburgueria esta inicialmente vazio.\n");


    int opcao, id, quantidade;

    do {
        printf("\n--- Menu de Gerenciamento do Jogo ---\n");
        printf("1. Listar Estoque ATIVO da LOJA\n"); //IMPORTANTE.
        printf("2. Listar Ingredientes da HAMBURGUERIA\n");
        printf("3. COMPRAR Ingredientes (Loja -> Hamburgueria)\n"); //IMPORTANTE.
        printf("4. Simular Producao (Usa Ingredientes - Em desenvolvimento)\n");
        printf("5. Passar o Dia (Nova Reposicao Aleatoria da Loja)\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
             while(getchar() != '\n');
             opcao = -1; 
        }

        switch (opcao) {
            case 1:
                listar_estoque(estoque_loja, 1);
                break;
                
            case 2:
                listar_estoque(estoque_hamburgueria, 2);
                break;

            case 3:
                printf("\n--- Comprar Ingredientes ---\n");
                listar_estoque(estoque_loja, 1);
                printf("ID do Produto a comprar (1 a %d): ", MAX_PRODUTOS);
                scanf("%d", &id);
                if (!produto_existe(id)) {
                    printf("[ERRO] ID de produto invalido.\n");
                    break;
                }
                printf("Quantidade a comprar: ");
                scanf("%d", &quantidade);
                if (quantidade < 1) {
                    printf("[ERRO] A quantidade deve ser maior que zero.\n");
                    break;
                }
                comprar_para_hamburgueria(estoque_loja, estoque_hamburgueria, id, quantidade);
                break;

            case 4:
                // Aqui entraria a função de montar o hamburguer
                printf("\n--- Simular Producao ---\n");
                printf("Funcao em desenvolvimento. Voce usaria ingredientes do Estoque da Hamburgueria aqui.\n");
                
                break;
                
            case 5:
                // Aqui entraria o sistema de passar  o dia
                simular_reposicao_loja(estoque_loja);
                break;

            case 0: 
                printf("Saindo do sistema. Destruindo listas de estoque.\n");
                estoque_loja = Destroi_listad(estoque_loja);
                estoque_hamburgueria = Destroi_listad(estoque_hamburgueria);
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}