#include "../Header/gameplayEtapa2.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../Header/burgerLE.h"
#include "../Header/loja.h"
#include "../Header/filaLE.h"
#include "../Header/historico.h" 
#include <ctype.h> 

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

void resizeBuffer(GameContext *ctx, int width, int height)
{
    if (ctx->charBuffer) free(ctx->charBuffer);
    ctx->screenSize.X = width;
    ctx->screenSize.Y = height;
    ctx->charBuffer = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * width * height);
    if (!ctx->charBuffer) exit(1);
}

void clearBuffer(GameContext *ctx)
{
    if (!ctx->charBuffer) return;
    for (int i = 0; i < ctx->screenSize.X * ctx->screenSize.Y; ++i)
    {
        ctx->charBuffer[i].Char.AsciiChar = ' ';
        ctx->charBuffer[i].Attributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE; 
    }
}

void writeToBuffer(GameContext *ctx, int x, int y, const char *texto, WORD atributos)
{
    if (!ctx->charBuffer || x < 0 || y < 0 || x >= ctx->screenSize.X || y >= ctx->screenSize.Y) return;

    int len = strlen(texto);
    for (int i = 0; i < len; ++i)
    {
        int index = (y * ctx->screenSize.X) + x + i;
        if (x + i < ctx->screenSize.X)
        {
            ctx->charBuffer[index].Char.AsciiChar = texto[i]; 
            ctx->charBuffer[index].Attributes = atributos; 
        }
    }
}

void drawBox(GameContext *ctx, int left, int top, int right, int bottom, WORD attributes)
{
    if (right >= ctx->screenSize.X) right = ctx->screenSize.X - 1;
    if (bottom >= ctx->screenSize.Y) bottom = ctx->screenSize.Y - 1;
    if (left > right || top > bottom) return;

    writeToBuffer(ctx, left, top, "\xC9", attributes);       
    writeToBuffer(ctx, right, top, "\xBB", attributes);      
    writeToBuffer(ctx, left, bottom, "\xC8", attributes);    
    writeToBuffer(ctx, right, bottom, "\xBC", attributes);   

    for (int x = left + 1; x < right; ++x) {
        writeToBuffer(ctx, x, top, "\xCD", attributes);      
        writeToBuffer(ctx, x, bottom, "\xCD", attributes);   
    }

    for (int y = top + 1; y < bottom; ++y) {
        writeToBuffer(ctx, left, y, "\xBA", attributes);     
        writeToBuffer(ctx, right, y, "\xBA", attributes);    
    }
}

void drawOrders(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, "PEDIDOS PENDENTES ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
    
    // --- NEW: Pending Count Indicator ---
    char pendingText[32];
    snprintf(pendingText, sizeof(pendingText), "PENDENTES: %d", state->ordersPending);
    writeToBuffer(ctx, left + 2, bottom - 1, pendingText, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    // ------------------------------------

    for (int i = 0; i < state->contadorDisplayPedidos; i++)
    {
        int yPos = top + 2 + i;
        if (yPos >= bottom) break;

        // Define a cor baseada no tempo restante (opcional: fica vermelho se estiver quase sumindo)
        WORD cor = FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Verde claro

        ULONGLONG tempoDeVida = (ULONGLONG)GetTickCount() - state->pedidosDisplay[i].spawnTime;
        if (tempoDeVida > 7000) { // Se já passou 7 segundos (faltam 3)
            cor = FOREGROUND_RED | FOREGROUND_INTENSITY; // Fica vermelho piscando ou fixo
        }

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "> %s", state->pedidosDisplay[i].text);
        writeToBuffer(ctx, left + 2, yPos, buffer, cor);
    }
}

