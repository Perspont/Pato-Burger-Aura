//Aqui teremos o jogo principal. Isto inclui montagem de hamburgueres, tela, recebimento de pedidos, e sistema de passagem de dias INTERCONECTADO com o sistema de loja.

#include "../Header/gameplayEtapa2.h" // Agora inclui tudo (windows, loja, burgerLE)
#include <stdio.h>
#include <string.h>
#include <time.h>

// As definições de struct (GameContext, GameState, etc.) foram movidas para o .h

// --- Protótipos de Funções Internas (static) ---
// --- MODIFICADO: initializeGame agora aceita um flag ---
void initializeGame(GameContext *ctx, GameState *state, BOOL carregarSave);
void clearStack(GameState *state);
void resizeBuffer(GameContext *ctx, int width, int height);
void drawTimer(GameContext *ctx, GameState *state);
void drawCardapioScreen(GameContext *ctx, GameState *state);
void desenharCardapio_pagina2(GameContext *ctx, GameState *state);

// --- NOVO: Protótipo do menu de Continuar/Novo Jogo ---
static BOOL menuCarregarJogo(GameContext *ctx);


/*
 * ===================================================================
 * FUNÇÕES DE SALVAMENTO E CARREGAMENTO
 * ===================================================================
 */

/**
 * @brief Salva o estado atual do jogo em "Save/savegame.txt".
 */
void salvarProgresso(GameState *state) {
    FILE *f = fopen("Save/savegame.txt", "w"); 
    if (f == NULL) {
        return; 
    }
    fprintf(f, "dias: %d\n", state->dia);
    fprintf(f, "dinheiro: %.2f\n", state->inventario.dinheiro);
    fprintf(f, "vendidos: %d\n", state->hamburgueresVendidos);
    fprintf(f, "pao: %d\n", getQuantidadeInventario(&state->inventario, 1));
    fprintf(f, "carne: %d\n", getQuantidadeInventario(&state->inventario, 2));
    fprintf(f, "queijo: %d\n", getQuantidadeInventario(&state->inventario, 3));
    fprintf(f, "alface: %d\n", getQuantidadeInventario(&state->inventario, 4));
    fprintf(f, "tomate: %d\n", getQuantidadeInventario(&state->inventario, 5));
    fprintf(f, "bacon: %d\n", getQuantidadeInventario(&state->inventario, 6));
    fprintf(f, "picles: %d\n", getQuantidadeInventario(&state->inventario, 7));
    fprintf(f, "cebola: %d\n", getQuantidadeInventario(&state->inventario, 8));
    fprintf(f, "falafel: %d\n", getQuantidadeInventario(&state->inventario, 9));
    fprintf(f, "molho: %d\n", getQuantidadeInventario(&state->inventario, 10));
    fprintf(f, "onionrings: %d\n", getQuantidadeInventario(&state->inventario, 11));
    fprintf(f, "maionese: %d\n", getQuantidadeInventario(&state->inventario, 12));
    fprintf(f, "frango: %d\n", getQuantidadeInventario(&state->inventario, 13));
    fprintf(f, "hamburguerCru: %d\n", state->hamburguerCru_count);
    fprintf(f, "hamburguerGrelhado: %d\n", state->hamburguerGrelhado_count);
    fclose(f);
}

/**
 * @brief Carrega o estado do jogo de "Save/savegame.txt".
 */
