#ifndef ESTOQUE_H
#define ESTOQUE_H
// Número máximo de tipos de ingredientes
#define MAX_INGREDIENTES 12
// Estrutura para representar um ingrediente
typedef struct
{
	int id;
	char nome[30];
	float preco;
	int quantidade; // Quantidade em estoque
} Ingrediente;
// Estrutura para representar o estoque completo
typedef struct
{
	Ingrediente ingredientes[MAX_INGREDIENTES];
} Estoque;
// Protótipos das funções definidas em estoque.c
void inicializarEstoque(Estoque *estoque);
void adicionarIngrediente(Estoque *estoque, int id, int quantidade);
void exibirEstoque(Estoque *estoque);
void removerIngrediente(Estoque *estoque, int id, int quantidade);

#endif
