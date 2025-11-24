//Aqui teremos o jogo principal. Isto inclui montagem de hamburgueres, tela, recebimento de pedidos, e sistema de passagem de dias INTERCONECTADO com o sistema de loja.

#include "../Header/gameplayEtapa2.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../Header/burgerLE.h"
#include "../Header/loja.h"
#include "../Header/filaLE.h"
#include <ctype.h> //Analisar.




//Possui nome de cada hambúrguer

const char* getNomeDoBurger(int id) {
    switch (id) {
        case 1: return "Bit and Bacon";
        case 2: return "Duck Cheese";
        case 3: return "Quackteirao";
        case 4: return "Big Pato";
        case 5: return "Zero e Um";
        case 6: return "Chicken Duckey";
        case 7: return "Pato s/ Rodas";
        case 8: return "Recursivo";
        case 9: return "Pato Verde";
        case 10: return "Pickles & Mayo";
        default: return "Unknown";
    }
}

//----

/**
 * @Descrição: Resizes the global off-screen buffer (ctx->charBuffer).
 * Called when the console window size changes.
 * (width): New width of the console.
 * (height): New height of the console.
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
 * @Descrição: Clears the off-screen buffer (fills with spaces).
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
 * @Descrição: Escreve uma string para o buffer off-screen na posição (x, y).
 * (x): Coordenada no X.
 * (y): Coordenada no Y.
 * (text): String à ser escrita.
 * (attributes): Atributos do texto no console (No caso, cor).
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
 * @Descrição: Draws a box in the off-screen buffer using line-drawing characters.
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
    writeToBuffer(ctx, left + 2, top, "PEDIDOS PENDENTES ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    int drawX = left + 2;
    int drawY = top + 2;

//Analisar.

    //Iterar pela lista de pedidos, introduzindo-os 1 a 1.
    NoPedido_FilaLE *atual = state->filaDePedidos.inicio;
    int count = 0;

    while (atual != NULL && drawY < bottom - 1)
    {
        char nomeDoPedido[64];
        snprintf(nomeDoPedido, sizeof(nomeDoPedido), "%d. %s", count + 1, getNomeDoBurger(atual->info.id_burger));
        
        writeToBuffer(ctx, drawX, drawY, nomeDoPedido, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); // White
        
        drawY++;
        atual = atual->prox;
        count++;
    }

    if (state->filaDePedidos.tamanho == 0) {
         writeToBuffer(ctx, drawX, drawY, "Nenhum pedido.", FOREGROUND_RED | FOREGROUND_INTENSITY);
    }

//------
}

void drawInput(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, "INPUT ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    // Draw the command prompt and the user's current command
    char prompt[MAX_COMMAND_LENGTH + 3];
    snprintf(prompt, sizeof(prompt), "> %s", state->currentCommand);
    writeToBuffer(ctx, left + 2, top + 2, prompt, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    // Show a blinking cursor (simple alternating character)
    if (((ULONGLONG)GetTickCount() / 500) % 2 == 0)
    {
        writeToBuffer(ctx, left + 4 + state->commandLength, top + 2, "_", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    }
}

void drawGrilling(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_RED | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, "GRELHA ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    int width = right - left - 4;
    if (width < 10) width = 10; // Minimum width for progress bar

    if (state->isGrilling)
    {
        ULONGLONG elapsed = (ULONGLONG)GetTickCount() - state->grillStartTime;
        float progress = (float)elapsed / GRILL_TIME_MS;
        if (progress > 1.0f) progress = 1.0f;

        int barLength = (int)(progress * width);

        char progressBar[256];
        memset(progressBar, 0, sizeof(progressBar));

        // Fill the progress bar
        for(int i = 0; i < barLength; i++) progressBar[i] = '\xDB';
        for(int i = barLength; i < width; i++) progressBar[i] = '\xB0';

        writeToBuffer(ctx, left + 2, top + 2, "Grelhando hamburguer...", FOREGROUND_RED | FOREGROUND_INTENSITY);
        writeToBuffer(ctx, left + 2, top + 4, progressBar, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else
    {
        writeToBuffer(ctx, left + 2, top + 2, "Grelha está vazia.", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
        writeToBuffer(ctx, left + 2, top + 3, "Escreva 'grelhar' para grelhar um hamburguer.", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    }
}

void drawIngredientes(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_GREEN);
    writeToBuffer(ctx, left + 2, top, "INGREDIENTES E DINHEIRO ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    char text[64];
    int y = top + 2;

    snprintf(text, sizeof(text), "dinheiro: $%d", state->dinheiro);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN);

    snprintf(text, sizeof(text), "Pao:   %d", state->pao_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Hamburguer Cru:%d", state->hamburguerCru_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Alface:%d", state->alface_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Tomate: %d", state->tomate_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Queijo: %d", state->queijo_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    // --- NOVOS INGREDIENTES ADICIONADOS ---
    snprintf(text, sizeof(text), "Bacon:  %d", state->bacon_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Maionese:%d", state->maioneseDoPato_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Onion Rings:%d", state->onion_rings_count); // Abreviação para caber
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Cebola: %d", state->cebola_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Picles: %d", state->picles_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Falafel:%d", state->falafel_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Frango: %d", state->frango_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    // --- FIM DA ADIÇÃO ---

    snprintf(text, sizeof(text), "Hamburguer Grelhado: %d", state->hamburguerGrelhado_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow
}

void drawPilhaDeHamburguerLE_display(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    int y = height / 2 - 4;

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

    if (state->semPedidos) {
        const char *title = "Não há mais pedidos para entregar";
        writeToBuffer(ctx, ((width - (int)strlen(title)) / 2)+5, y, title, FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    else if (state->hamburguerVazio) {
        const char *title = "Não é possível servir um hambúrguer vazio";
        writeToBuffer(ctx, ((width - (int)strlen(title)) / 2)+5, y, title, FOREGROUND_RED | FOREGROUND_INTENSITY); //Posição provisória.
    }
}

/**
 * @brief Draws the main game timer.
 */