void carregarProgresso(GameState *state) {
    FILE *f = fopen("Save/savegame.txt", "r");
    if (f == NULL) {
        return;
    }

    liberarInventario(&state->inventario);
    inicializarInventario(&state->inventario, state->inventario.dinheiro); 

    int dia, vendidos, cru, grelhado;
    double dinheiro;
    int inv[13] = {0}; 
    
    char linha[100];
    while (fgets(linha, sizeof(linha), f)) {
        if (sscanf(linha, "dias: %d", &dia) == 1) state->dia = dia;
        else if (sscanf(linha, "dinheiro: %lf", &dinheiro) == 1) state->inventario.dinheiro = dinheiro;
        else if (sscanf(linha, "vendidos: %d", &vendidos) == 1) state->hamburgueresVendidos = vendidos;
        else if (sscanf(linha, "hamburguerCru: %d", &cru) == 1) state->hamburguerCru_count = cru;
        else if (sscanf(linha, "hamburguerGrelhado: %d", &grelhado) == 1) state->hamburguerGrelhado_count = grelhado;
        else if (sscanf(linha, "pao: %d", &inv[0]) == 1) {}
        else if (sscanf(linha, "carne: %d", &inv[1]) == 1) {}
        else if (sscanf(linha, "queijo: %d", &inv[2]) == 1) {}
        else if (sscanf(linha, "alface: %d", &inv[3]) == 1) {}
        else if (sscanf(linha, "tomate: %d", &inv[4]) == 1) {}
        else if (sscanf(linha, "bacon: %d", &inv[5]) == 1) {}
        else if (sscanf(linha, "picles: %d", &inv[6]) == 1) {}
        else if (sscanf(linha, "cebola: %d", &inv[7]) == 1) {}
        else if (sscanf(linha, "falafel: %d", &inv[8]) == 1) {}
        else if (sscanf(linha, "molho: %d", &inv[9]) == 1) {}
        else if (sscanf(linha, "onionrings: %d", &inv[10]) == 1) {}
        else if (sscanf(linha, "maionese: %d", &inv[11]) == 1) {}
        else if (sscanf(linha, "frango: %d", &inv[12]) == 1) {}
    }
    fclose(f);

    for (int id = 1; id <= 13; id++) {
        int quantidade = inv[id - 1]; 
        for (int i = 0; i < quantidade; i++) {
            adicionarItemInventario(&state->inventario, id);
        }
    }
}

/**
 * @brief APAGA o arquivo "Save/savegame.txt".
 */
void apagarProgresso(void) {
    // Abrir o arquivo em modo "w" (escrita) apaga todo o seu conteúdo.
    FILE *f = fopen("Save/savegame.txt", "w");
    if (f != NULL) {
        fclose(f);
    }
}


/*
 * ===================================================================
 * FUNÇÕES DE LÓGICA DO JOGO (INTEGRAÇÃO DO SAVE)
 * ===================================================================
 */

// --- Utility Functions ---

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
        printf("Failed to allocate screen buffer!");
        exit(1);
    }
}

void clearBuffer(GameContext *ctx)
{
    if (!ctx->charBuffer) return;
    for (int i = 0; i < ctx->screenSize.X * ctx->screenSize.Y; ++i)
    {
        ctx->charBuffer[i].Char.AsciiChar = ' ';
        ctx->charBuffer[i].Attributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE; // Default: White
    }
}

void writeToBuffer(GameContext *ctx, int x, int y, const char *texto, WORD atributos)
{
    if (!ctx->charBuffer || x < 0 || y < 0 || x >= ctx->screenSize.X || y >= ctx->screenSize.Y)
    {
        return;
    }

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
        if (drawX + 2 > right)
        {
            drawX = left + 2;
            drawY++;
            if (drawY >= bottom - 1) break;
        }
        writeToBuffer(ctx, drawX, drawY, "\xFE ", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        drawX += 2;
    }

    if (drawX > left + 2)
    {
        drawY++;
    }

    drawX = left + 2;

    for (int i = 0; i < state->dynamicOrderCount; i++)
    {
        if (drawY >= bottom - 1) break;
        writeToBuffer(ctx, drawX, drawY, state->dynamicOrders[i].text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
        drawY++;
    }
}

void drawInput(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
    drawBox(ctx, left, top, right, bottom, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    writeToBuffer(ctx, left + 2, top, " COMMAND INPUT ", FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); // Yellow

    char prompt[MAX_COMMAND_LENGTH + 3];
    snprintf(prompt, sizeof(prompt), "> %s", state->currentCommand);
    writeToBuffer(ctx, left + 2, top + 2, prompt, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

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
    if (width < 10) width = 10;

    if (state->isGrilling)
    {
        ULONGLONG elapsed = GetTickCount64() - state->grillStartTime;
        float progress = (float)elapsed / GRILL_TIME_MS;
        if (progress > 1.0f) progress = 1.0f;

        int barLength = (int)(progress * width);

        char progressBar[256];
        memset(progressBar, 0, sizeof(progressBar));

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

    snprintf(text, sizeof(text), "dinheiro: $%.2f", state->inventario.dinheiro);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN);

    snprintf(text, sizeof(text), "Pao:    %d", getQuantidadeInventario(&state->inventario, 1));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Carne:  %d", getQuantidadeInventario(&state->inventario, 2));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Queijo: %d", getQuantidadeInventario(&state->inventario, 3));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Alface: %d", getQuantidadeInventario(&state->inventario, 4));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Tomate: %d", getQuantidadeInventario(&state->inventario, 5));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Bacon:  %d", getQuantidadeInventario(&state->inventario, 6));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Picles: %d", getQuantidadeInventario(&state->inventario, 7));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Cebola: %d", getQuantidadeInventario(&state->inventario, 8));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Falafel:%d", getQuantidadeInventario(&state->inventario, 9));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Molho:  %d", getQuantidadeInventario(&state->inventario, 10));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "O.Rings:%d", getQuantidadeInventario(&state->inventario, 11)); 
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Maionese:%d", getQuantidadeInventario(&state->inventario, 12));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    snprintf(text, sizeof(text), "Frango: %d", getQuantidadeInventario(&state->inventario, 13));
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    
    snprintf(text, sizeof(text), "Cru:     %d", state->hamburguerCru_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_RED | FOREGROUND_INTENSITY); 
    snprintf(text, sizeof(text), "Grelhado:%d", state->hamburguerGrelhado_count);
    writeToBuffer(ctx, left + 2, y++, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
}

