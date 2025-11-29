#ifndef GAMEPLAYETAPA2_H
#define GAMEPLAYETAPA2_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../Header/burgerLE.h"
#include "../Header/loja.h"
#include "../Header/filaLE.h"

#define MAX_COMMAND_LENGTH 50
#define MAX_BURGER_STACK 20
#define GRILL_TIME_MS 5000     //5 segundos para grelhar o hambúrguer.
#define GAME_DURATION_MS 180000 //3 minutos (3 * 60 * 1000)
#define TEMPO_PARA_NOTIFICACAO_MS 3000 //3 segundos na tela (Para as notificações de hambúrguer vazio e sem pedidos).



/**
 * @brief Possui o contexto para o jogo, como Handles pro console e o buffer de input.
 *
 *"a task context is the minimal set of data used by a task
 *(which may be a process, thread, or fiber) that must be saved to allow a task to be interrupted, and later continued from the same point" - Wikipedia sobre "Task Context".
 */
typedef struct
{
    HANDLE hConsoleIn;    //Handle para o input buffer do terminal.
    HANDLE hConsoleOut;   //Handle para o screen buffer [ativo] do terminal.
    COORD screenSize;     //Tamanho atual da tela ([COORD] -> "Defines the coordinates of a character cell in a console screen buffer. The origin of the coordinate system (0,0) is at the top, left cell of the buffer". - Microsoft).
    CHAR_INFO *charBuffer; //Buffer off-screen.
} GameContext;

/**

  Possui variáveis para o display de pedidos.

**/
typedef struct {
    char text[30];
    ULONGLONG spawnTime;
    int id_burger;
} pedidoDisplay;

#define MAX_PEDIDOS_DISPLAY 10 //Máximo de strings possíveis em tela.

/**
 * @brief Holds the game's logical state, like inventory, timers, and scores.
 */
typedef struct
{
    // Inventory
    int dinheiro;
    int pao_count;
    int hamburguerCru_count;
    int alface_count;
    int tomate_count;
    int queijo_count;
    int hamburguerGrelhado_count;
    // --- NOVOS INGREDIENTES ADICIONADOS ---
    int bacon_count;
    int maioneseDoPato_count;
    int onion_rings_count;
    int cebola_count;
    int picles_count;
    int falafel_count;
    int frango_count;
	int totalHamburgueresVendidos; // adicionado para o save
    // Pedido atual.
    char *PilhaDeHamburguerLE_display[MAX_BURGER_STACK]; //Pilha de hambúrguer (Em texto).
    int stackSize;
    BurgerLE_Player burgerPlayer; //Hambúrguer do player.


    // Fila pros pedidos.
    FilaLEPedidos filaDePedidos; //Fila de pedidos.
    FilaLEPedidos filaAtiva; //Fila com os pedidos ativos (Que já apareceram na tela).
    int ordersPending;
    int hamburguerVazio;
    int semPedidos;
    ULONGLONG tempoDeNotificacao;

    // Grilling State
    BOOL isGrilling;
    ULONGLONG grillStartTime;
    ULONGLONG tempoDoJogo; //Timer para o jogo. São sempre 3 minutos.

    //Para o display e timing dos pedidos.
    pedidoDisplay pedidosDisplay[MAX_PEDIDOS_DISPLAY];
    int contadorDisplayPedidos;
    ULONGLONG ultimoSpawnDisplayPedidos;
    int spawnCycleCount;
    float tempoDeSpawnPedidos;

    int totalPedidosNoDia; //Número de pedidos do dia.

    // Input State
    char currentCommand[MAX_COMMAND_LENGTH]; //Input que jogador acabou de inserir (Enter).
    int commandLength; //Tamanho do input inserido.

    // Control
    BOOL isRunning;
    BOOL showEndScreen; //Flag para ativar a tela de "Game Over".
    BOOL showCardapio;  //Flag para ativar a tela de "cardapio" (Página 1).
    BOOL showCardapio_2; //Flag para a segunda página do cardápio.

    //Dia.
    int dia;

} GameState;

// --- Forward Declarations ---
// We need to declare these functions up here so main and others can find them.
void initializeGame(GameContext *ctx, GameState *state);
void clearStack(GameState *state);
void resizeBuffer(GameContext *ctx, int width, int height);
void drawTimer(GameContext *ctx, GameState *state);
void drawCardapioScreen(GameContext *ctx, GameState *state);
void desenharCardapio_pagina2(GameContext *ctx, GameState *state); // Declarar a nova função
void writeToBuffer(GameContext *ctx, int x, int y, const char *texto, WORD atributos);
void drawBox(GameContext *ctx, int left, int top, int right, int bottom, WORD attributes);
void drawOrders(GameContext *ctx, GameState *state, int left, int top, int right, int bottom);
void drawInput(GameContext *ctx, GameState *state, int left, int top, int right, int bottom);
void drawGrilling(GameContext *ctx, GameState *state, int left, int top, int right, int bottom);
void drawIngredientes(GameContext *ctx, GameState *state, int left, int top, int right, int bottom);
void drawPilhaDeHamburguerLE_display(GameContext *ctx, GameState *state, int left, int top, int right, int bottom);
void blitToScreen(GameContext *ctx);
void drawEndScreen(GameContext *ctx, GameState *state);
void empilharIngrediente_display(GameState *state, const char *item, int *inventory);
void processCommand(GameContext *ctx, GameState *state);
void processInput(GameContext *ctx, GameState *state);
void renderGame(GameContext *ctx, GameState *state);
void initializeNextDay(GameState *state);
BOOL runEndScreen(GameContext *ctx, GameState *state);
void cleanup(GameContext *ctx, GameState *state);
void salvarJogo(GameState *state);
void carregarJogo(GameState *state);
void telaPrincipalEtapa2();



#endif