void drawTimer(GameContext *ctx, GameState *state)
{
    ULONGLONG elapsed = (ULONGLONG)GetTickCount() - state->tempoDoJogo;
    ULONGLONG remaining = (elapsed > GAME_DURATION_MS) ? 0 : (GAME_DURATION_MS - elapsed);

    int minutes = (int)(remaining / 60000);
    int seconds = (int)((remaining % 60000) / 1000);

    char timerText[32];
    snprintf(timerText, sizeof(timerText), " TEMPO: %02d:%02d ", minutes, seconds);

    int x = ctx->screenSize.X - (int)strlen(timerText) - 2; // Top-right corner
    int y = 1;

    WORD attributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; // Yellow
    if (remaining < 30000) // Under 30 seconds
    {
        // Blink red
        attributes = ((ULONGLONG)GetTickCount() / 500) % 2 == 0
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

    snprintf(text, sizeof(text), "Final Score: $%d", state->dinheiro);
    writeToBuffer(ctx, (width - (int)strlen(text)) / 2, y, text, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    y += 4;

    const char *restart = "[R]estart";
    writeToBuffer(ctx, (width - (int)strlen(restart)) / 2, y, restart, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    y += 2;

    const char *exitCmd = "[S]air";
    writeToBuffer(ctx, (width - (int)strlen(exitCmd)) / 2, y, exitCmd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    blitToScreen(ctx);
}


// --- Game Logic ---

/**
 * @brief Sets the initial game state.
 *
 * Pendências: Fazer isso carregar do save.
 */
void initializeGame(GameContext *ctx, GameState *state)
{
    // Set initial game state
    state->dinheiro = 100;
    state->pao_count = 20;
    state->hamburguerCru_count = 10;
    state->alface_count = 30;
    state->tomate_count = 30;
    state->queijo_count = 30;
    state->hamburguerGrelhado_count = 0;
    state->bacon_count = 10;
    state->maioneseDoPato_count = 30;
    state->onion_rings_count = 20;
    state->cebola_count = 30;
    state->picles_count = 30;
    state->falafel_count = 10;
    state->frango_count = 10;

    state->totalHamburgueresVendidos = 0; // inicializa a nova variável totalHamburgueresVendidos com o valor 0.
    
    inicializar_BurgerLE_Player(&state->burgerPlayer);

    //Sistema de fila de pedidos.
    inicializaFilaLEPedidos(&state->filaDePedidos);
    geraPedidos_FilaLE(&state->filaDePedidos, 1); // "initializeGame" roda apenas no 1o dia.


    state->stackSize = 0;
    state->ordersPending = 0; // Alterado de 5 para 0, para sincronizar com os patos/guaxinims
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->isRunning = TRUE;
    state->showEndScreen = FALSE;
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE; // Inicia na página 2 como falso.
    state->dia = 1; //Ler do save.
    state->hamburguerVazio = 0;
    state->semPedidos = 0;

    state->tempoDoJogo = (ULONGLONG)GetTickCount();

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

// Adiciona ingrediente à pilha de hambúrguer.
void empilharIngrediente_display(GameState *state, const char *item, int *inventory)
{
    if (state->stackSize >= MAX_BURGER_STACK)
    {
        // Tamanho máximo do hambúrguer.
        return;
    }
    if (*inventory > 0)
    {
        (*inventory)--;
        state->PilhaDeHamburguerLE_display[state->stackSize] = _strdup(item); // Dar free() em _stdrup().
        state->stackSize++;

//Analisar.

        if (strcmp(item, "Pao") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 1);
        }
        else if (strcmp(item, "Carne") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 2);
        }
        else if (strcmp(item, "Queijo") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 3);
        }
        else if (strcmp(item, "Alface") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 4);
        }
        else if (strcmp(item, "Tomate") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 5);
        }
        else if (strcmp(item, "Bacon") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 6);
        }
        else if (strcmp(item, "Picles") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 7);
        }
        else if (strcmp(item, "Cebola") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 8);
        }
        else if (strcmp(item, "Falafel") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 9);
        }
        else if (strcmp(item, "Maionese de Pato") == 0) { // Corrected name
            adicionarIngredienteLE(&state->burgerPlayer, 10);
        }
        else if (strcmp(item, "Onion Rings") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 11);
        }
        else if (strcmp(item, "Maionese") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 12);
        }
        else if (strcmp(item, "Frango") == 0) {
            adicionarIngredienteLE(&state->burgerPlayer, 13);
        }
        //state->burgerPlayer
    }
    else
    {
        // Sem estoque.
        return;
    }
}