void drawPilhaDeHamburguerLE_display(GameContext *ctx, GameState *state, int left, int top, int right, int bottom)
{
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
}

void drawTimer(GameContext *ctx, GameState *state)
{
    ULONGLONG elapsed = GetTickCount64() - state->gameStartTime;
    ULONGLONG remaining = (elapsed > GAME_DURATION_MS) ? 0 : (GAME_DURATION_MS - elapsed);

    int minutes = (int)(remaining / 60000);
    int seconds = (int)((remaining % 60000) / 1000);

    char timerText[32];
    snprintf(timerText, sizeof(timerText), " TIME: %02d:%02d ", minutes, seconds);

    int x = ctx->screenSize.X - (int)strlen(timerText) - 2;
    int y = 1;

    WORD attributes = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
    if (remaining < 30000)
    {
        attributes = (GetTickCount64() / 500) % 2 == 0
            ? (FOREGROUND_RED | FOREGROUND_INTENSITY)
            : (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }

    writeToBuffer(ctx, x, y, timerText, attributes);
}

void blitToScreen(GameContext *ctx)
{
    if (!ctx->charBuffer) return;

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

    // Coluna 1
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

    // Coluna 2
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

    // Coluna 1
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

    // Coluna 2
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

    snprintf(text, sizeof(text), "Final Score: $%.2f", state->inventario.dinheiro);
    writeToBuffer(ctx, (width - (int)strlen(text)) / 2, y, text, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    y += 2;

    snprintf(text, sizeof(text), "Hamburgueres Vendidos: %d", state->hamburgueresVendidos);
    writeToBuffer(ctx, (width - (int)strlen(text)) / 2, y, text, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    y += 3;


    const char *restart = "[R]estart";
    writeToBuffer(ctx, (width - (int)strlen(restart)) / 2, y, restart, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
    y += 2;

    const char *exitCmd = "[E]xit";
    writeToBuffer(ctx, (width - (int)strlen(exitCmd)) / 2, y, exitCmd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

    blitToScreen(ctx);
}


// --- Game Logic ---

// --- MODIFICADO: A função agora aceita um flag ---
void initializeGame(GameContext *ctx, GameState *state, BOOL carregarSave)
{
    memset(state, 0, sizeof(GameState));
    
    inicializarInventario(&state->inventario, 100.0); 
    inicializarLoja(&state->loja);
    popularLoja(&state->loja);
    
    state->hamburguerCru_count = 10;
    state->hamburguerGrelhado_count = 0;
    state->hamburgueresVendidos = 0;
    
    // --- LÓGICA DE CARREGAMENTO ATUALIZADA ---
    if (carregarSave) {
        // Tenta carregar o progresso se o usuário escolheu "Continuar"
        carregarProgresso(state);
    } else {
        // Apaga o progresso se o usuário escolheu "Novo Jogo"
        apagarProgresso();
    }
    // ------------------------------------------
    
    inicializar_BurgerLE_Player(&state->burgerPlayer);
    state->stackSize = 0;
    state->ordersPending = 0; 
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->isRunning = TRUE;
    state->showEndScreen = FALSE;
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE; 

    if (state->dia == 0) {
        state->dia = 1;
    }

    state->dynamicOrderCount = 0;
    state->lastDynamicOrderSpawn = GetTickCount64();
    state->nextIsPato = TRUE;
    state->spawnCycleCount = 0;
    for (int i = 0; i < MAX_DYNAMIC_ORDERS; i++)
    {
        state->dynamicOrders[i].spawnTime = 0;
        state->dynamicOrders[i].text[0] = '\0';
    }

    state->gameStartTime = GetTickCount64();

    // --- ATENÇÃO: Handles do console movidos para telaPrincipalEtapa2 ---
    // (Eles já foram inicializados antes desta função ser chamada)
    // ctx->hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    // ctx->hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // ...
    // CONSOLE_SCREEN_BUFFER_INFO csbi;
    // ...
    // resizeBuffer(...)

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
}


void clearStack(GameState *state)
{
    for (int i = 0; i < state->stackSize; i++)
    {
        free(state->PilhaDeHamburguerLE_display[i]);
    }
    
    inicializar_BurgerLE_Player(&state->burgerPlayer);
    state->stackSize = 0;
}

void processCommand(GameState *state)
{
    // --- NOVO COMANDO PARA SALVAR MANUALMENTE ---
    if (_stricmp(state->currentCommand, "salvar") == 0)
    {
        salvarProgresso(state);
        // (Nota: O jogo não tem um sistema de "pop-up" para dizer "Jogo Salvo!")
        // (O jogador terá que confiar que o comando funcionou)
    }
    // ------------------------------------------
    else if (_stricmp(state->currentCommand, "grelhar") == 0)
    {
        if (!state->isGrilling && state->hamburguerCru_count > 0)
        {
            state->hamburguerCru_count--;
            state->isGrilling = TRUE;
            state->grillStartTime = GetTickCount64();
        }
    }
    else if (_stricmp(state->currentCommand, "pao") == 0)
    {
        if (usarItemInventario(&state->inventario, 1))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Pao");
                adicionarIngredienteLE(&state->burgerPlayer, 1);
            } else {
                adicionarItemInventario(&state->inventario, 1);
            }
        }
    }
    else if (_stricmp(state->currentCommand, "alface") == 0)
    {
        if (usarItemInventario(&state->inventario, 4))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Alface");
                adicionarIngredienteLE(&state->burgerPlayer, 4);
            } else {
                adicionarItemInventario(&state->inventario, 4); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "tomate") == 0)
    {
        if (usarItemInventario(&state->inventario, 5))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Tomate");
                adicionarIngredienteLE(&state->burgerPlayer, 5);
            } else {
                adicionarItemInventario(&state->inventario, 5); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "queijo") == 0)
    {
         if (usarItemInventario(&state->inventario, 3))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Queijo");
                adicionarIngredienteLE(&state->burgerPlayer, 3);
            } else {
                adicionarItemInventario(&state->inventario, 3); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "hamburguer") == 0)
    {
        if (state->hamburguerGrelhado_count > 0)
        {
            state->hamburguerGrelhado_count--;
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Hamburguer Grelhado");
                adicionarIngredienteLE(&state->burgerPlayer, 2);
            } else {
                state->hamburguerGrelhado_count++;
            }
        }
    }
    else if (_stricmp(state->currentCommand, "bacon") == 0)
    {
         if (usarItemInventario(&state->inventario, 6))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Bacon");
                adicionarIngredienteLE(&state->burgerPlayer, 6);
            } else {
                adicionarItemInventario(&state->inventario, 6); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "maionese") == 0)
    {
         if (usarItemInventario(&state->inventario, 12))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Maionese do Pato");
                adicionarIngredienteLE(&state->burgerPlayer, 12);
            } else {
                adicionarItemInventario(&state->inventario, 12); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "onion_rings") == 0)
    {
         if (usarItemInventario(&state->inventario, 11))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Onion Rings");
                adicionarIngredienteLE(&state->burgerPlayer, 11);
            } else {
                adicionarItemInventario(&state->inventario, 11); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "cebola") == 0)
    {
         if (usarItemInventario(&state->inventario, 8))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Cebola");
                adicionarIngredienteLE(&state->burgerPlayer, 8);
            } else {
                adicionarItemInventario(&state->inventario, 8); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "picles") == 0)
    {
         if (usarItemInventario(&state->inventario, 7))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Picles");
                adicionarIngredienteLE(&state->burgerPlayer, 7);
            } else {
                adicionarItemInventario(&state->inventario, 7); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "falafel") == 0)
    {
         if (usarItemInventario(&state->inventario, 9))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Falafel");
                adicionarIngredienteLE(&state->burgerPlayer, 9);
            } else {
                adicionarItemInventario(&state->inventario, 9); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "frango") == 0)
    {
         if (usarItemInventario(&state->inventario, 13))
        {
            if (state->stackSize < MAX_BURGER_STACK) {
                state->PilhaDeHamburguerLE_display[state->stackSize++] = _strdup("Frango");
                adicionarIngredienteLE(&state->burgerPlayer, 13);
            } else {
                adicionarItemInventario(&state->inventario, 13); 
            }
        }
    }
    else if (_stricmp(state->currentCommand, "servir") == 0)
    {
        if (state->stackSize > 0 && state->ordersPending > 0)
        {
            clearStack(state);
            state->ordersPending--;
            state->inventario.dinheiro += 10;
            state->hamburgueresVendidos++; 
        }
    }
    else if (_stricmp(state->currentCommand, "lixo") == 0)
    {
        clearStack(state);
    }

    else if (_stricmp(state->currentCommand, "cardapio") == 0)
    {
        state->showCardapio = TRUE;
        state->showCardapio_2 = FALSE;
    }
    else if (_stricmp(state->currentCommand, "sair") == 0)
    {
        state->isRunning = FALSE;
    }

    state->commandLength = 0;
    state->currentCommand[0] = '\0';
}

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

                if (state->showCardapio)
                {
                    if (c == 'v' || c == 'V') state->showCardapio = FALSE;
                    else if (c == 'p' || c == 'P')
                    {
                        state->showCardapio = FALSE;
                        state->showCardapio_2 = TRUE;
                    }
                }
                else if (state->showCardapio_2)
                {
                    if (c == 'v' || c == 'V') state->showCardapio_2 = FALSE;
                    else if (c == 'a' || c == 'A')
                    {
                        state->showCardapio_2 = FALSE;
                        state->showCardapio = TRUE;
                    }
                }
                else
                {
                    if (c == '\r')
                    {
                        processCommand(state);
                    }
                    else if (c == '\b')
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

        case WINDOW_BUFFER_SIZE_EVENT:
        {
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
        ULONGLONG now = GetTickCount64();
        if (now - state->grillStartTime >= GRILL_TIME_MS)
        {
            state->isGrilling = FALSE;
            state->hamburguerGrelhado_count++;
        }
    }

    ULONGLONG now = GetTickCount64();

    int i = 0;
    while (i < state->dynamicOrderCount)
    {
        if (now - state->dynamicOrders[i].spawnTime >= 10000)
        {
            for (int j = i; j < state->dynamicOrderCount - 1; j++)
            {
                state->dynamicOrders[j] = state->dynamicOrders[j + 1];
            }
            state->dynamicOrderCount--;
        }
        else
        {
            i++;
        }
    }

    if (now - state->lastDynamicOrderSpawn >= 2000)
    {
        state->lastDynamicOrderSpawn = now;

        if (state->dynamicOrderCount >= 3)
        {
            state->dynamicOrderCount = 2;
        }

        for (int j = state->dynamicOrderCount; j > 0; j--)
        {
            state->dynamicOrders[j] = state->dynamicOrders[j - 1];
        }

        state->dynamicOrders[0].spawnTime = now;

        if (state->nextIsPato)
        {
            strcpy(state->dynamicOrders[0].text, "pato");
            state->spawnCycleCount++;
            if (state->spawnCycleCount >= 3)
            {
                state->nextIsPato = FALSE;
                state->spawnCycleCount = 0;
            }
        }
        else
        {
            strcpy(state->dynamicOrders[0].text, "guaxinim");
            state->spawnCycleCount++;
            if (state->spawnCycleCount >= 3)
            {
                state->nextIsPato = TRUE;
                state->spawnCycleCount = 0;
            }
        }
        state->dynamicOrderCount++;
    }

    state->ordersPending = state->dynamicOrderCount;

    if (state->inventario.dinheiro <= 0)
    {
        state->showEndScreen = TRUE;
        return;
    }

    ULONGLONG elapsed = GetTickCount64() - state->gameStartTime;
    if (elapsed >= GAME_DURATION_MS)
    {
        state->showEndScreen = TRUE;
    }
}

