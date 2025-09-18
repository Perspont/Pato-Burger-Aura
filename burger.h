#ifndef BURGER_H
#define BURGER_H
// Número máximo de hambúrgueres no cardápio
#define MAX_BURGERS 10
// Estrutura para representar um hambúrguer
typedef struct
{
	int id;
	char nome[30];
	float preco;
	int ingredientes[10]; // Array com os IDs dos ingredientes necessários
} Burger;

// Protótipos das funções definidas em burger.c
void inicializarCardapio(Burger *cardapio);
void exibirCardapio(Burger *cardapio);


#endif
