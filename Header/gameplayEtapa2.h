#ifndef GAMEPLAYETAPA2_H
#define GAMEPLAYETAPA2_H

// As dependências foram movidas para cá
#include <windows.h>
#include "loja.h" 
#include "burgerLE.h"

/*
 * ===================================================================
 * ESTRUTURAS DE ESTADO DO JOGO (MOVIDAS DE .C PARA .H)
 * ===================================================================
 */

// --- Game Constants ---
#define MAX_COMMAND_LENGTH 50
#define MAX_BURGER_STACK 20
#define GRILL_TIME_MS 5000     // 5 seconds to grill a patty
#define GAME_DURATION_MS 180000 // 3 minutes (3 * 60 * 1000)

/**
 * @brief Possui o contexto para o jogo (handles do console, etc.)
 */
typedef struct
{
    HANDLE hConsoleIn;
    HANDLE hConsoleOut;
    COORD screenSize;
    CHAR_INFO *charBuffer;
} GameContext;

/**
 * @brief Armazena um pedido dinâmico (pato/guaxinim)
 */
typedef struct
{
    char text[10];
    ULONGLONG spawnTime;
} DynamicOrder;

#define MAX_DYNAMIC_ORDERS 10

/**
 * @brief Armazena o estado lógico principal do jogo.
 */
typedef struct
{
    // --- Sistema de Loja e Inventário ---
    Loja loja;
    InventarioJogador inventario;

    // --- Itens intermediários ---
    int hamburguerCru_count;
    int hamburguerGrelhado_count;

    // --- NOVO: Rastreamento de Vendas ---
    int hamburgueresVendidos; 
    // ------------------------------------

    // Pedido atual
    char *PilhaDeHamburguerLE_display[MAX_BURGER_STACK];
    int stackSize;
    BurgerLE_Player burgerPlayer;

    // Fila de pedidos
    int ordersPending;

    // Estado da Grelha
    BOOL isGrilling;
    ULONGLONG grillStartTime;
    ULONGLONG gameStartTime;

    // Pedidos Dinâmicos
    DynamicOrder dynamicOrders[MAX_DYNAMIC_ORDERS];
    int dynamicOrderCount;
    ULONGLONG lastDynamicOrderSpawn;
    BOOL nextIsPato;
    int spawnCycleCount;

    // Estado do Input
    char currentCommand[MAX_COMMAND_LENGTH];
    int commandLength;

    // Controle
    BOOL isRunning;
    BOOL showEndScreen;
    BOOL showCardapio;
    BOOL showCardapio_2;

    // Dia
    int dia;

} GameState;


/*
 * ===================================================================
 * PROTÓTIPOS DE FUNÇÕES
 * ===================================================================
 */

// Função principal (chamada pelo main.c)
void telaPrincipalEtapa2(void);

// --- FUNÇÕES DE SAVE/LOAD ---

/**
 * @brief Salva o estado atual do jogo em "Save/savegame.txt".
 * @param state O estado atual do jogo.
 */
void salvarProgresso(GameState *state);

/**
 * @brief Carrega o estado do jogo de "Save/savegame.txt".
 * @param state O estado do jogo a ser preenchido.
 */
void carregarProgresso(GameState *state);

/**
 * @brief APAGA o arquivo "Save/savegame.txt" (para Novo Jogo).
 */
void apagarProgresso(void);


#endif // GAMEPLAYETAPA2_H