void renderGame(GameContext *ctx, GameState *state)
{
    if (!ctx->charBuffer) return;

    clearBuffer(ctx);

    int orderBoxL = 1;
    int orderBoxT = 1;
    int orderBoxR = ctx->screenSize.X / 3;
    int orderBoxB = ctx->screenSize.Y / 3;
    drawOrders(ctx, state, orderBoxL, orderBoxT, orderBoxR, orderBoxB);

    int inputL = 1;
    int inputT = orderBoxB + 1;
    int inputR = orderBoxR;
    int inputB = inputT + 4;
    drawInput(ctx, state, inputL, inputT, inputR, inputB);

    int grillL = 1;
    int grillT = inputB + 1;
    int grillR = orderBoxR;
    int grillB = grillT + 6;
    drawGrilling(ctx, state, grillL, grillT, grillR, grillB);

    int invL = 1;
    int invB = ctx->screenSize.Y - 2;
    int invT = max(grillB + 1, invB - 17); // Aumentado para 17
    int invR = orderBoxR;
    drawInventory(ctx, state, invL, invT, invR, invB);

    int stackL = orderBoxR + 2;
    int stackT = 1;
    int stackR = ctx->screenSize.X - 2;
    int stackB = ctx->screenSize.Y - 2;
    drawPilhaDeHamburguerLE_display(ctx, state, stackL, stackT, stackR, stackB);

    drawTimer(ctx, state);

    blitToScreen(ctx);
}


