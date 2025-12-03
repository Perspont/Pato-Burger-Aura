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

WORD obterCorDoIngrediente(const char* nomeIngrediente) {
    if (strcmp(nomeIngrediente, "Pao") == 0) return FOREGROUND_RED | FOREGROUND_GREEN;
    else if (strcmp(nomeIngrediente, "Hamburguer") == 0) return FOREGROUND_RED;
    else if (strcmp(nomeIngrediente, "Queijo") == 0) return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    else if (strcmp(nomeIngrediente, "Alface") == 0) return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    else if (strcmp(nomeIngrediente, "Tomate") == 0) return FOREGROUND_RED | FOREGROUND_INTENSITY;
    else if (strcmp(nomeIngrediente, "Bacon") == 0) return FOREGROUND_RED;
    else if (strcmp(nomeIngrediente, "Picles") == 0) return FOREGROUND_GREEN;
    else if (strcmp(nomeIngrediente, "Cebola") == 0) return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    else if (strcmp(nomeIngrediente, "Falafel") == 0) return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    else if (strcmp(nomeIngrediente, "Maionese do Pato") == 0) return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    else if (strcmp(nomeIngrediente, "Onion Rings") == 0) return FOREGROUND_RED | FOREGROUND_GREEN;
    else if (strcmp(nomeIngrediente, "Frango") == 0) return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
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
    
    char pendingText[32];
    snprintf(pendingText, sizeof(pendingText), "PENDENTES: %d", state->ordersPending);
    writeToBuffer(ctx, left + 2, bottom - 1, pendingText, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    for (int i = 0; i < state->contadorDisplayPedidos; i++)
    {
        int yPos = top + 2 + i;
        if (yPos >= bottom) break;

        WORD cor = FOREGROUND_GREEN | FOREGROUND_INTENSITY; 
        ULONGLONG tempoDeVida = (ULONGLONG)GetTickCount() - state->pedidosDisplay[i].spawnTime;
        if (tempoDeVida > 7000) cor = FOREGROUND_RED | FOREGROUND_INTENSITY; 

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
        writeToBuffer(ctx, left + 2, top + 2, "Grelha esta vazia.", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
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

        const char *nomeIngrediente = state->PilhaDeHamburguerLE_display[i];
        char textoDoIngrediente_noHamburguer[64]; 
        snprintf(textoDoIngrediente_noHamburguer, sizeof(textoDoIngrediente_noHamburguer), " - %s - ", nomeIngrediente);

        int textLen = strlen(textoDoIngrediente_noHamburguer);
        int boxWidth = right - left;
        int textX = left + (boxWidth - textLen) / 2;
        if (textX < left + 1) textX = left + 1;

        WORD corDoIngrediente = obterCorDoIngrediente(nomeIngrediente);
        writeToBuffer(ctx, textX, y, textoDoIngrediente_noHamburguer, corDoIngrediente);
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


void drawInstructionsScreen(GameContext *ctx, GameState *state)
{
    clearBuffer(ctx);
    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    WORD laranja = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD branco = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;
    WORD verde = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD amarelo = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    drawBox(ctx, 0, 0, width - 1, height - 1, laranja);

    const char *header = " INSTRUCOES ";
    writeToBuffer(ctx, (width - (int)strlen(header)) / 2, 0, header, amarelo);

    int y = 3;
    int x_col1 = 4;
    int x_col2 = (width / 2) + 2;

    // Coluna 1
    writeToBuffer(ctx, x_col1, y++, "grelhar    -> Grelha hamburguer", branco);
    writeToBuffer(ctx, x_col1, y++, "pao        -> Empilha pao", branco);
    writeToBuffer(ctx, x_col1, y++, "alface     -> Empilha alface", branco);
    writeToBuffer(ctx, x_col1, y++, "tomate     -> Empilha tomate", branco);
    writeToBuffer(ctx, x_col1, y++, "queijo     -> Empilha queijo", branco);
    writeToBuffer(ctx, x_col1, y++, "hamburguer -> Empilha carne", branco);
    writeToBuffer(ctx, x_col1, y++, "bacon      -> Empilha bacon", branco);
    writeToBuffer(ctx, x_col1, y++, "maionese   -> Empilha maionese", branco);

    // Reinicia Y para Coluna 2
    y = 3;
    writeToBuffer(ctx, x_col2, y++, "onion_rings -> Empilha onion rings", branco);
    writeToBuffer(ctx, x_col2, y++, "cebola      -> Empilha cebola", branco);
    writeToBuffer(ctx, x_col2, y++, "picles      -> Empilha picles", branco);
    writeToBuffer(ctx, x_col2, y++, "falafel     -> Empilha falafel", branco);
    writeToBuffer(ctx, x_col2, y++, "frango      -> Empilha frango", branco);
    y++; // Espaço extra
    writeToBuffer(ctx, x_col2, y++, "servir      -> Serve o pedido", verde);
    writeToBuffer(ctx, x_col2, y++, "lixo        -> Joga fora", FOREGROUND_RED | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, x_col2, y++, "cardapio    -> Abre este menu", branco);
    writeToBuffer(ctx, x_col2, y++, "sair        -> Sai do jogo", branco);

    const char *exitCmd = "[V]oltar ao Jogo";
    writeToBuffer(ctx, (width / 2), (height - 2), exitCmd, branco);
    const char *cardCmd = "Mostrar [C]ardapio";
    writeToBuffer(ctx, (width / 2), (height - 4), cardCmd, branco);

    drawTimer(ctx, state);
    blitToScreen(ctx);
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
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "bacon", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "2. Duck Cheese (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "alface", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "tomate", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "3. Quackteirao (16)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "alface", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    y += 2;

    y = 3; 

    writeToBuffer(ctx, x_col2, y++, "4. Big Pato (27)", verde);
    writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "alface", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "alface", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col2, y++, "5. Zero e Um (13)", verde);
    writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    y += 2;


    const char *instCmd = "Mostrar [I]nstrucoes";
    writeToBuffer(ctx, (width / 2), (height - 4), instCmd, branco);

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
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "maionese", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "frango", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "onion_rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "7. Pato Sobre Rodas (24)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "onion_rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "onion_rings", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    y += 2;

    writeToBuffer(ctx, x_col1, y++, "8. Recursivo (35)", verde);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "tomate", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "cebola", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "hamburguer", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "maionese", branco);
    writeToBuffer(ctx, x_col1 + 2, y++, "pao", branco);
    y += 2;

    y = 3; 

    writeToBuffer(ctx, x_col2, y++, "9. Pato Verde (21)", verde);
    writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "maionese", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "picles", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "cebola", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "queijo", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "falafel", branco);
    writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    y += 2;

    if (y < height - 10) 
    {
        writeToBuffer(ctx, x_col2, y++, "10. Pickles and MAYO! (25)", verde);
        writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "picles", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "maionese", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "bacon", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "picles", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "maionese", branco);
        writeToBuffer(ctx, x_col2 + 2, y++, "pao", branco);
    }

    const char *instCmd = "Mostrar [I]nstrucoes";
    writeToBuffer(ctx, (width / 2), (height - 4), instCmd, branco);

    const char *prevCmd = "<- pagina [A]nterior";
    writeToBuffer(ctx, (width / 2) - 20, (height - 2), prevCmd, branco);
    const char *exitCmd = "[V]oltar ao Jogo";
    writeToBuffer(ctx, (width / 2) + 5, (height - 2), exitCmd, branco);

    drawTimer(ctx, state);

    blitToScreen(ctx);
}