//----

// Limpa o stack de hamburguer atual, limpando memória.
void clearStack(GameState *state)
{
    for (int i = 0; i < state->stackSize; i++)
    {
        free(state->PilhaDeHamburguerLE_display[i]);
    }

    deletaBurgerLE(&state->burgerPlayer);

    state->stackSize = 0;
}

/**
 * - Descrição: Processa o que for escrito pelo player.
 */
void processCommand(GameContext *ctx, GameState *state)
{
    if (_stricmp(state->currentCommand, "grelhar") == 0)
    {
        if (!state->isGrilling && state->hamburguerCru_count > 0)
        {
            state->hamburguerCru_count--;
            state->isGrilling = TRUE;
            state->grillStartTime = (ULONGLONG)GetTickCount();
        }
    }
    else if (_stricmp(state->currentCommand, "pao") == 0)
    {
        empilharIngrediente_display(state, "Pao", &state->pao_count);
    }
    else if (_stricmp(state->currentCommand, "alface") == 0)
    {
        empilharIngrediente_display(state, "Alface", &state->alface_count);
    }
    else if (_stricmp(state->currentCommand, "tomate") == 0)
    {
        empilharIngrediente_display(state, "Tomate", &state->tomate_count);
    }
    else if (_stricmp(state->currentCommand, "queijo") == 0)
    {
        empilharIngrediente_display(state, "Queijo", &state->queijo_count);
    }
    else if (_stricmp(state->currentCommand, "hamburguer") == 0)
    {
        empilharIngrediente_display(state, "Hamburguer Grelhado", &state->hamburguerGrelhado_count);
    }
    else if (_stricmp(state->currentCommand, "bacon") == 0)
    {
        empilharIngrediente_display(state, "Bacon", &state->bacon_count);
    }
    else if (_stricmp(state->currentCommand, "maionese") == 0)
    {
        empilharIngrediente_display(state, "Maionese do Pato", &state->maioneseDoPato_count);
    }
    else if (_stricmp(state->currentCommand, "onion_rings") == 0)
    {
        empilharIngrediente_display(state, "Onion Rings", &state->onion_rings_count);
    }
    else if (_stricmp(state->currentCommand, "cebola") == 0)
    {
        empilharIngrediente_display(state, "Cebola", &state->cebola_count);
    }
    else if (_stricmp(state->currentCommand, "picles") == 0)
    {
        empilharIngrediente_display(state, "Picles", &state->picles_count);
    }
    else if (_stricmp(state->currentCommand, "falafel") == 0)
    {
        empilharIngrediente_display(state, "Falafel", &state->falafel_count);
    }
    else if (_stricmp(state->currentCommand, "frango") == 0)
    {
        empilharIngrediente_display(state, "Frango", &state->frango_count);
    }

    else if (_stricmp(state->currentCommand, "servir") == 0)
    {
        if (state->stackSize > 0 && state->ordersPending > 0)
        {
            state->totalHamburgueresVendidos++; //Atualiza o save ANTES de limpar a pilha

            Pedido_FilaLE pedidoAtual;
            desenfileiraPedido_FilaLE(&state->filaDePedidos, &pedidoAtual); //Desenfileira pedido na frente da fila, e insere seu valor em pedidoAtual.

            //Verifica o id do Pedido atual, e cria com o hambúrguer necessário.
//Analisar.
            BurgerLE burgerPedido = {0}; //Inicializa no 0.

            switch (pedidoAtual.id_burger) { // Fix: Switch on burger ID, not order ID
                case 1:
                    inicializa_BitAndBacon_LE(&burgerPedido);
                    break;
                case 2:
                    inicializa_DuckCheese_LE(&burgerPedido);
                    break;
                case 3:
                    inicializa_Quackteirao_LE(&burgerPedido);
                    break;
                case 4:
                    inicializa_BigPato_LE(&burgerPedido);
                    break;
                case 5:
                    inicializa_ZeroUm_LE(&burgerPedido);
                    break;
                case 6:
                    inicializa_ChickenDuckey_LE(&burgerPedido);
                    break;
                case 7:
                    inicializa_PatoSobreRodas_LE(&burgerPedido);
                    break;
                case 8:
                    inicializa_Recursivo_LE(&burgerPedido);
                    break;
                case 9:
                    inicializa_PatoVerde_LE(&burgerPedido);
                    break;
                case 10:
                    inicializa_PicklesAndMayo_LE(&burgerPedido);
                    break;
                default:
                    // Handle unknown burger ID if necessary
                    break;
            }

            state->dinheiro += comparaHamburgueresLE(&state->burgerPlayer, &burgerPedido); // Comparar burger pedido com o do player, retornando moedas, e "deletando" os 2 (Do pedido é deletado, do player só esvaziado).
            
            clearStack(state); // Move clearStack to AFTER comparison
            state->ordersPending--;
        }
        else if (state->stackSize <= 0){ //Quando não há hambúrguer montado.
            state->hamburguerVazio = 1; //Exibir mensagem de hambúrguer vazio.
            state->tempoDeNotificacao = (ULONGLONG)GetTickCount();

        }
        else if (state->ordersPending <= 0) { //Quando acabam os pedidos.
            state->semPedidos = 1; //Exibir mensagem de sem pedido.
            state->tempoDeNotificacao = (ULONGLONG)GetTickCount(); //Inicia timer para a notificação.
        }
    } //-----
    else if (_stricmp(state->currentCommand, "lixo") == 0)
    {
        clearStack(state);
        deletaBurgerLE(&state->burgerPlayer); //Deleta hambúrguer do player (Apenas os ingredientes dentro dele).
    }

    else if (_stricmp(state->currentCommand, "cardapio") == 0)
    {
        state->showCardapio = TRUE; // Abre na página 1
        state->showCardapio_2 = FALSE;
    }

	else if (_stricmp(state->currentCommand, "salvar") == 0) // para salvar o jogo no input (manualmente).
    {
        salvarJogo(state);
        // Seria bom colocar um feedback visual para falar que o jogo foi salvo ou algo do tipo...
    }
    
    else if (_stricmp(state->currentCommand, "carregar") == 0) // para carregar o jogo atualmente salvo (manualmente tbm).
    {
        clearStack(state); // limpa o hamburguer atual.
        
        carregarJogo(state); // carrega os dados do save_game.txt

        // reinicia o estado do dia
        state->tempoDoJogo = (ULONGLONG)GetTickCount();
        state->isGrilling = FALSE;
        state->ordersPending = 0;

        // limpa a fila de pedidos antiga e gera uma nova para o dia que tiver sido salvo.
        while (state->filaDePedidos.tamanho != 0) {
            Pedido_FilaLE pedido;
            desenfileiraPedido_FilaLE(&state->filaDePedidos, &pedido);
        }
        geraPedidos_FilaLE(&state->filaDePedidos, state->dia); // recomeca a gerar pedidos e volta ao jogo normal.
    } // nota: para carregar o jogo, tem que limpar e reiniciar o progresso atual pelo salvo nos arquivos (se nn tiver save, reseta tudo).
    
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
                        processCommand(ctx, state);
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

//Analisar.

/**
 * @brief Atualiza o game state (timer da grelha, timer principal).
 *
 * Pendências: Colocar timer para mensagens de hambúrguer vazio e sem pedidos.
 */
void updateGame(GameState *state)
{

    // Check grill timer
    if (state->isGrilling)
    {
        ULONGLONG now = (ULONGLONG)GetTickCount();
        if (now - state->grillStartTime >= GRILL_TIME_MS)
        {
            state->isGrilling = FALSE;
            state->hamburguerGrelhado_count++;
        }
    }

    //Timer da notificação para quando jogador tenta servir sem pedidos.
    if (state->semPedidos) {

        ULONGLONG now = (ULONGLONG)GetTickCount();

        if (now - state->tempoDeNotificacao >= TEMPO_PARA_NOTIFICACAO_MS) {
            state->semPedidos = 0;
        }
    }

    //Timer da notificação para quando jogador tenta servir com hambúrguer vazio.
    if (state->hamburguerVazio) {

        ULONGLONG now = (ULONGLONG)GetTickCount();

        if (now - state->tempoDeNotificacao >= TEMPO_PARA_NOTIFICACAO_MS) {
            state->hamburguerVazio = 0;
        }
    }

    //Aqui que aparecem os pedidos.
    ULONGLONG now = GetTickCount64();

    // 1. Check for 10-second lifetime expiry
    int numeroDePedidosEmTela = 0;
    while (numeroDePedidosEmTela < state->contadorDisplayPedidos)
    {
        if (now - state->pedidosDisplay[numeroDePedidosEmTela].spawnTime >= 10000) // 10 seconds
        {
            // Remove this order. Shift all subsequent orders down.
            for (int j = numeroDePedidosEmTela; j < state->contadorDisplayPedidos - 1; j++)
            {
                state->pedidosDisplay[j] = state->pedidosDisplay[j + 1];
            }
            state->contadorDisplayPedidos--;
            // Don't increment 'i' since the next item is now at index 'i'
        }
        else
        {
            i++; // Move to the next item
        }
    }

    // 2. Check for 2-second spawn interval
    if (now - state->ultimoSpawnDisplayPedidos >= 2000) // 2 seconds
    {
        state->ultimoSpawnDisplayPedidos = now;

        // Remove oldest if count is 3 or more (to make space)
        if (state->contadorDisplayPedidos >= 3)
        {
            // This effectively removes the item at index 2 (the 3rd item),
            // as it will be overwritten by the shift.
            state->contadorDisplayPedidos = 2;
        }

        // Shift all existing orders down by one
        for (int j = state->contadorDisplayPedidos; j > 0; j--)
        {
            state->pedidosDisplay[j] = state->pedidosDisplay[j - 1];
        }

        // Add the new order at the top (index 0)
        state->pedidosDisplay[0].spawnTime = now;

        if (state->nextIsPato)
        {
            strcpy(state->pedidosDisplay[0].text, "pato");
            state->spawnCycleCount++;
            if (state->spawnCycleCount >= 3)
            {
                state->nextIsPato = FALSE;
                state->spawnCycleCount = 0; // Reset for guaxinim
            }
        }
        else // next is guaxinim
        {
            strcpy(state->pedidosDisplay[0].text, "guaxinim");
            state->spawnCycleCount++;
            if (state->spawnCycleCount >= 3)
            {
                state->nextIsPato = TRUE;
                state->spawnCycleCount = 0; // Reset for pato
            }
        }

        // Increment count
        state->contadorDisplayPedidos++;
    }

    // 3. Sync ordersPending
    state->ordersPending = state->filaDePedidos.tamanho;
    // --- End of NEW Logic ---

    // 1. Checa se o dinheiro acabou (Game Over imediato).
    if (state->dinheiro <= 0)
    {
        state->showEndScreen = TRUE;
        return; // Para atualizar o terminal, levando o player à tela de Game Over.
    }

    // Check main game timer
    ULONGLONG elapsed = (ULONGLONG)GetTickCount() - state->tempoDoJogo;
    if (elapsed >= GAME_DURATION_MS)
    {
        state->showEndScreen = TRUE; // Trigger game over
    }
}

//----

/**
 * @brief Draws all UI components to the off-screen buffer.
 */
void renderGame(GameContext *ctx, GameState *state)
{
    if (!ctx->charBuffer) return;

    clearBuffer(ctx);

    // --- Define UI Layout (Dynamically) ---
    // Lógica de cima para baixo com caixas de tamanho fixo no topo.
    
    int leftColR = ctx->screenSize.X / 3; // Borda direita da coluna esquerda

    // Area 2: Orders (Top-Left)
    int orderBoxL = 1;
    int orderBoxT = 1;
    int orderBoxR = leftColR;
    int orderBoxB = 8; // Altura fixa de 8 linhas
    drawOrders(ctx, state, orderBoxL, orderBoxT, orderBoxR, orderBoxB);

    // Area 3: Input (Below Orders)
    int inputL = 1;
    int inputT = orderBoxB + 1; // Começa abaixo de 'Orders'
    int inputR = leftColR;
    int inputB = inputT + 4;   // Altura fixa de 4 linhas
    drawInput(ctx, state, inputL, inputT, inputR, inputB);

    // Area 4: Grilling (Below Input)
    int grillL = 1;
    int grillT = inputB + 1; // Começa abaixo de 'Input'
    int grillR = leftColR;
    int grillB = grillT + 6;   // Altura fixa de 6 linhas
    drawGrilling(ctx, state, grillL, grillT, grillR, grillB);

    // Area 5: Inventory (Bottom-Left, preenche o resto)
    int invL = 1;
    int invB = ctx->screenSize.Y - 2; // Borda de baixo
    int invT = grillB + 1;            // Começa abaixo de 'Grill'
    int invR = leftColR;
    drawIngredientes(ctx, state, invL, invT, invR, invB); // Usa todo o espaço restante

    // Area 1: Burger Stack (Right side)
    int stackL = leftColR + 2;
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
 * @brief Reseta o estado para o início de um novo dia (sem resetar inventário/dinheiro).
 */
void initializeNextDay(GameState *state)
{
    // Reseta o estado do dia
    state->stackSize = 0;
    state->ordersPending = 0;
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->showEndScreen = FALSE; // Garante que não estamos na tela de game over
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE;
    state->hamburguerVazio = 0;
    state->semPedidos = 0;

    deletaBurgerLE(&state->burgerPlayer); //Reinicia hambúrguer do player (Deleta os ingredientes dentro dele).

    // Reseta o timer do jogo.
    state->tempoDoJogo = (ULONGLONG)GetTickCount();

    //Aumenta o dia em 1.
    state->dia++;


    //Deleta fila do dia anterior e popula nova fila.
    while (state->filaDePedidos.tamanho != 0) {
        Pedido_FilaLE pedido;

        desenfileiraPedido_FilaLE(&state->filaDePedidos, &pedido);
    }
    geraPedidos_FilaLE(&state->filaDePedidos, state->dia); //Gera nova fila pro dia atual.

    // NOTA: Não reseta dinheiro nem ingredientes.

    state->tempoDeSpawnPedidos = state->tempoDoJogo/state->filaDePedidos.tamanho; //Este é o tempo de spawn para cada pedido.
}

/**
 * @brief Handles the input loop for the "Game Over" screen.
 * @return Returns TRUE if the game should restart, FALSE if it should exit.
 */
BOOL runEndScreen(GameContext *ctx, GameState *state)
{
    BOOL inEndScreen = TRUE;

    deletaBurgerLE(&state->burgerPlayer); //Deleta hambúrguer do player (Apenas os ingredientes dentro dele).
                                          //No momento que você entra nessa tela, o jogo já acabou (Ou seja, é necessário deletar os itens do hambúrguer).
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
                                // Reinicia o jogo COMPLETAMENTE
                                clearStack(state);
                                initializeGame(ctx, state); // Chama o reset TOTAL
                                return TRUE; // Diz ao loop principal para continuar (no dia 1)
                            }
                        if (c == 'e' || c == 'E')
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
    printf("Obrigado por jogar!\n");
}

void salvarJogo(GameState *state) { // funcao de save do jogo
    FILE *arquivo; // ponteiro de arquivo generico
    
    // salva as moedas
    arquivo = fopen("Saves/coins.txt", "w"); //salvamento "w" para write and read (escrever e ler)
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", state->dinheiro);
        fclose(arquivo);
    }

    // salva os dias
    arquivo = fopen("Saves/days.txt", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", state->dia);
        fclose(arquivo);
    }

    // salva os ingredientes / inventario atual
    arquivo = fopen("Saves/ingredientes.txt", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d\n", state->pao_count);
        fprintf(arquivo, "%d\n", state->hamburguerCru_count);
        fprintf(arquivo, "%d\n", state->alface_count);
        fprintf(arquivo, "%d\n", state->tomate_count);
        fprintf(arquivo, "%d\n", state->queijo_count);
        fprintf(arquivo, "%d\n", state->bacon_count);
        fprintf(arquivo, "%d\n", state->maioneseDoPato_count);
        fprintf(arquivo, "%d\n", state->onion_rings_count);
        fprintf(arquivo, "%d\n", state->cebola_count);
        fprintf(arquivo, "%d\n", state->picles_count);
        fprintf(arquivo, "%d\n", state->falafel_count);
        fprintf(arquivo, "%d\n", state->frango_count);
        fclose(arquivo);
    }

    // salva os burgers vendidos
    arquivo = fopen("Saves/burgers.txt", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d\n", state->totalHamburgueresVendidos);
        
        fprintf(arquivo, "\n");
        
        fclose(arquivo);
    }
    //salvamento "w" para write and read (escrever e ler)
    arquivo = fopen("Saves/save_game.txt", "w"); // junta tudo que eu salvei para o arquivo principal de progresso.
    if (arquivo == NULL) { 
        return; // nn pode salvar pois algum dos arquivos esta vazio (nulo)
    }

    fprintf(arquivo, "[Moedas]\n");
    fprintf(arquivo, "%d\n", state->dinheiro);

	// salva days.txt
    fprintf(arquivo, "[Dia]\n");
    fprintf(arquivo, "%d\n", state->dia);

    // salva ingredientes.txt
    fprintf(arquivo, "[IngredientesAtuais]\n");
    fprintf(arquivo, "%d\n", state->pao_count);
    fprintf(arquivo, "%d\n", state->hamburguerCru_count);
    fprintf(arquivo, "%d\n", state->alface_count);
    fprintf(arquivo, "%d\n", state->tomate_count);
    fprintf(arquivo, "%d\n", state->queijo_count);
    fprintf(arquivo, "%d\n", state->bacon_count);
    fprintf(arquivo, "%d\n", state->maioneseDoPato_count);
    fprintf(arquivo, "%d\n", state->onion_rings_count);
    fprintf(arquivo, "%d\n", state->cebola_count);
    fprintf(arquivo, "%d\n", state->picles_count);
    fprintf(arquivo, "%d\n", state->falafel_count);
    fprintf(arquivo, "%d\n", state->frango_count);

    // salva burgers.txt
    fprintf(arquivo, "[HamburgueresVendidos]\n");
    fprintf(arquivo, "%d\n", state->totalHamburgueresVendidos);
    
    fprintf(arquivo, "\n");

    fclose(arquivo); // fecha o save_game.txt
} // fim da funcao salvar jogo