void initializeNextDay(GameState *state)
{
    clearStack(state);
    state->ordersPending = 0;
    state->isGrilling = FALSE;
    state->commandLength = 0;
    state->currentCommand[0] = '\0';
    state->showEndScreen = FALSE;
    state->showCardapio = FALSE;
    state->showCardapio_2 = FALSE;

    state->dynamicOrderCount = 0;
    state->lastDynamicOrderSpawn = GetTickCount64();
    state->nextIsPato = TRUE;
    state->spawnCycleCount = 0;
    for (int i = 0; i < MAX_DYNAMIC_ORDERS; i++)
    {
        state->dynamicOrders[i].spawnTime = 0;
        state->dynamicOrders[i].text[0] = '\0';
    }

    state->gameStartTime = GetTickCount64();
    state->dia++;
}

BOOL runEndScreen(GameContext *ctx, GameState *state)
{
    BOOL inEndScreen = TRUE;
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
                                // Reinicia o jogo (apaga o save)
                                apagarProgresso(); // Limpa o save antigo

                                liberarLoja(&state->loja);
                                liberarInventario(&state->inventario);
                                clearStack(state);
                                
                                // Reinicializa o jogo SEM carregar
                                initializeGame(ctx, state, FALSE); 
                                return TRUE; 
                            }
                        if (c == 'e' || c == 'E')
                        {
                            return FALSE;
                        }
                    }
                    break;
                case WINDOW_BUFFER_SIZE_EVENT:
                {
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
    return FALSE;
}

