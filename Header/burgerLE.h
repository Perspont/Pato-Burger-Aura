#ifndef BURGERDE_H
#define BURGERDE_H

#include "pilhaLE.h"

typedef struct
{
    int id;
    char nome[30];
    float preco;
    tp_pilha ingredientes;
} BurgerLE;

#endif