void drawInput(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, "INPUT ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); 

    char prompt[MAX_COMMAND_LENGTH + 3];
    snprintf(prompt, sizeof(prompt), "> %s", state->currentCommand);
    writeToBuffer(ctx, left + 2, top + 2, prompt, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    if (((ULONGLONG)GetTickCount() / 500) % 2 == 0)
    {
        writeToBuffer(ctx, left + 4 + state->commandLength, top + 2, "_", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    }
}

void drawGrilling(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_RED | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, "GRELHA ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); 

    int width = right - left - 4;
    if (width < 10) width = 10; 

    if (state->isGrilling)
    {
        ULONGLONG elapsed = (ULONGLONG)GetTickCount() - state->grillStartTime;
        float progress = (float)elapsed / GRILL_TIME_MS;
        if (progress > 1.0f) progress = 1.0f;

        int barLength = (int)(progress * width);
        char progressBar[256];
        memset(progressBar, 0, sizeof(progressBar));

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
    writeToBuffer(ctx, left + 2, top, "INGREDIENTES E DINHEIRO ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); 

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

    snprintf(text, sizeof(text), "Bacon:  %d", state->bacon_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Maionese:%d", state->maioneseDoPato_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Onion Rings:%d", state->onion_rings_count); 
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Cebola: %d", state->cebola_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Picles: %d", state->picles_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Falafel:%d", state->falafel_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Frango: %d", state->frango_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    snprintf(text, sizeof(text), "Hamburguer Grelhado: %d", state->hamburguerGrelhado_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); 
}

void drawPilhaDeHamburguerLE_display(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    int width = ctx->screenSize.X;
    
    drawBox(ctx, left, top, right, bottom, FOREGROUND_GREEN | FOREGROUND_RED); 
    writeToBuffer(ctx, left + 2, top, " HAMBURGUER ATUAL ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); 

    int y = bottom - 2; 

    for (int i = 0; i < state->stackSize; ++i) 
    {
        if (y <= top) break; 

        char textoDoIngrediente_noHamburguer[64]; 
        snprintf(textoDoIngrediente_noHamburguer, sizeof(textoDoIngrediente_noHamburguer), " - %s - ", state->PilhaDeHamburguerLE_display[i]);

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
        writeToBuffer(ctx, ((width - (int)strlen(title)) / 2)+5, y, title, FOREGROUND_RED | FOREGROUND_INTENSITY); 
    }
}

void drawTimer(GameContext *ctx, GameState *state)
{
    ULONGLONG elapsed = (ULONGLONG)GetTickCount() - state->tempoDoJogo;
    ULONGLONG remaining = (elapsed > GAME_DURATION_MS) ? 0 : (GAME_DURATION_MS - elapsed);

    int minutes = (int)(remaining / 60000);
    int seconds = (int)((remaining % 60000) / 1000);

    char timerText[32];
    snprintf(timerText, sizeof(timerText), " TEMPO: %02d:%02d ", minutes, seconds);

    int x = ctx->screenSize.X - (int)strlen(timerText) - 2; 
    int y = 1;

    WORD attributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; 
    if (remaining < 30000) 
    {
        attributes = ((ULONGLONG)GetTickCount() / 500) % 2 == 0
            ? (FOREGROUND_RED | FOREGROUND_INTENSITY)
            : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    writeToBuffer(ctx, x, y, timerText, attributes);

    char diaText[32];
    snprintf(diaText, sizeof(diaText), "DIA: %d", state->dia);
    writeToBuffer(ctx, 2, 0, diaText, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
}

void blitToScreen(GameContext *ctx)
{
    if (!ctx->charBuffer) return;
    COORD bufferSize = {ctx->screenSize.X, ctx->screenSize.Y};
    COORD bufferCoord = {0, 0};
    SMALL_RECT writeRegion = {0, 0, ctx->screenSize.X - 1, ctx->screenSize.Y - 1};
    WriteConsoleOutputA(ctx->hConsoleOut, ctx->charBuffer, bufferSize, bufferCoord, &writeRegion);
}

void drawCardapioScreen(GameContext *ctx, GameState *state)
{
    clearBuffer(ctx);

    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    WORD laranja = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; 
    WORD branco = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;
    WORD verde = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD amarelo = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    drawBox(ctx, 0, 0, width - 1, height - 1, laranja);
    writeToBuffer(ctx, (width - 16) / 2, 0, " CARDAPIO (1/2) ", amarelo);

    int x_col1 = 3;
    int x_col2 = (width / 2) + 1;
    int y = 3; 

    writeToBuffer(ctx, x_col1, y++, "1. Bit and Bacon (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Bacon", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "2. Duck Cheese (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Alface", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Tomate", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "3. Quackteirao (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Alface", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    y = 3; 

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

    writeToBuffer(ctx, x_col2, y++, "5. Zero e Um (13)", verde);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "Pao", branco);
    y += 2;

    const char *exitCmd = "[V]oltar ao Jogo";
    writeToBuffer(ctx, (width / 2) - 20, (height - 2), exitCmd, branco);
    const char *nextCmd = "[P]roxima Pagina ->";
    writeToBuffer(ctx, (width / 2) + 5, (height - 2), nextCmd, branco);

    drawTimer(ctx, state);

    blitToScreen(ctx);
}

void desenharCardapio_pagina2(GameContext *ctx, GameState *state)
{
    clearBuffer(ctx);

    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    WORD laranja = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; 
    WORD branco = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;
    WORD verde = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD amarelo = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    drawBox(ctx, 0, 0, width - 1, height - 1, laranja);
    writeToBuffer(ctx, (width - 16) / 2, 0, " CARDAPIO (2/2) ", amarelo);

    int x_col1 = 3;
    int x_col2 = (width / 2) + 1;
    int y = 3; 

    writeToBuffer(ctx, x_col1, y++, "6. Chicken Duckey (21)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Maionese", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Frango", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Onion_Rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "7. Pato Sobre Rodas (24)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Onion_Rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Carne", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Onion_Rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "9. Pato Verde (21)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Maionese", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Picles", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Falafel", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "Pao", branco);
    y += 2;

    y = 3; 

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

    if (y < height - 10) 
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

    const char *prevCmd = "<- pagina [A]nterior";
    writeToBuffer(ctx, (width / 2) - 20, (height - 2), prevCmd, branco);
    const char *exitCmd = "[V]oltar ao Jogo";
    writeToBuffer(ctx, (width / 2) + 5, (height - 2), exitCmd, branco);

    drawTimer(ctx, state);

    blitToScreen(ctx);
}

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

void initializeGame(GameContext *ctx, GameState *state)
{
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
    state->totalHamburgueresVendidos = 0; 
    state->vendasNoDiaAtual = 0; // zera o contador diario
    
    inicializar_BurgerLE_Player(&state->burgerPlayer);
    inicializaFilaLEPedidos(&state->filaDePedidos);
    geraPedidos_FilaLE(&state->filaDePedidos, 1); 
    state->totalPedidosNoDia = state->filaDePedidos.tamanho;
    inicializaFilaLEPedidos(&state->filaAtiva);

    state->stackSize = 0;
    state->ordersPending = state->totalPedidosNoDia; 
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->isRunning = TRUE;
    state->showEndScreen = FALSE;
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE; 
    state->dia = 1; 
    state->hamburguerVazio = 0;
    state->semPedidos = 0;

    // Inicializa a arvore aqui
    inicializa_arvore(&state->historicoVendas);

    state->tempoDoJogo = (ULONGLONG)GetTickCount();

    ctx->hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    ctx->hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    ctx->charBuffer = NULL;

    SetConsoleMode(ctx->hConsoleIn, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(ctx->hConsoleOut, &csbi);
    resizeBuffer(ctx, csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
}

void empilharIngrediente_display(GameState *state, const char *item, int *inventory)
{
    if (state->stackSize >= MAX_BURGER_STACK) return;
    if (*inventory > 0)
    {
        (*inventory)--;
        state->PilhaDeHamburguerLE_display[state->stackSize] = _strdup(item); 
        state->stackSize++;

        if (strcmp(item, "Pao") == 0) adicionarIngredienteLE(&state->burgerPlayer, 1);
        else if (strcmp(item, "Hamburguer Grelhado") == 0) adicionarIngredienteLE(&state->burgerPlayer, 2);
        else if (strcmp(item, "Queijo") == 0) adicionarIngredienteLE(&state->burgerPlayer, 3);
        else if (strcmp(item, "Alface") == 0) adicionarIngredienteLE(&state->burgerPlayer, 4);
        else if (strcmp(item, "Tomate") == 0) adicionarIngredienteLE(&state->burgerPlayer, 5);
        else if (strcmp(item, "Bacon") == 0) adicionarIngredienteLE(&state->burgerPlayer, 6);
        else if (strcmp(item, "Picles") == 0) adicionarIngredienteLE(&state->burgerPlayer, 7);
        else if (strcmp(item, "Cebola") == 0) adicionarIngredienteLE(&state->burgerPlayer, 8);
        else if (strcmp(item, "Falafel") == 0) adicionarIngredienteLE(&state->burgerPlayer, 9);
        else if (strcmp(item, "Maionese do Pato") == 0) adicionarIngredienteLE(&state->burgerPlayer, 10);
        else if (strcmp(item, "Onion Rings") == 0) adicionarIngredienteLE(&state->burgerPlayer, 11);
        else if (strcmp(item, "Maionese") == 0) adicionarIngredienteLE(&state->burgerPlayer, 12);
        else if (strcmp(item, "Frango") == 0) adicionarIngredienteLE(&state->burgerPlayer, 13);
    }
}

void clearStack(GameState *state)
{
    for (int i = 0; i < state->stackSize; i++)
    {
        free(state->PilhaDeHamburguerLE_display[i]);
    }
    deletaBurgerLE(&state->burgerPlayer);
    state->stackSize = 0;
}

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
    else if (_stricmp(state->currentCommand, "pao") == 0) empilharIngrediente_display(state, "Pao", &state->pao_count);
    else if (_stricmp(state->currentCommand, "alface") == 0) empilharIngrediente_display(state, "Alface", &state->alface_count);
    else if (_stricmp(state->currentCommand, "tomate") == 0) empilharIngrediente_display(state, "Tomate", &state->tomate_count);
    else if (_stricmp(state->currentCommand, "queijo") == 0) empilharIngrediente_display(state, "Queijo", &state->queijo_count);
    else if (_stricmp(state->currentCommand, "hamburguer") == 0) empilharIngrediente_display(state, "Hamburguer Grelhado", &state->hamburguerGrelhado_count);
    else if (_stricmp(state->currentCommand, "bacon") == 0) empilharIngrediente_display(state, "Bacon", &state->bacon_count);
    else if (_stricmp(state->currentCommand, "maionese") == 0) empilharIngrediente_display(state, "Maionese do Pato", &state->maioneseDoPato_count);
    else if (_stricmp(state->currentCommand, "onion_rings") == 0) empilharIngrediente_display(state, "Onion Rings", &state->onion_rings_count);
    else if (_stricmp(state->currentCommand, "cebola") == 0) empilharIngrediente_display(state, "Cebola", &state->cebola_count);
    else if (_stricmp(state->currentCommand, "picles") == 0) empilharIngrediente_display(state, "Picles", &state->picles_count);
    else if (_stricmp(state->currentCommand, "falafel") == 0) empilharIngrediente_display(state, "Falafel", &state->falafel_count);
    else if (_stricmp(state->currentCommand, "frango") == 0) empilharIngrediente_display(state, "Frango", &state->frango_count);

    else if (_stricmp(state->currentCommand, "servir") == 0)
    {
        if (state->stackSize > 0 && state->filaAtiva.tamanho > 0)
        {
            state->totalHamburgueresVendidos++; 
            state->vendasNoDiaAtual++; // conta venda do dia

            Pedido_FilaLE pedidoAlvo;
            desenfileiraPedido_FilaLE(&state->filaAtiva, &pedidoAlvo); //pedidoAlvo == Pedido Atual.

            //Verifica o id do Pedido atual, e cria com o hambúrguer necessário.

            BurgerLE burgerPedido = {0}; //Inicializa no index 0.

            switch (pedidoAlvo.id_burger) {
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

            state->dinheiro += comparaHamburgueresLE(&state->burgerPlayer, &burgerPedido); 
            
            // --- Unidade 3: Registrar venda na arvore ---
            registrar_venda_arvore(&state->historicoVendas, burgerPedido.id);

            // --- FIX: Clear player's burger and free temporary burger ---
            deletaBurgerLE(&state->burgerPlayer); 
            destroi_pilha_LE(&burgerPedido.ingredientes);
            // -----------------------------------------------------------



            // --- FIX: Only decrement display count if the order was actually on screen ---
            // If we have more active orders than what's shown, it means we might be serving one that already scrolled off.
            // However, the logic here assumes we ALWAYS serve the oldest active order (head of filaAtiva).
            // If the head of filaAtiva is NOT in the display array (because it timed out visually),
            // then we should NOT decrement contadorDisplayPedidos.
            
            // Logic: The display shows the first 'contadorDisplayPedidos' items of 'filaAtiva'.
            // If an item times out, it is removed from 'pedidosDisplay' but REMAINS in 'filaAtiva'.
            // So, 'filaAtiva' contains ALL active orders, including invisible ones.
            // The invisible ones are at the BEGINNING of 'filaAtiva' (because they are older).
            // Wait, if they time out, they are removed from display but still in queue.
            // So if we serve, we serve the HEAD of filaAtiva.
            // If the HEAD is NOT on screen, we shouldn't touch the screen counter.
            
            // How do we know if the HEAD is on screen?
            // 'contadorDisplayPedidos' tracks how many are on screen.
            // 'filaAtiva.tamanho' tracks total active.
            // If 'filaAtiva.tamanho' > 'contadorDisplayPedidos', it means there are (Total - Display) invisible orders.
            // Since invisible orders are always the oldest, they are at the front of the queue.
            // So if (Total > Display), the one we just served was invisible.
            
            if (state->filaAtiva.tamanho >= state->contadorDisplayPedidos) {
                 // We served an invisible order (or the counts matched exactly before decrement).
                 // Wait, if Total > Display, we served an invisible one. Count shouldn't change.
                 // If Total == Display, we served a visible one. Count should decrement.
                 // Note: We already dequeued, so 'filaAtiva.tamanho' is now 1 less than before.
                 // Let's use the pre-decrement logic or just check:
                 // If we still have enough orders to fill the display, don't reduce display count?
                 // No, that's complex.
                 
                 // Simpler:
                 // We just served an order.
                 // If that order was OFF-SCREEN, 'contadorDisplayPedidos' should remain the same.
                 // If that order was ON-SCREEN, 'contadorDisplayPedidos' should decrease.
                 
                 // The order served was the HEAD.
                 // Was the HEAD on screen?
                 // If (OldTotal > OldDisplay), then HEAD was off-screen.
                 // OldTotal = state->filaAtiva.tamanho + 1 (since we just dequeued)
                 // So if ((state->filaAtiva.tamanho + 1) > state->contadorDisplayPedidos) -> Off-screen.
                 
                 if ((state->filaAtiva.tamanho + 1) > state->contadorDisplayPedidos) {
                     // Served off-screen order. Do NOT decrement display count.
                 } else {
                     // Served on-screen order.
                     state->contadorDisplayPedidos--; 
                 }
            } else {
                // Should not happen normally unless state is weird, but safe to decrement if we think it was on screen.
                state->contadorDisplayPedidos--;
            }
            // -----------------------------------------------------------------------------

            for (int i = 0; i < state->stackSize; i++)
            {
                if (state->PilhaDeHamburguerLE_display[i] != NULL) {
                    free(state->PilhaDeHamburguerLE_display[i]);
                }
            }
            state->stackSize = 0;
        }
        else if (state->stackSize <= 0){ 
            state->hamburguerVazio = 1; 
            state->tempoDeNotificacao = (ULONGLONG)GetTickCount();
        }
        else if (state->filaAtiva.tamanho > 0) { 
            state->semPedidos = 1; 
            state->tempoDeNotificacao = (ULONGLONG)GetTickCount(); 
        }
    } 
    else if (_stricmp(state->currentCommand, "lixo") == 0)
    {
        clearStack(state);
        deletaBurgerLE(&state->burgerPlayer); 
    }

    else if (_stricmp(state->currentCommand, "cardapio") == 0)
    {
        state->showCardapio = TRUE; 
        state->showCardapio_2 = FALSE;
    }

	else if (_stricmp(state->currentCommand, "salvar") == 0) 
    {
        salvarJogo(state);
    }
    
    else if (_stricmp(state->currentCommand, "carregar") == 0) 
    {
        clearStack(state); 
        carregarJogo(state); 
        state->tempoDoJogo = (ULONGLONG)GetTickCount();
        state->isGrilling = FALSE;
        state->ordersPending = 0;
        state->vendasNoDiaAtual = 0; // ao carregar, reseta contador do dia atual (assumindo inicio do dia)

        while (state->filaDePedidos.tamanho != 0) {
            Pedido_FilaLE pedido;
            desenfileiraPedido_FilaLE(&state->filaDePedidos, &pedido);
        }
        geraPedidos_FilaLE(&state->filaDePedidos, state->dia); 
    } 
    
    else if (_stricmp(state->currentCommand, "sair") == 0)
    {
        state->isRunning = FALSE;
    }
    else if (_stricmp(state->currentCommand, "finalizar") == 0)
    {
        state->tempoDoJogo = (ULONGLONG)GetTickCount() - GAME_DURATION_MS;
    }

    state->commandLength = 0;
    state->currentCommand[0] = '\0';
}

void processInput(GameContext *ctx, GameState *state)
{
    DWORD numEvents = 0;
    GetNumberOfConsoleInputEvents(ctx->hConsoleIn, &numEvents);
    if (numEvents == 0) return;

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

                if (state->showCardapio)
                {
                    if (c == 'v' || c == 'V') state->showCardapio = FALSE; 
                    else if (c == 'p' || c == 'P') {
                        state->showCardapio = FALSE;
                        state->showCardapio_2 = TRUE; 
                    }
                }
                else if (state->showCardapio_2)
                {
                    if (c == 'v' || c == 'V') state->showCardapio_2 = FALSE; 
                    else if (c == 'a' || c == 'A') {
                        state->showCardapio_2 = FALSE;
                        state->showCardapio = TRUE; 
                    }
                }
                else
                {
                    if (c == '\r') processCommand(ctx, state);
                    else if (c == '\b') {
                        if (state->commandLength > 0) {
                            state->commandLength--;
                            state->currentCommand[state->commandLength] = '\0';
                        }
                    }
                    else if (isprint(c) && state->commandLength < MAX_COMMAND_LENGTH - 1) {
                        state->currentCommand[state->commandLength] = c;
                        state->commandLength++;
                        state->currentCommand[state->commandLength] = '\0';
                    }
                }
            }
            break;

        case WINDOW_BUFFER_SIZE_EVENT:
        {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(ctx->hConsoleOut, &csbi);
            int newWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int newHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            if (newWidth != ctx->screenSize.X || newHeight != ctx->screenSize.Y) resizeBuffer(ctx, newWidth, newHeight);
            break;
        }
        case MOUSE_EVENT:
        case FOCUS_EVENT:
            break;
        }
    }
    free(eventBuffer);
}

void updateGame(GameState *state)
{
    if (state->isGrilling)
    {
        ULONGLONG now = (ULONGLONG)GetTickCount();
        if (now - state->grillStartTime >= GRILL_TIME_MS)
        {
            state->isGrilling = FALSE;
            state->hamburguerGrelhado_count++;
        }
    }

    if (state->semPedidos) {
        ULONGLONG now = (ULONGLONG)GetTickCount();
        if (now - state->tempoDeNotificacao >= TEMPO_PARA_NOTIFICACAO_MS) state->semPedidos = 0;
    }

    if (state->hamburguerVazio) {
        ULONGLONG now = (ULONGLONG)GetTickCount();
        if (now - state->tempoDeNotificacao >= TEMPO_PARA_NOTIFICACAO_MS) state->hamburguerVazio = 0;
    }

   //Lógica de spawn de pedidos:
    ULONGLONG now = (ULONGLONG)GetTickCount();

    int timerPedidoAtual = 0;
    while (timerPedidoAtual < state->contadorDisplayPedidos)
    {
        //Se passou 15.000ms (15s) desde que o pedido apareceu
        if (now - state->pedidosDisplay[timerPedidoAtual].spawnTime >= 15000)
        {
            for (int j = timerPedidoAtual; j < state->contadorDisplayPedidos - 1; j++)
            {
                state->pedidosDisplay[j] = state->pedidosDisplay[j + 1];
            }
            state->contadorDisplayPedidos--;
        }
        else timerPedidoAtual++;
    }

    ULONGLONG intervaloSpawn = GAME_DURATION_MS / state->totalPedidosNoDia;
    if (intervaloSpawn < 1000) intervaloSpawn = 1000;

    if ((now - state->ultimoSpawnDisplayPedidos >= intervaloSpawn) &&
        (state->filaDePedidos.inicio != NULL) &&
        (state->contadorDisplayPedidos < 3))
    {
        state->ultimoSpawnDisplayPedidos = now;
        for (int j = state->contadorDisplayPedidos; j > 0; j--) state->pedidosDisplay[j] = state->pedidosDisplay[j - 1];

        Pedido_FilaLE pedidoAtual; 
        desenfileiraPedido_FilaLE(&state->filaDePedidos, &pedidoAtual);

        state->pedidosDisplay[0].spawnTime = now;
        strcpy(state->pedidosDisplay[0].text, getNomeDoBurger(pedidoAtual.id_burger));
        state->pedidosDisplay[0].id_burger = pedidoAtual.id_burger;

        enfileiraPedido_FilaLE(&state->filaAtiva, pedidoAtual); 
        state->contadorDisplayPedidos++;
    }

    // 3. Sync ordersPending
    state->ordersPending = state->filaAtiva.tamanho;
    // --- End of NEW Logic ---

    if (state->dinheiro <= 0)
    {
        state->showEndScreen = TRUE;
        return; 
    }

    ULONGLONG elapsed = (ULONGLONG)GetTickCount() - state->tempoDoJogo;
    if (elapsed >= GAME_DURATION_MS) state->showEndScreen = TRUE; 
}

void renderGame(GameContext *ctx, GameState *state)
{
    if (!ctx->charBuffer) return;
    clearBuffer(ctx);

    int leftColR = ctx->screenSize.X / 3; 

    int orderBoxL = 1;
    int orderBoxT = 1;
    int orderBoxR = leftColR;
    int orderBoxB = 8; 
    drawOrders(ctx, state, orderBoxL, orderBoxT, orderBoxR, orderBoxB);

    int inputL = 1;
    int inputT = orderBoxB + 1; 
    int inputR = leftColR;
    int inputB = inputT + 4;   
    drawInput(ctx, state, inputL, inputT, inputR, inputB);

    int grillL = 1;
    int grillT = inputB + 1; 
    int grillR = leftColR;
    int grillB = grillT + 6;   
    drawGrilling(ctx, state, grillL, grillT, grillR, grillB);

    int invL = 1;
    int invB = ctx->screenSize.Y - 2; 
    int invT = grillB + 1;            
    int invR = leftColR;
    drawIngredientes(ctx, state, invL, invT, invR, invB); 

    int stackL = leftColR + 2;
    int stackT = 1;
    int stackR = ctx->screenSize.X - 2;
    int stackB = ctx->screenSize.Y - 2;
    drawPilhaDeHamburguerLE_display(ctx, state, stackL, stackT, stackR, stackB);

    drawTimer(ctx, state);
    blitToScreen(ctx);
}

// save diario
void salvarRelatorioDiario(int dia, int vendas) {
    // abre o arquivo com "a" (append) pra nn apagar o historico anterior
    FILE *f = fopen("Saves/vendas_diarias.txt", "a");
    if (f == NULL) return;

    fprintf(f, "Dia %d: %d ingredientes/hamburgueres vendidos.\n", dia, vendas);
    fclose(f);
}

void initializeNextDay(GameState *state)
{
    salvarRelatorioDiario(state->dia, state->vendasNoDiaAtual);
    state->vendasNoDiaAtual = 0;

    state->stackSize = 0;
    state->ordersPending = 0;
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->showEndScreen = FALSE; 
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE;
    state->hamburguerVazio = 0;
    state->semPedidos = 0;

    deletaBurgerLE(&state->burgerPlayer); 
    state->tempoDoJogo = (ULONGLONG)GetTickCount();
    state->dia++;

    // Multa se sobrou pedido ou pedido incompleto
    int pedidosRestantes = state->filaDePedidos.tamanho;
    if (pedidosRestantes > 0) {
        int multa = pedidosRestantes * 10; 
        state->dinheiro -= multa;
    }

    while (state->filaDePedidos.tamanho != 0) {
        Pedido_FilaLE pedido;
        desenfileiraPedido_FilaLE(&state->filaDePedidos, &pedido);
    }
    geraPedidos_FilaLE(&state->filaDePedidos, state->dia); 

    state->totalPedidosNoDia = state->filaDePedidos.tamanho;
    if (state->totalPedidosNoDia == 0) state->totalPedidosNoDia = 1; 
}

BOOL runEndScreen(GameContext *ctx, GameState *state)
{
    BOOL inEndScreen = TRUE;
    deletaBurgerLE(&state->burgerPlayer); 
                                          
    while (inEndScreen)
    {
        drawEndScreen(ctx, state);
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
                                clearStack(state);
                                initializeGame(ctx, state); 
                                return TRUE; 
                            }
                        if (c == 'e' || c == 'E') return FALSE; 
                    }
                    break;
                case WINDOW_BUFFER_SIZE_EVENT:
                {
                    CONSOLE_SCREEN_BUFFER_INFO csbi;
                    GetConsoleScreenBufferInfo(ctx->hConsoleOut, &csbi);
                    int newWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                    int newHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                    if (newWidth != ctx->screenSize.X || newHeight != ctx->screenSize.Y) resizeBuffer(ctx, newWidth, newHeight);
                    break;
                }
                }
            }
            free(eventBuffer);
        }
        Sleep(33);
    }
    return FALSE; 
}

void cleanup(GameContext *ctx, GameState *state)
{
    clearStack(state);
    if (ctx->charBuffer) free(ctx->charBuffer);

    // Libera a arvore
    liberar_arvore(state->historicoVendas);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);

    system("cls");
    printf("Obrigado por jogar!\n");
}

void salvarJogo(GameState *state) { 
    FILE *arquivo; 
    
    arquivo = fopen("Saves/coins.txt", "w"); 
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", state->dinheiro);
        fclose(arquivo);
    }
    arquivo = fopen("Saves/days.txt", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", state->dia);
        fclose(arquivo);
    }
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
    arquivo = fopen("Saves/burgers.txt", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d\n", state->totalHamburgueresVendidos);
        fprintf(arquivo, "\n");
        fclose(arquivo);
    }
    
    arquivo = fopen("Saves/save_game.txt", "w"); 
    if (arquivo == NULL) return; 

    fprintf(arquivo, "[Moedas]\n");
    fprintf(arquivo, "%d\n", state->dinheiro);
    fprintf(arquivo, "[Dia]\n");
    fprintf(arquivo, "%d\n", state->dia);
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
    fprintf(arquivo, "[HamburgueresVendidos]\n");
    fprintf(arquivo, "%d\n", state->totalHamburgueresVendidos);
    fprintf(arquivo, "\n");
    fclose(arquivo); 
} 

void carregarJogo(GameState *state) {
    FILE *arquivo;
    arquivo = fopen("Saves/save_game.txt", "r"); 
    if (arquivo == NULL) return; 

    fscanf(arquivo, "[Moedas]\n");
    fscanf(arquivo, "%d\n", &state->dinheiro);
    fscanf(arquivo, "[Dia]\n");
    fscanf(arquivo, "%d\n", &state->dia);
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
    fscanf(arquivo, "[HamburgueresVendidos]\n");
    fscanf(arquivo, "%d\n", &state->totalHamburgueresVendidos);
    fscanf(arquivo, "\n");
    fclose(arquivo);
} 

/**
 * @brief Draws the Main Menu screen.
 */
void drawMainMenu(GameContext *ctx, int selectedOption)
{
    clearBuffer(ctx);

    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    // Draw border
    drawBox(ctx, 0, 0, width - 1, height - 1, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    // Title
    const char *title = "BEM VINDO AO PATO BURGUER";
    int titleX = (width - (int)strlen(title)) / 2;
    int titleY = height / 4;
    writeToBuffer(ctx, titleX, titleY, title, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    // Options
    const char *opt1 = "Carregar Jogo";
    const char *opt2 = "Novo Jogo";

    int optX = (width - 20) / 2; // Approximate centering
    int optY = height / 2;

    WORD colorSelected = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD colorNormal = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;

    // Option 1: Carregar Jogo
    char buffer1[64];
    if (selectedOption == 0) {
        snprintf(buffer1, sizeof(buffer1), "> %s <", opt1);
        writeToBuffer(ctx, optX - 2, optY, buffer1, colorSelected);
    } else {
        writeToBuffer(ctx, optX, optY, opt1, colorNormal);
    }

    // Option 2: Novo Jogo
    char buffer2[64];
    if (selectedOption == 1) {
        snprintf(buffer2, sizeof(buffer2), "> %s <", opt2);
        writeToBuffer(ctx, optX - 2, optY + 2, buffer2, colorSelected);
    } else {
        writeToBuffer(ctx, optX, optY + 2, opt2, colorNormal);
    }

    // Instructions
    const char *instr = "Use W/S para navegar e ENTER para selecionar";
    writeToBuffer(ctx, (width - (int)strlen(instr)) / 2, height - 4, instr, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    blitToScreen(ctx);
}

/**
 * @brief Runs the Main Menu loop.
 * @return 0 for "Carregar Jogo", 1 for "Novo Jogo".
 */
int runMainMenu(GameContext *ctx)
{
    int selectedOption = 0;
    BOOL inMenu = TRUE;
    
    // Clear input buffer first
    FlushConsoleInputBuffer(ctx->hConsoleIn);

    while (inMenu)
    {
        drawMainMenu(ctx, selectedOption);

        DWORD numEvents = 0;
        GetNumberOfConsoleInputEvents(ctx->hConsoleIn, &numEvents);

        if (numEvents > 0)
        {
            INPUT_RECORD *eventBuffer = (INPUT_RECORD *)malloc(sizeof(INPUT_RECORD) * numEvents);
            DWORD eventsRead = 0;
            ReadConsoleInput(ctx->hConsoleIn, eventBuffer, numEvents, &eventsRead);

            for (DWORD i = 0; i < eventsRead; ++i)
            {
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown)
                {
                    WORD vk = eventBuffer[i].Event.KeyEvent.wVirtualKeyCode;
                    char ch = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;

                    if (vk == VK_UP || ch == 'w' || ch == 'W')
                    {
                        selectedOption--;
                        if (selectedOption < 0) selectedOption = 1;
                    }
                    else if (vk == VK_DOWN || ch == 's' || ch == 'S')
                    {
                        selectedOption++;
                        if (selectedOption > 1) selectedOption = 0;
                    }
                    else if (vk == VK_RETURN)
                    {
                        free(eventBuffer);
                        return selectedOption;
                    }
                }
                else if (eventBuffer[i].EventType == WINDOW_BUFFER_SIZE_EVENT)
                {
                    CONSOLE_SCREEN_BUFFER_INFO csbi;
                    GetConsoleScreenBufferInfo(ctx->hConsoleOut, &csbi);
                    int newWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                    int newHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                    if (newWidth != ctx->screenSize.X || newHeight != ctx->screenSize.Y)
                    {
                        resizeBuffer(ctx, newWidth, newHeight);
                    }
                }
            }
            free(eventBuffer);
        }
        Sleep(33);
    }
    return 1; // Default to New Game if something breaks
}

//Função principal.
void telaPrincipalEtapa2()
{
    GameContext gameContext = {0};
    GameState gameState = {0};

    Loja loja;
    Inventarioplayer inventarioJogador;

    // Get console handles manually for the menu before initialization
    gameContext.hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    gameContext.hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Set console mode
    SetConsoleMode(gameContext.hConsoleIn, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);

    // Initial buffer setup
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(gameContext.hConsoleOut, &csbi);
    resizeBuffer(&gameContext, csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    
    // Hide cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(gameContext.hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(gameContext.hConsoleOut, &cursorInfo);

    // --- MAIN MENU ---
    int choice = runMainMenu(&gameContext);
    
    // --- GAME START ---

    inicializarInventario_loja(&inventarioJogador, 100);
    inicializarLoja(&loja);
    organizaloja(&loja);

    initializeGame(&gameContext, &gameState);
    
    if (choice == 0) {
        carregarJogo(&gameState); // carrega o jogo APENAS se o usuário escolheu "Carregar Jogo"
    }
    // Se choice == 1 (Novo Jogo), initializeGame já configurou os defaults (Dia 1, $100, etc.)

    BOOL showShopScreen = FALSE; 

    while (gameState.isRunning) {
        if (gameState.showEndScreen)
        {
            if (gameState.dinheiro <= 0)
            {
                if (runEndScreen(&gameContext, &gameState)) gameState.showEndScreen = FALSE;
                else gameState.isRunning = FALSE;
            }
            else
            {
                gameState.showEndScreen = FALSE;
                showShopScreen = TRUE;
            }
        }
        else if (showShopScreen)
        {
			salvarJogo(&gameState); 
            inventarioJogador.dinheiro = gameState.dinheiro; 
            loopfuncionaloja(&loja, &inventarioJogador); 
            gameState.dinheiro = inventarioJogador.dinheiro; 
            showShopScreen = FALSE; 
            clearStack(&gameState);
            initializeNextDay(&gameState); 
        }
        else
        {
            processInput(&gameContext, &gameState);
            updateGame(&gameState);
            if (gameState.showCardapio) drawCardapioScreen(&gameContext, &gameState);
            else if (gameState.showCardapio_2) desenharCardapio_pagina2(&gameContext, &gameState);
            else renderGame(&gameContext, &gameState);
            Sleep(33); 
        }
    }
    cleanup(&gameContext, &gameState);
}