// Função auxiliar recursiva para desenhar o histórico de vendas
void drawHistoricoVendasRec(GameContext *ctx, NoHistorico *raiz, int *x, int *y) {
    if (raiz == NULL) return;

    //Percorre lado esquerdo (IDs menores)
    drawHistoricoVendasRec(ctx, raiz->esq, x, y);

    //Verifica se ainda cabe na tela (deixa 4 linhas de margem embaixo)
    if (*y >= ctx->screenSize.Y - 4) {
        *y = 8; //Move para o próximo espaço.
        *x += 30;
    }

    //Verifica se a nova coluna ainda cabe na tela
    if (*x < ctx->screenSize.X - 20) {
        const char* nome = getNomeDoBurger(raiz->id_burger);
        char buffer[64];

        snprintf(buffer, sizeof(buffer), "%s: %d", nome, raiz->quantidade_vendida);

        writeToBuffer(ctx, *x, *y, buffer, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

        (*y)++; // Incrementa a linha para o próximo item
    }

    //Percorre lado direito (IDs maiores)
    drawHistoricoVendasRec(ctx, raiz->dir, x, y);
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

    const char *relatorio = "RELATORIO DE VENDAS";
    int titleX = (width - (int)strlen(title)) / 2;
    writeToBuffer(ctx, titleX, 2, relatorio, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

    writeToBuffer(ctx, 4, 6, "Hamburgueres Vendidos:", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

    int startX = 6;
    int startY = 8; // Começa a listar na linha 8

    if (state->historicoVendas == NULL) {
        writeToBuffer(ctx, startX, startY, "Nenhuma venda registrada.", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    } else {
        // Chama a função recursiva passando o endereço de startY para que ela possa incrementá-lo
        drawHistoricoVendasRec(ctx, state->historicoVendas, &startX, &startY);
    }

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

    // Inicializa contadores de itens vendidos com zero
    state->pao_vendidos = 0;
    state->alface_vendidos = 0;
    state->tomate_vendidos = 0;
    state->queijo_vendidos = 0;
    state->hamburguer_vendidos = 0;
    state->bacon_vendidos = 0;
    state->maioneseDoPato_vendidos = 0;
    state->onion_rings_vendidos = 0;
    state->cebola_vendidos = 0;
    state->picles_vendidos = 0;
    state->falafel_vendidos = 0;
    state->frango_vendidos = 0;
    
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
    state->showInstructions = FALSE;
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE; 
    state->dia = 1; 
    state->hamburguerVazio = 0;
    state->semPedidos = 0;

    // Inicializa a arvore aqui
    liberar_arvore(state->historicoVendas);
    inicializa_arvore(&state->historicoVendas);
    inicializarArvoreIngredientesDia(&state->raizIngredientes);

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

        // aqui adiciono a logica para contar cada ingrediente usado no relatorio
        if (strcmp(item, "Pao") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 1);
            state->pao_vendidos++; 
        }
        else if (strcmp(item, "Hamburguer") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 2);
            state->hamburguer_vendidos++; 
        }
        else if (strcmp(item, "Queijo") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 3);
            state->queijo_vendidos++; 
        }
        else if (strcmp(item, "Alface") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 4);
            state->alface_vendidos++; 
        }
        else if (strcmp(item, "Tomate") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 5);
            state->tomate_vendidos++; 
        }
        else if (strcmp(item, "Bacon") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 6);
            state->bacon_vendidos++; 
        }
        else if (strcmp(item, "Picles") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 7);
            state->picles_vendidos++; 
        }
        else if (strcmp(item, "Cebola") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 8);
            state->cebola_vendidos++; 
        }
        else if (strcmp(item, "Falafel") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 9);
            state->falafel_vendidos++; 
        }
        else if (strcmp(item, "Maionese do Pato") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 10);
            state->maioneseDoPato_vendidos++; 
        }
        else if (strcmp(item, "Onion Rings") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 11);
            state->onion_rings_vendidos++; 
        }
        else if (strcmp(item, "Maionese") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 12);
            // nao tinha contador especifico para maionese comum no struct, entao deixo sem
        }
        else if (strcmp(item, "Frango") == 0) { 
            adicionarIngredienteLE(&state->burgerPlayer, 13);
            state->frango_vendidos++; 
        }
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
    else if (_stricmp(state->currentCommand, "hamburguer") == 0) empilharIngrediente_display(state, "Hamburguer", &state->hamburguerGrelhado_count);
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
            state->vendasNoDiaAtual++; 

            Pedido_FilaLE pedidoAlvo;
            desenfileiraPedido_FilaLE(&state->filaAtiva, &pedidoAlvo); 

            BurgerLE burgerPedido = {0}; 

            switch (pedidoAlvo.id_burger) {
                case 1: inicializa_BitAndBacon_LE(&burgerPedido); break;
                case 2: inicializa_DuckCheese_LE(&burgerPedido); break;
                case 3: inicializa_Quackteirao_LE(&burgerPedido); break;
                case 4: inicializa_BigPato_LE(&burgerPedido); break;
                case 5: inicializa_ZeroUm_LE(&burgerPedido); break;
                case 6: inicializa_ChickenDuckey_LE(&burgerPedido); break;
                case 7: inicializa_PatoSobreRodas_LE(&burgerPedido); break;
                case 8: inicializa_Recursivo_LE(&burgerPedido); break;
                case 9: inicializa_PatoVerde_LE(&burgerPedido); break;
                case 10: inicializa_PicklesAndMayo_LE(&burgerPedido); break;
                default: break;
            }

            // REGISTRA A VENDA NA ARVORE DE HISTORICO (PARA O GAME OVER)
            registrar_venda_arvore(&state->historicoVendas, pedidoAlvo.id_burger);

            state->dinheiro += comparaHamburgueresLE(&state->burgerPlayer, &burgerPedido); 

            int hiddenOrders = (state->filaAtiva.tamanho + 1) - state->contadorDisplayPedidos;

            if (hiddenOrders > 0) {
            } else {
                
                state->contadorDisplayPedidos--; 
            }

            clearStack(state); // Limpa o hamburguer da mao do jogador para o proximo pedido
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
        state->showInstructions = TRUE;
        state->showCardapio = FALSE;
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
        state->vendasNoDiaAtual = 0; 

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

                if (state->showInstructions) {
                    if (c == 'v' || c == 'V') state->showInstructions = FALSE;
                    else if (c == 'c' || c == 'C') {
                        state->showInstructions = FALSE;
                        state->showCardapio = TRUE;
                        state->showCardapio_2 = FALSE;
                    }
                }

                else if (state->showCardapio)
                {
                    if (c == 'v' || c == 'V') state->showCardapio = FALSE; 
                    else if (c == 'p' || c == 'P') {
                        state->showInstructions = FALSE;
                        state->showCardapio = FALSE;
                        state->showCardapio_2 = TRUE; 
                    }
                    else if (c == 'i' || c == 'I') {
                        state->showInstructions = TRUE;
                        state->showCardapio = FALSE;
                        state->showCardapio_2 = FALSE;
                    }
                }
                else if (state->showCardapio_2)
                {
                    if (c == 'v' || c == 'V') state->showCardapio_2 = FALSE; 
                    else if (c == 'a' || c == 'A') {
                        state->showInstructions = FALSE;
                        state->showCardapio_2 = FALSE;
                        state->showCardapio = TRUE; 
                    }
                    else if (c == 'i' || c == 'I') {
                        state->showInstructions = TRUE;
                        state->showCardapio = FALSE;
                        state->showCardapio_2 = FALSE;
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

    ULONGLONG now = GetTickCount();

    int timerPedidoAtual = 0;
    while (timerPedidoAtual < state->contadorDisplayPedidos)
    {
        // AUMENTADO PARA 15 SEGUNDOS
        if (now - state->pedidosDisplay[timerPedidoAtual].spawnTime >= 15000)
        {
            for (int j = timerPedidoAtual; j < state->contadorDisplayPedidos - 1; j++)
            {
                state->pedidosDisplay[j] = state->pedidosDisplay[j + 1];
            }
            state->contadorDisplayPedidos--;
            
            // NAO REMOVE DA FILA ATIVA! O PEDIDO CONTINUA VALENDO, SO FICA INVISIVEL.
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

    // PENDENTES = TOTAL NA FILA - O QUE ESTA NA TELA
    // SE TEM 5 NA FILA E 3 NA TELA, TEM 2 PENDENTES (OCULTOS)
    int pendentes = state->filaAtiva.tamanho - state->contadorDisplayPedidos;
    if (pendentes < 0) pendentes = 0; // Just in case
    state->ordersPending = pendentes;

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

// aqui implementei o log diario pra salvar cada ingrediente individualmente
void salvarLogDiario(GameState *state) {
    FILE *f = fopen("Saves/vendas_diarias.txt", "a");
    if (f == NULL) return;

    fprintf(f, "========================================\n");
    fprintf(f, "RELATORIO DO DIA %d\n", state->dia);
    fprintf(f, "Hamburgueres Servidos: %d\n", state->vendasNoDiaAtual);
    fprintf(f, "----------------------------------------\n");
    fprintf(f, "Ingredientes Consumidos:\n");
    fprintf(f, "Pao: %d\n", state->pao_vendidos);
    fprintf(f, "Hamburguer (Carne): %d\n", state->hamburguer_vendidos);
    fprintf(f, "Queijo: %d\n", state->queijo_vendidos);
    fprintf(f, "Alface: %d\n", state->alface_vendidos);
    fprintf(f, "Tomate: %d\n", state->tomate_vendidos);
    fprintf(f, "Bacon: %d\n", state->bacon_vendidos);
    fprintf(f, "Picles: %d\n", state->picles_vendidos);
    fprintf(f, "Cebola: %d\n", state->cebola_vendidos);
    fprintf(f, "Falafel: %d\n", state->falafel_vendidos);
    fprintf(f, "Maionese Pato: %d\n", state->maioneseDoPato_vendidos);
    fprintf(f, "Onion Rings: %d\n", state->onion_rings_vendidos);
    fprintf(f, "Frango: %d\n", state->frango_vendidos);
    fprintf(f, "========================================\n\n");
    
    fclose(f);
}

void initializeNextDay(GameState *state)
{
    // SALVA O RELATORIO ANTES DE MUDAR O DIA
    salvarLogDiario(state);

    // REMOVIDO: inicializarArvoreIngredientesDia(&state->raizIngredientes);
    // REMOVIDO: atualizar_quantidade_ingrediente... (movido para atualizarIngredientesDoDia)

    
    // Zera os contadores diarios
    state->vendasNoDiaAtual = 0; 
    state->pao_vendidos = 0;
    state->hamburguer_vendidos = 0;
    state->queijo_vendidos = 0;
    state->alface_vendidos = 0;
    state->tomate_vendidos = 0;
    state->bacon_vendidos = 0;
    state->picles_vendidos = 0;
    state->cebola_vendidos = 0;
    state->falafel_vendidos = 0;
    state->maioneseDoPato_vendidos = 0;
    state->onion_rings_vendidos = 0;
    state->frango_vendidos = 0;

    state->stackSize = 0;
    state->ordersPending = 0;
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->showEndScreen = FALSE;
    state->showInstructions = FALSE;
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE;
    state->hamburguerVazio = 0;
    state->semPedidos = 0;

    deletaBurgerLE(&state->burgerPlayer); 
    state->tempoDoJogo = (ULONGLONG)GetTickCount();
    state->dia++;

    // --- LOGICA DE MULTAS ---
    
    // 1. Multa se sobrou pedido na fila
    int pedidosRestantes = state->filaDePedidos.tamanho;
    if (pedidosRestantes > 0) {
        int multa = pedidosRestantes * 10; 
        state->dinheiro -= multa;
    }

    // 2. Multa por desperdicio (comida na mao quando acaba o dia)
    if (state->stackSize > 0) {
        int multaDesperdicio = state->stackSize * 2; 
        state->dinheiro -= multaDesperdicio;
    }

    // LIMPA A FILA ATIVA (PEDIDOS NA TELA)
    while (state->filaAtiva.tamanho != 0) {
        Pedido_FilaLE pedido;
        desenfileiraPedido_FilaLE(&state->filaAtiva, &pedido);
    }

    while (state->filaDePedidos.tamanho != 0) {
        Pedido_FilaLE pedido;
        desenfileiraPedido_FilaLE(&state->filaDePedidos, &pedido);
    }
    geraPedidos_FilaLE(&state->filaDePedidos, state->dia); 

    state->totalPedidosNoDia = state->filaDePedidos.tamanho;
    if (state->totalPedidosNoDia == 0) state->totalPedidosNoDia = 1; 
}

int runEndScreen(GameContext *ctx, GameState *state)
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
                                free(eventBuffer);
                                return 1; 
                            }
                            if (c == 's' || c == 'S') { 
                                clearStack(state);
                                free(eventBuffer);
                                return 2; 
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
                    default: return 0;
                }
            }
            free(eventBuffer);
        }
        Sleep(33);
    }
    return 0;
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

