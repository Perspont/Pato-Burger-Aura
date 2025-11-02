//Aqui teremos o jogo principal. Isto inclui montagem de hamburgueres, tela, recebimento de pedidos, e sistema de passagem de dias INTERCONECTADO com o sistema de loja.

#include "../Header/gameplayEtapa2.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../Header/burgerLE.h"

// --- Game Constants ---
#define MAX_COMMAND_LENGTH 50
#define MAX_BURGER_STACK 20
#define GRILL_TIME_MS 5000     // 5 seconds to grill a patty
#define GAME_DURATION_MS 180000 // 3 minutes (3 * 60 * 1000)

// --- Game State Structures ---

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
 * @brief Holds one of the dynamic order strings ("pato", "guaxinim").
 */
typedef struct
{
    char text[10]; // "pato" or "guaxinim"
    ULONGLONG spawnTime;
} DynamicOrder;

#define MAX_DYNAMIC_ORDERS 10 // Max strings on screen (3 pato + 3 guaxinim is 6, 10 is safe)

/**
 * @brief Holds the game's logical state, like inventory, timers, and scores.
 */
typedef struct
{
    // Inventory
    int money;
    int bun_count;
    int patty_count;
    int lettuce_count;
    int tomato_count;
    int cheese_count;
    int grilled_patty_count;
    // --- NOVOS INGREDIENTES ADICIONADOS ---
    int bacon_count;
    int mayo_count;
    int onion_rings_count;
    int onion_count;
    int pickles_count;
    int falafel_count;
    int chicken_count;
    // --- FIM DA ADIÇÃO ---
																			//Substituir por arquivo externo de save.
    // Pedido atual.
    char *PilhaDeHamburguerLE_display[MAX_BURGER_STACK]; //Pilha de hambúrguer (Em texto).
    int stackSize;
    BurgerLE_Player burgerPlayer; //Hambúrguer do player.

    // Fila pros pedidos.
    int ordersPending;

    // Grilling State
    BOOL isGrilling;
    ULONGLONG grillStartTime;
    ULONGLONG gameStartTime; // Timer for the whole game

    // Dynamic Orders (pato/guaxinim)
    DynamicOrder dynamicOrders[MAX_DYNAMIC_ORDERS];
    int dynamicOrderCount;
    ULONGLONG lastDynamicOrderSpawn;
    BOOL nextIsPato; // To track the pato/guaxinim cycle
    int spawnCycleCount;

    // Input State
    char currentCommand[MAX_COMMAND_LENGTH]; //Input que jogador acabou de inserir (Enter).
    int commandLength; //Tamanho do input inserido.

    // Control
    BOOL isRunning;
    BOOL showEndScreen; //Flag para ativar a tela de "Game Over".
    BOOL showCardapio;  //Flag para ativar a tela de "cardapio" (Página 1).
    BOOL showCardapio_2; //Flag para a segunda página do cardápio.

} GameState;

// --- Forward Declarations ---
// We need to declare these functions up here so main and others can find them.
void initializeGame(GameContext *ctx, GameState *state);
void clearStack(GameState *state);
void resizeBuffer(GameContext *ctx, int width, int height);
void drawTimer(GameContext *ctx, GameState *state);
void drawCardapioScreen(GameContext *ctx, GameState *state);
void desenharCardapio_pagina2(GameContext *ctx, GameState *state); // Declarar a nova função


// --- Utility Functions ---

/**
 * @brief Resizes the global off-screen buffer (ctx->charBuffer).
 * Called when the console window size changes.
 * @param width New width of the console.
 * @param height New height of the console.
 */
void resizeBuffer(GameContext *ctx, int width, int height)
{
    if (ctx->charBuffer)
    {
        free(ctx->charBuffer);
    }
    ctx->screenSize.X = width;
    ctx->screenSize.Y = height;
    ctx->charBuffer = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * width * height);
    if (!ctx->charBuffer)
    {
        // Handle allocation failure
        printf("Failed to allocate screen buffer!");
        exit(1);
    }
}

/**
 * @brief Clears the off-screen buffer (fills with spaces).
 */
void clearBuffer(GameContext *ctx)
{
    if (!ctx->charBuffer) return;
    for (int i = 0; i < ctx->screenSize.X * ctx->screenSize.Y; ++i)
    {
        ctx->charBuffer[i].Char.AsciiChar = ' ';
        ctx->charBuffer[i].Attributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE; // Default: White
    }
}

/**
 * @brief: Escreve uma string para o buffer off-screen na posição (x, y).
 * @param (x): Coordenada no X.
 * @param (y): Coordenada no Y.
 * @param (text): String à ser escrita.
 * @param (attributes): Atributos do texto no console (No caso, cor).
 */
