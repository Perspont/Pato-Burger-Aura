#include <stdio.h>
#include "../Header/burgerLE.h"


BurgerLE_Player *inicializar_BurgerLE_Player() {
    BurgerLE_Player *burger;
    burger->ingredientes = inicializa_pilha_LE();
    return *burger;
}