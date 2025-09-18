#ifndef GAME_H
#define GAME_H
// Estrutura para guardar o estado atual do jogo
typedef struct
{
	int dia; // Dia atual
	float dinheiro; // Dinheiro do player
} Estado;
// Protótipo da função definida em game.c
void inicializarEstado(Estado *estado);

#endif