void writeToBuffer(GameContext *ctx, int x, int y, const char *texto, WORD atributos)
{
    if (!ctx->charBuffer || x < 0 || y < 0 || x >= ctx->screenSize.X || y >= ctx->screenSize.Y) //Se X/Y são: Menores do que 0 e/ou maiores que o tamanho da tela. Se não há array charBuffer.
    {
        return;
    }

    int len = strlen(texto);
    for (int i = 0; i < len; ++i)
    {
        int index = (y * ctx->screenSize.X) + x + i;
        if (x + i < ctx->screenSize.X)
        {
            ctx->charBuffer[index].Char.AsciiChar = texto[i]; //Letra do texto.
            ctx->charBuffer[index].Attributes = atributos; //Cor.
        }
    }
}

/**
 * @brief Draws a box in the off-screen buffer using line-drawing characters.
 */
void drawBox(GameContext *ctx, int left, int top, int right, int bottom, WORD attributes)
{
    if (right >= ctx->screenSize.X) right = ctx->screenSize.X - 1;
    if (bottom >= ctx->screenSize.Y) bottom = ctx->screenSize.Y - 1;
    if (left > right || top > bottom) return;

    // Draw corners
    writeToBuffer(ctx, left, top, "\xC9", attributes);       // Top-left
    writeToBuffer(ctx, right, top, "\xBB", attributes);      // Top-right
    writeToBuffer(ctx, left, bottom, "\xC8", attributes);    // Bottom-left
    writeToBuffer(ctx, right, bottom, "\xBC", attributes);   // Bottom-right

    // Draw horizontal lines
    for (int x = left + 1; x < right; ++x)
    {
        writeToBuffer(ctx, x, top, "\xCD", attributes);      // Top
        writeToBuffer(ctx, x, bottom, "\xCD", attributes);   // Bottom
    }

    // Draw vertical lines
    for (int y = top + 1; y < bottom; ++y)
    {
        writeToBuffer(ctx, left, y, "\xBA", attributes);     // Left
        writeToBuffer(ctx, right, y, "\xBA", attributes);    // Right
    }
}

// --- UI Drawing Functions ---

