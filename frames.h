//
// Created by lucas on 18/11/2025.
//

#ifndef FRAME_H
#define FRAME_H

#include "byteTypes.h"
#include "constantPool.h"

// Estrutura de um frame de execução
typedef struct {
    byte4 pc;                  // Program counter (posição atual no bytecode)
    byte2 max_stack;           // Tamanho máximo da pilha de operandos
    byte2 max_locals;          // Tamanho máximo das variáveis locais
    byte1 *code;               // Bytecode do método
    int *operand_stack;        // Pilha de operandos
    int *local_variables;      // Vetor de variáveis locais
    int sp;                    // Stack pointer (topo da pilha)
} Frame;

// Estrutura de uma thread de execução (única para este projeto)
typedef struct {
    Frame *current_frame;      // Frame atual em execução
} Thread;

#endif // FRAME_H