void carregarJogo(GameState *state) {
    FILE *arquivo;
    //carregamento "r" para read (ler)
    arquivo = fopen("Saves/save_game.txt", "r"); // detalhe: como tudo esta dentro do save_game, ele so carrega esse save_game.txt
    if (arquivo == NULL) {
        return; // nn existe o save_game.txt
    }

    // carrega coins.txt
    fscanf(arquivo, "[Moedas]\n");
    fscanf(arquivo, "%d\n", &state->dinheiro);

    // carrega days.txt
    fscanf(arquivo, "[Dia]\n");
    fscanf(arquivo, "%d\n", &state->dia);

    // carrega ingredientes.txt
    fscanf(arquivo, "[IngredientesAtuais]\n");
    fscanf(arquivo, "%d\n", &state->pao_count);
    fscanf(arquivo, "%d\n", &state->hamburguerCru_count);
    fscanf(arquivo, "%d\n", &state->alface_count);
    fscanf(arquivo, "%d\n", &state->tomate_count);
    fscanf(arquivo, "%d\n", &state->queijo_count);
    fscanf(arquivo, "%d\n", &state->bacon_count);
    fscanf(arquivo, "%d\n", &state->maioneseDoPato_count);
    fscanf(arquivo, "%d\n", &state->onion_rings_count);
    fscanf(arquivo, "%d\n", &state->cebola_count);
    fscanf(arquivo, "%d\n", &state->picles_count);
    fscanf(arquivo, "%d\n", &state->falafel_count);
    fscanf(arquivo, "%d\n", &state->frango_count);

    // carrega burgers.txt
    fscanf(arquivo, "[HamburgueresVendidos]\n");
    fscanf(arquivo, "%d\n", &state->totalHamburgueresVendidos);
    
    fscanf(arquivo, "\n");
    
    fclose(arquivo);
} // fim da funcao de carregar jogo