void drawOrders(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, " ORDERS PENDING ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    int drawX = left + 2;
    int drawY = top + 2;
    for (int i = 0; i < state->ordersPending; ++i)
    {
        if (drawX + 2 > right) // Wrap to next line if full
        {
            drawX = left + 2;
            drawY++;
            if (drawY >= bottom - 1) break; // Stop if box is full
        }
        writeToBuffer(ctx, drawX, drawY, "\xFE ", FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Green dot
        drawX += 2;
    }

    // --- NEW: Draw dynamic order strings ---
    // Start drawing *below* the last line of squares
    if (drawX > left + 2) // If we drew at least one square, move to the next line
    {
        drawY++;
    }

    // Set starting X back to the left
    drawX = left + 2;

    for (int i = 0; i < state->dynamicOrderCount; i++)
    {
        if (drawY >= bottom - 1) break; // Stop if box is full

        // Draw the string
        writeToBuffer(ctx, drawX, drawY, state->dynamicOrders[i].text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); // White

        drawY++; // Move to the next line for the next string
    }
    // --- End of NEW ---
}

void drawInput(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, " COMMAND INPUT ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    // Draw the command prompt and the user's current command
    char prompt[MAX_COMMAND_LENGTH + 3];
    snprintf(prompt, sizeof(prompt), "> %s", state->currentCommand);
    writeToBuffer(ctx, left + 2, top + 2, prompt, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    // Show a blinking cursor (simple alternating character)
    if ((GetTickCount64() / 500) % 2 == 0)
    {
        writeToBuffer(ctx, left + 4 + state->commandLength, top + 2, "_", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    }
}

void drawGrilling(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_RED | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, " GRILL STATION ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    int width = right - left - 4;
    if (width < 10) width = 10; // Minimum width for progress bar

    if (state->isGrilling)
    {
        ULONGLONG elapsed = GetTickCount64() - state->grillStartTime;
        float progress = (float)elapsed / GRILL_TIME_MS;
        if (progress > 1.0f) progress = 1.0f;

        int barLength = (int)(progress * width);

        char progressBar[256];
        memset(progressBar, 0, sizeof(progressBar));

        // Fill the progress bar
        for(int i = 0; i < barLength; i++) progressBar[i] = '\xDB';
        for(int i = barLength; i < width; i++) progressBar[i] = '\xB0';

        writeToBuffer(ctx, left + 2, top + 2, "Grilling patty...", FOREGROUND_RED | FOREGROUND_INTENSITY);
        writeToBuffer(ctx, left + 2, top + 4, progressBar, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else
    {
        writeToBuffer(ctx, left + 2, top + 2, "Grill is idle.", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
        writeToBuffer(ctx, left + 2, top + 3, "Type 'grill' to start.", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    }
}

void drawInventory(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_GREEN);
    writeToBuffer(ctx, left + 2, top, " INVENTORY & CASH ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    char text[64];
    int y = top + 2;

    snprintf(text, sizeof(text), "Money: $%d", state->money);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN);

    snprintf(text, sizeof(text), "Buns:   %d", state->bun_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Patties:%d", state->patty_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Lettuce:%d", state->lettuce_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Tomato: %d", state->tomato_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Cheese: %d", state->cheese_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    // --- NOVOS INGREDIENTES ADICIONADOS ---
    snprintf(text, sizeof(text), "Bacon:  %d", state->bacon_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Maionese:%d", state->mayo_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "O.Rings:%d", state->onion_rings_count); // Abreviação para caber
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Cebola: %d", state->onion_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Picles: %d", state->pickles_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Falafel:%d", state->falafel_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Frango: %d", state->chicken_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    // --- FIM DA ADIÇÃO ---

    snprintf(text, sizeof(text), "Grilled: %d", state->grilled_patty_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow
}

void drawPilhaDeHamburguerLE_display(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_GREEN | FOREGROUND_RED); // Brown-ish
    writeToBuffer(ctx, left + 2, top, " HAMBURGUER ATUAL ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    int y = bottom - 2; // Começa a desenhar o hamburguer de baixo pra cima.

    // Desenhar a pilha em ordem reversa, assim ela "sobe" a cada pedido.
    for (int i = 0; i < state->stackSize; ++i) //Loopa até desenhar toda a pilha na tela.
    {
        if (y <= top) break; // Se não houver mais espaço (Se top (topo da tela) for 0, então para quando y é menor ou igual a 0).

        char textoDoIngrediente_noHamburguer[64]; //Texto que aparecerá em tela do ingrediente empilhado no hamburguer.
        snprintf(textoDoIngrediente_noHamburguer, sizeof(textoDoIngrediente_noHamburguer), " - %s - ", state->PilhaDeHamburguerLE_display[i]);

        // Centrar o texto.
        int textLen = strlen(textoDoIngrediente_noHamburguer);
        int boxWidth = right - left;
        int textX = left + (boxWidth - textLen) / 2;
        if (textX < left + 1) textX = left + 1;

        writeToBuffer(ctx, textX, y, textoDoIngrediente_noHamburguer, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
        y--;
    }
}

/**
 * @brief Draws the main game timer.
 */
void drawTimer(GameContext *ctx, GameState *state)
{
    ULONGLONG elapsed = GetTickCount64() - state->gameStartTime;
    ULONGLONG remaining = (elapsed > GAME_DURATION_MS) ? 0 : (GAME_DURATION_MS - elapsed);

    int minutes = (int)(remaining / 60000);
    int seconds = (int)((remaining % 60000) / 1000);

    char timerText[32];
    snprintf(timerText, sizeof(timerText), " TIME: %02d:%02d ", minutes, seconds);

    int x = ctx->screenSize.X - (int)strlen(timerText) - 2; // Top-right corner
    int y = 1;

    WORD attributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; // Yellow
    if (remaining < 30000) // Under 30 seconds
    {
        // Blink red
        attributes = (GetTickCount64() / 500) % 2 == 0
            ? (FOREGROUND_RED | FOREGROUND_INTENSITY)
            : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    writeToBuffer(ctx, x, y, timerText, attributes);
}

/**
 * @brief Blitz the off-screen buffer to the console.
 */
void blitToScreen(GameContext *ctx)
{
    if (!ctx->charBuffer) return;

    // Copy the entire off-screen buffer to the active console buffer.
    COORD bufferSize = {ctx->screenSize.X, ctx->screenSize.Y};
    COORD bufferCoord = {0, 0};
    SMALL_RECT writeRegion = {0, 0, ctx->screenSize.X - 1, ctx->screenSize.Y - 1};

    WriteConsoleOutputA(
        ctx->hConsoleOut,
        ctx->charBuffer,
        bufferSize,
        bufferCoord,
        &writeRegion);
}

/*
 * @brief Desenha página 1 do cardápio.
 */
void drawCardapioScreen(GameContext *ctx, GameState *state)
{
    clearBuffer(ctx);

    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    WORD laranja = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Laranja
    WORD branco = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;
    WORD verde = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD amarelo = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    drawBox(ctx, 0, 0, width - 1, height - 1, laranja);
    writeToBuffer(ctx, (width - 16) / 2, 0, " CARDAPIO (1/2) ", amarelo);

    int x_col1 = 3;
    int x_col2 = (width / 2) + 1;
    int y = 3; // Linha inicial

    // --- Coluna 1 ---
    // 1. Bit and Bacon (16)
    writeToBuffer(ctx, x_col1, y++, "1. Bit and Bacon (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Bacon", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    // 2. Duck Cheese (16)
    writeToBuffer(ctx, x_col1, y++, "2. Duck Cheese (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Alface", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Tomate", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    // 3. Quackteirao (16)
    writeToBuffer(ctx, x_col1, y++, "3. Quackteirao (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Alface", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    // --- Coluna 2 ---
    y = 3; // Resetar Y para a segunda coluna

    // 4. Big Pato (27)
    writeToBuffer(ctx, x_col2, y++, "4. Big Pato (27)", verde);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Alface", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Alface", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    y += 2;

    // 5. Zero e Um (13)
    writeToBuffer(ctx, x_col2, y++, "5. Zero e Um (13)", verde);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    y += 2;

    // --- Navegação ---
    const char *exitCmd = "[V]oltar ao Jogo";
    writeToBuffer(ctx, (width / 2) - 20, (height - 2), exitCmd, branco);
    const char *nextCmd = "[P]roxima Pagina ->";
    writeToBuffer(ctx, (width / 2) + 5, (height - 2), nextCmd, branco);

    // Desenha o timer por cima! O jogo continua.
    drawTimer(ctx, state);

    blitToScreen(ctx);
}

/*
 * @brief Desenha página 2 do cardápio.
 */

void desenharCardapio_pagina2(GameContext *ctx, GameState *state)
{
    clearBuffer(ctx);

    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    WORD laranja = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Laranja
    WORD branco = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;
    WORD verde = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD amarelo = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    drawBox(ctx, 0, 0, width - 1, height - 1, laranja);
    writeToBuffer(ctx, (width - 16) / 2, 0, " CARDAPIO (2/2) ", amarelo);

    int x_col1 = 3;
    int x_col2 = (width / 2) + 1;
    int y = 3; // Linha inicial

    // --- Coluna 1 ---
    // 6. Chicken Duckey (21)
    writeToBuffer(ctx, x_col1, y++, "6. Chicken Duckey (21)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Maionese", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Frango", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Onion_Rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    // 7. Pato Sobre Rodas (24)
    writeToBuffer(ctx, x_col1, y++, "7. Pato Sobre Rodas (24)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Onion_Rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Onion_Rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    // 9. Pato Verde (21)
    writeToBuffer(ctx, x_col1, y++, "9. Pato Verde (21)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Maionese", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Picles", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Falafel", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;


    // --- Coluna 2 ---
    y = 3; // Resetar Y

    // 8. Recursivo (35)
    writeToBuffer(ctx, x_col2, y++, "8. Recursivo (35)", verde);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Tomate", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Maionese", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    y += 2;

    // 10. Pickles and MAYO! (25)
    if (y < height - 10) // Evitar estouro de buffer se a tela for pequena
    {
        writeToBuffer(ctx, x_col2, y++, "10. Pickles and MAYO! (25)", verde);
        writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "Picles", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "Maionese", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "Bacon", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "Picles", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "Maionese", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    }


    // --- Navegação ---
    const char *prevCmd = "<- pagina [A]nterior";
    writeToBuffer(ctx, (width / 2) - 20, (height - 2), prevCmd, branco);
    const char *exitCmd = "[V]oltar ao Jogo";
    writeToBuffer(ctx, (width / 2) + 5, (height - 2), exitCmd, branco);

    // Desenha o timer por cima! O jogo continua.
    drawTimer(ctx, state);

    blitToScreen(ctx);
}


// --- Game Logic ---

/**
 * @brief Sets the initial game state.
 */
void initializeGame(GameContext *ctx, GameState *state)
{
    // Set initial game state
    state->money = 100;
    state->bun_count = 20;
    state->patty_count = 10;
    state->lettuce_count = 30;
    state->tomato_count = 30;
    state->cheese_count = 30;
    state->grilled_patty_count = 0;
    // --- NOVOS INGREDIENTES ADICIONADOS ---
    state->bacon_count = 10;
    state->mayo_count = 30;
    state->onion_rings_count = 20;
    state->onion_count = 30;
    state->pickles_count = 30;
    state->falafel_count = 10;
    state->chicken_count = 10;
    // --- FIM DA ADIÇÃO ---
    state->stackSize = 0;
    state->ordersPending = 0; // Alterado de 5 para 0, para sincronizar com os patos/guaxinims
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->isRunning = TRUE;
    state->showEndScreen = FALSE;
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE; // Inicia na página 2 como falso

    // NEW: Initialize dynamic order state
    state->dynamicOrderCount = 0;
    state->lastDynamicOrderSpawn = GetTickCount64();
    state->nextIsPato = TRUE; // Start with "pato"
    state->spawnCycleCount = 0;
    for (int i = 0; i < MAX_DYNAMIC_ORDERS; i++)
    {
        state->dynamicOrders[i].spawnTime = 0;
        state->dynamicOrders[i].text[0] = '\0';
    }

    state->gameStartTime = GetTickCount64();

    // Get console handles
    ctx->hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    ctx->hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    ctx->charBuffer = NULL;

    // Set console mode to allow window/mouse/key events
    SetConsoleMode(ctx->hConsoleIn, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);

    // Get initial screen size and create buffer
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(ctx->hConsoleOut, &csbi);
    resizeBuffer(ctx, csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1);

    // Hide the blinking cursor in the active console
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
}

/**
 * @brief Adiciona ingrediente à pilha de hambúrguer.
 */
void stackItem(GameState *state, const char *item, int *inventory)
{
    if (state->stackSize >= MAX_BURGER_STACK)
    {
        // Tamanho máximo do hambúrguer.
        return;
    }
    if (*inventory > 0)
    {
        (*inventory)--;
        state->PilhaDeHamburguerLE_display[state->stackSize] = _strdup(item); // Note: _strdup is windows-specific, free later!
        state->stackSize++;

        state->burgerPlayer
    }
    else
    {
        // Out of stock
    }
}

/**
 * @brief Clears the current burger stack, freeing memory.
 */
void clearStack(GameState *state)
{
    for (int i = 0; i < state->stackSize; i++)
    {
        free(state->PilhaDeHamburguerLE_display[i]);
    }
    state->stackSize = 0;
}

/**
 * @brief Processa o que for escrito pelo player.
 */
void processCommand(GameState *state)
{
    if (_stricmp(state->currentCommand, "grelhar") == 0)
    {
        if (!state->isGrilling && state->patty_count > 0)
        {
            state->patty_count--;
            state->isGrilling = TRUE;
            state->grillStartTime = GetTickCount64();
        }
    }
    else if (_stricmp(state->currentCommand, "pao") == 0)
    {
        stackItem(state, "Pao", &state->bun_count);
    }
    else if (_stricmp(state->currentCommand, "alface") == 0)
    {
        stackItem(state, "Alface", &state->lettuce_count);
    }
    else if (_stricmp(state->currentCommand, "tomate") == 0)
    {
        stackItem(state, "Tomate", &state->tomato_count);
    }
    else if (_stricmp(state->currentCommand, "queijo") == 0)
    {
        stackItem(state, "Queijo", &state->cheese_count);
    }
    else if (_stricmp(state->currentCommand, "hamburguer") == 0)
    {
        stackItem(state, "Hamburguer Grelhado", &state->grilled_patty_count);
    }
    // --- NOVOS INGREDIENTES ADICIONADOS ---
    else if (_stricmp(state->currentCommand, "bacon") == 0)
    {
        stackItem(state, "Bacon", &state->bacon_count);
    }
    else if (_stricmp(state->currentCommand, "maionese") == 0)
    {
        stackItem(state, "Maionese do Pato", &state->mayo_count);
    }
    else if (_stricmp(state->currentCommand, "onion_rings") == 0)
    {
        stackItem(state, "Onion Rings", &state->onion_rings_count);
    }
    else if (_stricmp(state->currentCommand, "cebola") == 0)
    {
        stackItem(state, "Cebola", &state->onion_count);
    }
    else if (_stricmp(state->currentCommand, "picles") == 0)
    {
        stackItem(state, "Picles", &state->pickles_count);
    }
    else if (_stricmp(state->currentCommand, "falafel") == 0)
    {
        stackItem(state, "Falafel", &state->falafel_count);
    }
    else if (_stricmp(state->currentCommand, "frango") == 0)
    {
        stackItem(state, "Frango", &state->chicken_count);
    }
    // --- FIM DA ADIÇÃO ---
    else if (_stricmp(state->currentCommand, "servir") == 0)
    {
        if (state->stackSize > 0 && state->ordersPending > 0)
        {
            clearStack(state);
            state->ordersPending--;
            state->money += 10; // Simple reward
        }
    }
    else if (_stricmp(state->currentCommand, "lixo") == 0)
    {
        clearStack(state);
    }

    else if (_stricmp(state->currentCommand, "cardapio") == 0)
    {
        state->showCardapio = TRUE; // Abre na página 1
        state->showCardapio_2 = FALSE;
    }
    // --------------------
    else if (_stricmp(state->currentCommand, "sair") == 0)
    {
        state->isRunning = FALSE;
    }

    // Clear command buffer
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
}

/**
 * @brief Coordena os inputs (Teclado, Mouse, Redimensionamento da tela).
 */
void processInput(GameContext *ctx, GameState *state)
{
    DWORD numEvents = 0;
    GetNumberOfConsoleInputEvents(ctx->hConsoleIn, &numEvents);
    if (numEvents == 0)
    {
        return;
    }

    INPUT_RECORD *eventBuffer = (INPUT_RECORD *)malloc(sizeof(INPUT_RECORD) * numEvents);
    DWORD eventsRead = 0;
    ReadConsoleInputA(ctx->hConsoleIn, eventBuffer, numEvents, &eventsRead);

    for (DWORD i = 0; i < eventsRead; ++i)
    {
        switch (eventBuffer[i].EventType)
        {
        case KEY_EVENT:
            if (eventBuffer[i].Event.KeyEvent.bKeyDown)
            {
                char c = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;

                // --- ROTEAMENTO DE INPUT BASEADO NA CENA ---
                if (state->showCardapio)
                {
                    // Página 1 do Cardápio
                    if (c == 'v' || c == 'V')
                    {
                        state->showCardapio = FALSE; // Voltar ao jogo
                    }
                    else if (c == 'p' || c == 'P')
                    {
                        state->showCardapio = FALSE;
                        state->showCardapio_2 = TRUE; // Ir para Pág 2
                    }
                    // Ignora outras teclas
                }
                else if (state->showCardapio_2)
                {
                    // Página 2 do Cardápio
                    if (c == 'v' || c == 'V')
                    {
                        state->showCardapio_2 = FALSE; // Voltar ao jogo
                    }
                    else if (c == 'a' || c == 'A')
                    {
                        state->showCardapio_2 = FALSE;
                        state->showCardapio = TRUE; // Ir para Pág 1
                    }
                    // Ignora outras teclas
                }
                else
                {
                    // Lógica de input normal do jogo (fora do cardápio)
                    if (c == '\r') // Enter key
                    {
                        processCommand(state);
                    }
                    else if (c == '\b') // Backspace
                    {
                        if (state->commandLength > 0)
                        {
                            state->commandLength--;
                            state->currentCommand[state->commandLength] = '\0';
                        }
                    }
                    else if (isprint(c) && state->commandLength < MAX_COMMAND_LENGTH - 1)
                    {
                        state->currentCommand[state->commandLength] = c;
                        state->commandLength++;
                        state->currentCommand[state->commandLength] = '\0';
                    }
                }
            }
            break;
        // -------------------------------------------

        case WINDOW_BUFFER_SIZE_EVENT:
        {
            // Redimensionamento da tela.
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(ctx->hConsoleOut, &csbi);
            int newWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int newHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            if (newWidth != ctx->screenSize.X || newHeight != ctx->screenSize.Y)
            {
                resizeBuffer(ctx, newWidth, newHeight);
            }
            break;
        }

        // We don't care about mouse or focus events
        case MOUSE_EVENT:
        case FOCUS_EVENT:
            break;
        }
    }

    free(eventBuffer);
}

/**
 * @brief Updates the game state (e.g., grilling timer, game timer).
 */
void updateGame(GameState *state)
{
    // Check grill timer
    if (state->isGrilling)
    {
        ULONGLONG now = GetTickCount64();
        if (now - state->grillStartTime >= GRILL_TIME_MS)
        {
            state->isGrilling = FALSE;
            state->grilled_patty_count++;
        }
    }

    // --- NEW: Dynamic Order Logic ---
    ULONGLONG now = GetTickCount64();

    // 1. Check for 10-second lifetime expiry
    int i = 0;
    while (i < state->dynamicOrderCount)
    {
        if (now - state->dynamicOrders[i].spawnTime >= 10000) // 10 seconds
        {
            // Remove this order. Shift all subsequent orders down.
            for (int j = i; j < state->dynamicOrderCount - 1; j++)
            {
                state->dynamicOrders[j] = state->dynamicOrders[j + 1];
            }
            state->dynamicOrderCount--;
            // Don't increment 'i' since the next item is now at index 'i'
        }
        else
        {
            i++; // Move to the next item
        }
    }

    // 2. Check for 2-second spawn interval
    if (now - state->lastDynamicOrderSpawn >= 2000) // 2 seconds
    {
        state->lastDynamicOrderSpawn = now;

        // Remove oldest if count is 3 or more (to make space)
        if (state->dynamicOrderCount >= 3)
        {
            // This effectively removes the item at index 2 (the 3rd item),
            // as it will be overwritten by the shift.
            state->dynamicOrderCount = 2;
        }

        // Shift all existing orders down by one
        for (int j = state->dynamicOrderCount; j > 0; j--)
        {
            state->dynamicOrders[j] = state->dynamicOrders[j - 1];
        }

        // Add the new order at the top (index 0)
        state->dynamicOrders[0].spawnTime = now;

        if (state->nextIsPato)
        {
            strcpy(state->dynamicOrders[0].text, "pato");
            state->spawnCycleCount++;
            if (state->spawnCycleCount >= 3)
            {
                state->nextIsPato = FALSE;
                state->spawnCycleCount = 0; // Reset for guaxinim
            }
        }
        else // next is guaxinim
        {
            strcpy(state->dynamicOrders[0].text, "guaxinim");
            state->spawnCycleCount++;
            if (state->spawnCycleCount >= 3)
            {
                state->nextIsPato = TRUE;
                state->spawnCycleCount = 0; // Reset for pato
            }
        }

        // Increment count
        state->dynamicOrderCount++;
    }

    // 3. Sync ordersPending
    state->ordersPending = state->dynamicOrderCount;
    // --- End of NEW Logic ---


    // Check main game timer
    ULONGLONG elapsed = GetTickCount64() - state->gameStartTime;
    if (elapsed >= GAME_DURATION_MS)
    {
        state->showEndScreen = TRUE; // Trigger game over
    }
}

/**
 * @brief Draws all UI components to the off-screen buffer.
 */
void renderGame(GameContext *ctx, GameState *state)
{
    if (!ctx->charBuffer) return;

    clearBuffer(ctx);

    // --- Define UI Layout (Dynamically) ---
    // This is the core of the responsive UI. All positions are
    // relative to the screen size.

    // Area 2: Orders (Top-Left)
    int orderBoxL = 1;
    int orderBoxT = 1;
    int orderBoxR = ctx->screenSize.X / 3;
    int orderBoxB = ctx->screenSize.Y / 3;
    drawOrders(ctx, state, orderBoxL, orderBoxT, orderBoxR, orderBoxB);

    // Area 3: Input (Below Orders)
    int inputL = 1;
    int inputT = orderBoxB + 1;
    int inputR = orderBoxR;
    int inputB = inputT + 4;
    drawInput(ctx, state, inputL, inputT, inputR, inputB);

    // Area 4: Grilling (Below Input)
    int grillL = 1;
    int grillT = inputB + 1;
    int grillR = orderBoxR;
    int grillB = grillT + 6;
    drawGrilling(ctx, state, grillL, grillT, grillR, grillB);

    // Area 5: Inventory (Bottom-Left)
    int invL = 1;
    int invB = ctx->screenSize.Y - 2;
    int invT = max(grillB + 1, invB - 10); // At least 10 high, or fill space
    int invR = orderBoxR;
    drawInventory(ctx, state, invL, invT, invR, invB);

    // Area 1: Burger Stack (Right side)
    int stackL = orderBoxR + 2;
    int stackT = 1;
    int stackR = ctx->screenSize.X - 2;
    int stackB = ctx->screenSize.Y - 2;
    drawPilhaDeHamburguerLE_display(ctx, state, stackL, stackT, stackR, stackB);

    // Area 6: Timer (Top-Right)
    drawTimer(ctx, state);


    // --- Blit to Screen ---
    blitToScreen(ctx);
}

/**
 * @brief Draws the "Game Over" screen.
 */
void drawEndScreen(GameContext *ctx, GameState *state)
{
    clearBuffer(ctx);

    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;
    drawBox(ctx, 0, 0, width - 1, height - 1, FOREGROUND_RED | FOREGROUND_INTENSITY);

    char text[64];
    int y = height / 2 - 4;

    const char *title = "GAME OVER";
    writeToBuffer(ctx, (width - (int)strlen(title)) / 2, y, title, FOREGROUND_RED | FOREGROUND_INTENSITY);
    y += 3;

    snprintf(text, sizeof(text), "Final Score: $%d", state->money);
    writeToBuffer(ctx, (width - (int)strlen(text)) / 2, y, text, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    y += 4;

    const char *restart = "[R]estart";
    writeToBuffer(ctx, (width - (int)strlen(restart)) / 2, y, restart, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    y += 2;

    const char *exitCmd = "[E]xit";
    writeToBuffer(ctx, (width - (int)strlen(exitCmd)) / 2, y, exitCmd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    blitToScreen(ctx);
}

/**
 * @brief Handles the input loop for the "Game Over" screen.
 * @return Returns TRUE if the game should restart, FALSE if it should exit.
 */
BOOL runEndScreen(GameContext *ctx, GameState *state)
{
    BOOL inEndScreen = TRUE;
    while (inEndScreen)
    {
        // Draw the screen
        drawEndScreen(ctx, state);

        // Process input
        DWORD numEvents = 0;
        GetNumberOfConsoleInputEvents(ctx->hConsoleIn, &numEvents);
        if (numEvents > 0)
        {
            INPUT_RECORD *eventBuffer = (INPUT_RECORD *)malloc(sizeof(INPUT_RECORD) * numEvents);
            DWORD eventsRead = 0;
            ReadConsoleInputA(ctx->hConsoleIn, eventBuffer, numEvents, &eventsRead);

            for (DWORD i = 0; i < eventsRead; ++i)
            {
                switch (eventBuffer[i].EventType)
                {
                case KEY_EVENT:
                    if (eventBuffer[i].Event.KeyEvent.bKeyDown)
                    {
                        char c = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
                        if (c == 'r' || c == 'R')
                        {
                            // Restart
                            clearStack(state);
                            initializeGame(ctx, state); // Re-initialize the game
                            return TRUE; // Tell main loop to continue
                        }
                        if (c == 'e' || c == 'E' || c == 'q' || c == 'Q')
                        {
                            // Exit
                            return FALSE; // Tell main loop to stop
                        }
                    }
                    break;
                case WINDOW_BUFFER_SIZE_EVENT:
                {
                    // Handle window resize
                    CONSOLE_SCREEN_BUFFER_INFO csbi;
                    GetConsoleScreenBufferInfo(ctx->hConsoleOut, &csbi);
                    int newWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                    int newHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                    if (newWidth != ctx->screenSize.X || newHeight != ctx->screenSize.Y)
                    {
                        resizeBuffer(ctx, newWidth, newHeight);
                    }
                    break;
                }
                }
            }
            free(eventBuffer);
        }
        Sleep(33);
    }
    return FALSE; // Default to exit
}

/**
 * @brief Cleans up resources before exiting.
 */
void cleanup(GameContext *ctx, GameState *state)
{
    clearStack(state);
    if (ctx->charBuffer)
    {
        free(ctx->charBuffer);
    }

    // Restore cursor visibility
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);

    // Clear screen on exit
    system("cls");
    printf("Burger Boss exited. Thanks for playing!\n");
}


// --- Main Function ---
int telaPrincipalEtapa2()
{
    GameContext gameContext = {0};
    GameState gameState = {0};

    initializeGame(&gameContext, &gameState);

    while (gameState.isRunning)
    {
        if (gameState.showEndScreen)
        {
            // Este loop (runEndScreen) é bloqueante. Ele assume o controle
            // até que o usuário escolha 'R' ou 'E'.
            if (!runEndScreen(&gameContext, &gameState))
            {
                gameState.isRunning = FALSE; // Usuário escolheu sair
            }
            // O runEndScreen tem seu próprio Sleep()
        }
        else
        {
            // --- LOOP DE JOGO NÃO-BLOQUEANTE ---

            // 1. Processa input. A função agora sabe se deve
            //    ouvir comandos (jogo) ou teclas de navegação (cardápio).
            processInput(&gameContext, &gameState);

            // 2. Atualiza a lógica do jogo. Isso acontece
            //    independentemente de qual tela está sendo mostrada.
            //    (O timer e a grelha continuam)
            updateGame(&gameState);

            // 3. Decide qual cena desenhar.
            if (gameState.showCardapio)
            {
                // Desenha a tela do cardapio (Página 1).
                drawCardapioScreen(&gameContext, &gameState);
            }
            else if (gameState.showCardapio_2)
			{
            	// Desenha a tela do cardapio (Página 2).
                desenharCardapio_pagina2(&gameContext, &gameState);
			}
            else
            {
                // Desenha a tela principal do jogo
                renderGame(&gameContext, &gameState);
            }

            // Controla a taxa de quadros
            Sleep(33); // ~30 FPS
        }
    }

    // Cleanup
    cleanup(&gameContext, &gameState);

    return 0;
}