void cleanup(GameContext *ctx, GameState *state)
{
    salvarProgresso(state);
    
    liberarLoja(&state->loja);
    liberarInventario(&state->inventario);
    clearStack(state);
    
    if (ctx->charBuffer)
    {
        free(ctx->charBuffer);
    }

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(ctx->hConsoleOut, &cursorInfo);

    system("cls");
    printf("Burger Boss exited. Thanks for playing!\n");
}


/*
 * ===================================================================
 * NOVO MENU DE CARREGAMENTO (Chamado por telaPrincipalEtapa2)
 * ===================================================================
 */
static BOOL menuCarregarJogo(GameContext *ctx) {
    clearBuffer(ctx);
    int width = ctx->screenSize.X;
    int height = ctx->screenSize.Y;

    WORD branco = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE;
    WORD amarelo = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    drawBox(ctx, 0, 0, width - 1, height - 1, branco);
    
    const char *titulo = "PATO BURGER";
    const char *opt1 = "[1] Continuar Jogo";
    const char *opt2 = "[2] Novo Jogo (Apaga o save anterior)";
    
    writeToBuffer(ctx, (width - (int)strlen(titulo)) / 2, height / 2 - 3, titulo, amarelo);
    writeToBuffer(ctx, (width - (int)strlen(opt1)) / 2, height / 2, opt1, branco);
    writeToBuffer(ctx, (width - (int)strlen(opt2)) / 2, height / 2 + 1, opt2, branco);

    blitToScreen(ctx);

    DWORD numEvents = 0;
    DWORD eventsRead = 0;
    INPUT_RECORD eventBuffer[16];

    while (1) { // Loop até o usuário pressionar 1 ou 2
        GetNumberOfConsoleInputEvents(ctx->hConsoleIn, &numEvents);
        if (numEvents > 0) {
            ReadConsoleInputA(ctx->hConsoleIn, eventBuffer, 16, &eventsRead);
            for (DWORD i = 0; i < eventsRead; ++i) {
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
                    char c = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
                    if (c == '1') {
                        return TRUE; // Continuar (carregar save)
                    }
                    if (c == '2') {
                        return FALSE; // Novo Jogo (não carregar save)
                    }
                }
            }
        }
        Sleep(50); // Evita uso excessivo de CPU
    }
}