//Função principal.
void telaPrincipalEtapa2()
{
    GameContext gameContext = {0};
    GameState gameState = {0};

    //Variáveis para a loja.
    Loja loja;
    Inventarioplayer inventarioJogador;

    inicializarInventario_loja(&inventarioJogador, 100);
    inicializarLoja(&loja);
    organizaloja(&loja);

    initializeGame(&gameContext, &gameState);
    
    carregarJogo(&gameState); // carrega o jogo automaticamente

    BOOL showShopScreen = FALSE; // Flag para controlar a cena da loja

    while (gameState.isRunning) {
        if (gameState.showEndScreen)
        {
            // O dia acabou (timer ou dinheiro).

            if (gameState.dinheiro <= 0)
            {
                // CONDIÇÃO 1: Dinheiro acabou = GAME OVER

                // runEndScreen agora é bloqueante.
                // Se o jogador apertar 'R', ele já chama initializeGame (reset total).
                if (runEndScreen(&gameContext, &gameState))
                {
                    // Jogador apertou 'R' (Restart)
                    // O jogo já foi resetado por runEndScreen.
                    // Apenas saia da tela de game over e volte ao gameplay.
                    gameState.showEndScreen = FALSE;
                }
                else
                {
                    // Jogador apertou 'E' (Exit)
                    gameState.isRunning = FALSE;
                }
            }
            else
            {
                // CONDIÇÃO 2: Dia acabou, mas temos dinheiro = IR PARA A LOJA

                // Pula a tela de Game Over
                gameState.showEndScreen = FALSE;
                showShopScreen = TRUE;
            }
        }
        else if (showShopScreen)
        {
			salvarJogo(&gameState); // salva o jogo automaticamente
			
            loopfuncionaloja(&loja, &inventarioJogador); //Função de loja (Bloqueante).

            // Quando o jogador sair da loja:
            showShopScreen = FALSE; // Desativa a flag da loja

            // Prepara o próximo dia
            clearStack(&gameState);
            initializeNextDay(&gameState); // Reseta o timer e pedidos (NÃO o dinheiro).
        }
        else
        {
            // --- CENA DE GAMEPLAY ---

            // 1. Processa input
            processInput(&gameContext, &gameState);

            // 2. Atualiza a lógica do jogo
            // (updateGame irá ativar showEndScreen se o timer ou o dinheiro acabarem)
            updateGame(&gameState);

            // 3. Decide qual cena desenhar
            if (gameState.showCardapio)
            {
                drawCardapioScreen(&gameContext, &gameState);
            }
            else if (gameState.showCardapio_2)
            {
                desenharCardapio_pagina2(&gameContext, &gameState);
            }
            else
            {
                renderGame(&gameContext, &gameState);
            }

            // Controla a taxa de quadros
            Sleep(33); // ~30 FPS
        }
    }

    // Cleanup
    cleanup(&gameContext, &gameState);

}