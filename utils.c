#include "leitor.h"
#include <stdio.h>

/*
 * Funções utilitárias gerais
 * -------------------------
 * Funções auxiliares pequenas usadas por várias partes do projeto
 * (por exemplo, formatação de offsets). Mantemos aqui helpers que
 * não pertencem a um módulo maior.
 */

int setOffsetPrinting(int posicao, byte1 offset) {
    if (posicao == 0) {
        return offset;
    } else {
        return (offset + 1);
    }
}