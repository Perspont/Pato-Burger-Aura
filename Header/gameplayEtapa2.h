#ifndef GAMEPLAYETAPA2_H
#define GAMEPLAYETAPA2_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../Header/burgerLE.h"
#include "../Header/loja.h"
#include "../Header/filaLE.h"
#include "../Header/historico.h" 

#define MAX_COMMAND_LENGTH 50
#define MAX_BURGER_STACK 20
#define GRILL_TIME_MS 5000     
#define GAME_DURATION_MS 180000 
#define TEMPO_PARA_NOTIFICACAO_MS 3000 

typedef struct
{
    HANDLE hConsoleIn;    
    HANDLE hConsoleOut;   
    COORD screenSize;     
    CHAR_INFO *charBuffer; 
} GameContext;

typedef struct {
    char text[30];
    ULONGLONG spawnTime;
    int id_burger;
} pedidoDisplay;

#define MAX_PEDIDOS_DISPLAY 10 

typedef struct
{
    // Inventario (Estoque atual)
    int dinheiro;
    int pao_count;
    int hamburguerCru_count;
    int alface_count;
    int tomate_count;
    int queijo_count;
    int hamburguerGrelhado_count;
    
    int bacon_count;
    int maioneseDoPato_count;
    int onion_rings_count;
    int cebola_count;
    int picles_count;
    int falafel_count;
    int frango_count;

    // Estatisticas de Venda (Para o relatorio diario)
    int pao_vendidos;
    int alface_vendidos;
    int tomate_vendidos;
    int queijo_vendidos;
    int hamburguer_vendidos;
    int bacon_vendidos;
    int maioneseDoPato_vendidos;
    int onion_rings_vendidos;
    int cebola_vendidos;
    int picles_vendidos;
    int falafel_vendidos;
    int frango_vendidos; 
    
    // Contadores Gerais
	int totalHamburgueresVendidos; 
    int vendasNoDiaAtual;          
    
    char *PilhaDeHamburguerLE_display[MAX_BURGER_STACK]; 
    int stackSize;
    BurgerLE_Player burgerPlayer; 

    // Filas
    FilaLEPedidos filaDePedidos; 
    FilaLEPedidos filaAtiva; 
    int ordersPending;
    int hamburguerVazio;
    int semPedidos;
    ULONGLONG tempoDeNotificacao;

    // Estado da Grelha
    BOOL isGrilling;
    ULONGLONG grillStartTime;
    ULONGLONG tempoDoJogo; 

    // Display de Pedidos
    pedidoDisplay pedidosDisplay[MAX_PEDIDOS_DISPLAY];
    int contadorDisplayPedidos;
    ULONGLONG ultimoSpawnDisplayPedidos;
    int spawnCycleCount;
    float tempoDeSpawnPedidos;

    int totalPedidosNoDia; 

    // Input
    char currentCommand[MAX_COMMAND_LENGTH]; 
    int commandLength; 

    // Controle
    BOOL isRunning;
    BOOL showEndScreen; 
    BOOL showCardapio;  
    BOOL showCardapio_2;
    int showInstructions;

    int dia;

    // Arvore de Historico.
    NoHistorico *historicoVendas;
    NO_AVL *raizIngredientes; //Ponteiro para a raiz da árvore de ingredientes do dia.

} GameState;


void initializeGame(GameContext *ctx, GameState *state);
void clearStack(GameState *state);
void resizeBuffer(GameContext *ctx, int width, int height);
WORD obterCorDoIngrediente(const char* nomeIngrediente);
void drawTimer(GameContext *ctx, GameState *state);
void drawCardapioScreen(GameContext *ctx, GameState *state);
void desenharCardapio_pagina2(GameContext *ctx, GameState *state); 
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
void salvarLogDiario(GameState *state); // Alterei para receber o state todo
void initializeNextDay(GameState *state);
int runEndScreen(GameContext *ctx, GameState *state);
void cleanup(GameContext *ctx, GameState *state);
void salvarJogo(GameState *state);
void carregarJogo(GameState *state);
void drawMainMenu(GameContext *ctx, int selectedOption);
int runMainMenu(GameContext *ctx);
void telaPrincipalEtapa2();
void updateGame(GameState *state); // Adicionei o prototipo que faltava

#endif