// --- Main Function ---
void telaPrincipalEtapa2()
{
    GameContext gameContext = {0};
    GameState gameState = {0};
    BOOL carregarSave = TRUE; // Flag para carregar o jogo

    // --- SETUP INICIAL DO CONSOLE (Necessário para o menu) ---
    // (Isto foi movido de initializeGame para cá)
    gameContext.hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    gameContext.hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(gameContext.hConsoleOut, &csbi);
    resizeBuffer(&gameContext, csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    // ---------------------------------------------------------

    // --- NOVO MENU ---
    // Chama o menu de Continuar/Novo Jogo ANTES de inicializar
    carregarSave = menuCarregarJogo(&gameContext);
    // -----------------

    // Inicializa o jogo, passando a decisão do usuário
    initializeGame(&gameContext, &gameState, carregarSave);

    BOOL showShopScreen = FALSE;

    while (gameState.isRunning) {
        if (gameState.showEndScreen)
        {
            if (gameState.inventario.dinheiro <= 0)
            {
                // CONDIÇÃO 1: Dinheiro acabou = GAME OVER
                if (runEndScreen(&gameContext, &gameState))
                {
                    gameState.showEndScreen = FALSE;
                }
                else
                {
                    gameState.isRunning = FALSE;
                }
            }
            else
            {
                // CONDIÇÃO 2: Dia acabou, mas temos dinheiro = IR PARA A LOJA
                gameState.showEndScreen = FALSE;
                showShopScreen = TRUE;
            }
        }
        else if (showShopScreen)
        {
            // --- CENA DA LOJA ---
            loopPrincipalLoja(&gameState.loja, &gameState.inventario); 

            // Salva o progresso no final do dia
            salvarProgresso(&gameState);

            showShopScreen = FALSE;
            initializeNextDay(&gameState);
        }
        else
        {
            // --- CENA DE GAMEPLAY (DIA NORMAL) ---
            processInput(&gameContext, &gameState);
            updateGame(&gameState);

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

            Sleep(33); // ~30 FPS
        }
    }

    // Cleanup (agora também salva o jogo)
    cleanup(&gameContext, &gameState);

}