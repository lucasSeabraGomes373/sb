#include "leitor.h"
#include <stdio.h>

// General utility functions

int setOffsetPrinting(int posicao, byte1 offset) {
    if (posicao == 0) {
        return offset;
    } else {
        return (offset + 1);
    }
}