//Função auxiliar recursiva para desenhar a árvore na tela --> Necessário.
void drawIngredientesTreeRecursive(GameContext *ctx, NO_AVL *no, int x, int *y) {
    if (no == NULL) return;

    //ir para a direita primeiro (Pois está ordenada por quantidade).
    drawIngredientesTreeRecursive(ctx, no->dir, x, y);

    //Verifica se ainda cabe na tela
    if (*y < ctx->screenSize.Y - 4) {
        char buffer[64];

        snprintf(buffer, sizeof(buffer), "%-20s | %d", no->ingrediente, no->quantidade);

        WORD cor = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

        writeToBuffer(ctx, x, *y, buffer, cor);
        (*y)++;
    }

    drawIngredientesTreeRecursive(ctx, no->esq, x, y);
}


void desenharArvoreNoBuffer(GameContext *ctx, NO_AVL *raiz, int x, int *y) {
    if (raiz == NULL) return;

    desenharArvoreNoBuffer(ctx, raiz->esq, x, y);


    if (*y < ctx->screenSize.Y - 2) {
        char bufferTexto[64];

        snprintf(bufferTexto, sizeof(bufferTexto), "%-20s | Qtd: %d", raiz->ingrediente, raiz->quantidade);
        
        writeToBuffer(ctx, x, *y, bufferTexto, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        (*y)++;
    }

    desenharArvoreNoBuffer(ctx, raiz->dir, x, y);
}

void runTelaHistoricoIngredientes(GameContext *ctx, GameState *state) {
    BOOL inScreen = TRUE;


    while (inScreen) {

        clearBuffer(ctx);

        drawBox(ctx, 0, 0, ctx->screenSize.X - 1, ctx->screenSize.Y - 1, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        const char *titulo = "HISTORICO ACUMULADO DE INGREDIENTES";
        int tituloX = (ctx->screenSize.X - (int)strlen(titulo)) / 2;
        writeToBuffer(ctx, tituloX, 2, titulo, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        const char *subtitulo = "Ingrediente          | Quantidade";
        writeToBuffer(ctx, 10, 4, subtitulo, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
        writeToBuffer(ctx, 10, 5, "----------------------------------", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

        //Desenha a arvore.

        int linhaAtual = 6;

        if (state->raizIngredientes == NULL) {
            writeToBuffer(ctx, 10, linhaAtual, "Nenhum dado registrado ainda.", FOREGROUND_RED);
        } else {

            desenharArvoreNoBuffer(ctx, state->raizIngredientes, 10, &linhaAtual);
        }

        const char *msg = "Pressione [ENTER] para ir para a Loja";
        writeToBuffer(ctx, (ctx->screenSize.X - (int)strlen(msg)) / 2, ctx->screenSize.Y - 3, msg, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

        //Renderizar tudo na tela (Blit)
        blitToScreen(ctx);

        //Saida com ESC ou ENTER.
        DWORD numEvents = 0;
        GetNumberOfConsoleInputEvents(ctx->hConsoleIn, &numEvents);
        if (numEvents > 0) {
            INPUT_RECORD *eventBuffer = (INPUT_RECORD *)malloc(sizeof(INPUT_RECORD) * numEvents);
            DWORD eventsRead = 0;
            ReadConsoleInputA(ctx->hConsoleIn, eventBuffer, numEvents, &eventsRead);
            for (DWORD i = 0; i < eventsRead; ++i) {
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
                    WORD key = eventBuffer[i].Event.KeyEvent.wVirtualKeyCode;
                    if (key == VK_ESCAPE || key == VK_RETURN) {
                        inScreen = FALSE;
                    }
                }
            }
            free(eventBuffer);
        }

        //Delay necessário.
        Sleep(50);
    }
}

void drawMainMenu(GameContext *ctx, int selectedOption)
{
    clearBuffer(ctx);
    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;


    drawBox(ctx, 0, 0, width - 1, height - 1, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);


    const char *title = "=== PATO BURGER ===";
    const char *subtitle = "O Melhor Hamburguer do Lago";
    
    writeToBuffer(ctx, (width - (int)strlen(title)) / 2, height / 2 - 5, title, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, (width - (int)strlen(subtitle)) / 2, height / 2 - 4, subtitle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);


    const char *options[] = { "Carregar Jogo", "Novo Jogo", "Sair" };
    int numOptions = 3;

    for (int i = 0; i < numOptions; i++)
    {
        char buffer[64];
        WORD color = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE; // Branco padrão

        if (i == selectedOption)
        {
            // Opção selecionada fica brilhante e com setas
            color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            snprintf(buffer, sizeof(buffer), "> %s <", options[i]);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "  %s  ", options[i]);
        }

        writeToBuffer(ctx, (width - (int)strlen(buffer)) / 2, height / 2 - 1 + (i * 2), buffer, color);
    }
    

    const char *instr = "Use SETAS para mover e ENTER para selecionar";
    writeToBuffer(ctx, (width - (int)strlen(instr)) / 2, height - 3, instr, FOREGROUND_RED | FOREGROUND_GREEN);

    blitToScreen(ctx);
}

int runMainMenu(GameContext *ctx)
{
    int selectedOption = 1; //Default = Novo Jogo
    int numOptions = 3;
    BOOL inMenu = TRUE;


    while (inMenu)
    {
        drawMainMenu(ctx, selectedOption);

        DWORD numEvents = 0;
        GetNumberOfConsoleInputEvents(ctx->hConsoleIn, &numEvents);

        if (numEvents > 0)
        {
            INPUT_RECORD *eventBuffer = (INPUT_RECORD *)malloc(sizeof(INPUT_RECORD) * numEvents);
            DWORD eventsRead = 0;
            ReadConsoleInputA(ctx->hConsoleIn, eventBuffer, numEvents, &eventsRead);

            for (DWORD i = 0; i < eventsRead; ++i)
            {
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown)
                {
                    WORD vk = eventBuffer[i].Event.KeyEvent.wVirtualKeyCode;

                    if (vk == VK_UP)
                    {
                        selectedOption--;
                        if (selectedOption < 0) selectedOption = numOptions - 1;
                    }
                    else if (vk == VK_DOWN)
                    {
                        selectedOption++;
                        if (selectedOption >= numOptions) selectedOption = 0;
                    }
                    else if (vk == VK_RETURN)
                    {
                        free(eventBuffer);
                        // 0 = Carregar, 1 = Novo Jogo, 2 = Sair
                        if (selectedOption == 0) return 0; // Carregar Jogo
                        if (selectedOption == 1) return 1; // Novo Jogo
                        if (selectedOption == 2) exit(0);  // Sair
                    }
                }
            }
            free(eventBuffer);
        }
        Sleep(33); 
    }
    return 1;
}


void atualizarIngredientesDoDia(GameState *state) {
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Pao", state->pao_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Hamburguer", state->hamburguer_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Queijo", state->queijo_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Alface", state->alface_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Tomate", state->tomate_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Bacon", state->bacon_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Maionese do Pato", state->maioneseDoPato_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Onion Rings", state->onion_rings_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Cebola", state->cebola_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Picles", state->picles_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Falafel", state->falafel_vendidos);
    atualizar_quantidade_ingrediente(&state->raizIngredientes, "Frango", state->frango_vendidos);
}

void telaPrincipalEtapa2()
{
    GameContext gameContext = {0};
    GameState gameState = {0};

    Loja loja;
    Inventarioplayer inventarioJogador;

    // Menu handles
    gameContext.hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    gameContext.hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(gameContext.hConsoleIn, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(gameContext.hConsoleOut, &csbi);
    resizeBuffer(&gameContext, csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(gameContext.hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(gameContext.hConsoleOut, &cursorInfo);

    BOOL jogoRodando = TRUE;

    while (jogoRodando) {
        // Runs the Main Menu function we just added
        int choice = runMainMenu(&gameContext);

        inicializarInventario_loja(&inventarioJogador, 100);
        inicializarLoja(&loja);
        organizaloja(&loja);

        initializeGame(&gameContext, &gameState);

        // If choice was 0 (Carregar), load the game. Otherwise, it starts fresh (New Game)
        if (choice == 0) {
            carregarJogo(&gameState); 
        }

        BOOL showShopScreen = FALSE; 

        while (gameState.isRunning) {
            if (gameState.showEndScreen)
            {
                if (gameState.dinheiro <= 0)
                {
                    int acao = runEndScreen(&gameContext, &gameState);
                    if (acao == 1) gameState.showEndScreen = FALSE;
                    else if (acao == 2) gameState.isRunning = FALSE;
                    else {
                        gameState.isRunning = FALSE;
                        jogoRodando = FALSE; 
                    }
                }
                else
                {
                    atualizarIngredientesDoDia(&gameState); // Atualiza ANTES de mostrar
                    runTelaHistoricoIngredientes(&gameContext, &gameState);
                    gameState.showEndScreen = FALSE;
                    showShopScreen = TRUE;
                }
            }
            else if (showShopScreen)
            {
                salvarJogo(&gameState); 

                // atualiza dinheiro na struct da loja
                inventarioJogador.dinheiro = gameState.dinheiro;

                // atualizo o int quantidade direto pro print da loja ficar certo, sem precisar alocar memoria a toa
                inventarioJogador.paes.quantidade = gameState.pao_count;
                inventarioJogador.carnes.quantidade = gameState.hamburguerCru_count; 
                inventarioJogador.queijos.quantidade = gameState.queijo_count;
                inventarioJogador.alfaces.quantidade = gameState.alface_count;
                inventarioJogador.tomates.quantidade = gameState.tomate_count;
                inventarioJogador.bacons.quantidade = gameState.bacon_count;
                inventarioJogador.picles.quantidade = gameState.picles_count;
                inventarioJogador.cebolas.quantidade = gameState.cebola_count;
                inventarioJogador.falafels.quantidade = gameState.falafel_count;
                inventarioJogador.maionese_de_pato.quantidade = gameState.maioneseDoPato_count;
                inventarioJogador.onionRings.quantidade = gameState.onion_rings_count;
                inventarioJogador.frangos.quantidade = gameState.frango_count;
                
                loopfuncionaloja(&loja, &inventarioJogador); 

                // puxa o que comprou pro state do jogo
                gameState.dinheiro = inventarioJogador.dinheiro;
                gameState.pao_count = inventarioJogador.paes.quantidade;
                gameState.hamburguerCru_count = inventarioJogador.carnes.quantidade;
                gameState.queijo_count = inventarioJogador.queijos.quantidade;
                gameState.alface_count = inventarioJogador.alfaces.quantidade;
                gameState.tomate_count = inventarioJogador.tomates.quantidade;
                gameState.bacon_count = inventarioJogador.bacons.quantidade;
                gameState.picles_count = inventarioJogador.picles.quantidade;
                gameState.cebola_count = inventarioJogador.cebolas.quantidade;
                gameState.falafel_count = inventarioJogador.falafels.quantidade;
                gameState.maioneseDoPato_count = inventarioJogador.maionese_de_pato.quantidade;
                gameState.onion_rings_count = inventarioJogador.onionRings.quantidade;
                gameState.frango_count = inventarioJogador.frangos.quantidade;

                showShopScreen = FALSE; 
                clearStack(&gameState);
                initializeNextDay(&gameState); 
            }
            else
            {
                processInput(&gameContext, &gameState);
                updateGame(&gameState);
                if (gameState.showInstructions) drawInstructionsScreen(&gameContext, &gameState);
                else if (gameState.showCardapio) drawCardapioScreen(&gameContext, &gameState);
                else if (gameState.showCardapio_2) desenharCardapio_pagina2(&gameContext, &gameState);
                else renderGame(&gameContext, &gameState);
                Sleep(33); 
            }
        }
    }
    cleanup(&gameContext, &gameState);
}
