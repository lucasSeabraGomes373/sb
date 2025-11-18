//
// Created by lucas on 18/11/2025.
//
#include <stdio.h>
#include "executorInstrucoes.h"

// Vetor global de ponteiros para funções
InstrucaoFunc instrucoes_exec[256];

// ---------------------- Funções de execução ----------------------

void exec_iconst_0(Frame *frame) {
    frame->operand_stack[++frame->sp] = 0;
}

void exec_iadd(Frame *frame) {
    int v2 = frame->operand_stack[frame->sp--];
    int v1 = frame->operand_stack[frame->sp--];
    frame->operand_stack[++frame->sp] = v1 + v2;
}

void exec_return(Frame *frame) {
    frame->pc = frame->code_length; // Finaliza execução
}

// ---------------------- Inicialização ----------------------

void inicializarInstrucoes(void) {
    for (int i = 0; i < 256; i++) {
        instrucoes_exec[i] = NULL;
    }

    instrucoes_exec[iconst_0] = exec_iconst_0;
    instrucoes_exec[iadd] = exec_iadd;
    instrucoes_exec[inst_return] = exec_return;
}

// ---------------------- Loop de execução ----------------------

void executar(Frame *frame) {
    while (frame->pc < frame->code_length) {
        byte1 opcode = frame->code[frame->pc++];
        InstrucaoFunc func = instrucoes_exec[opcode];
        if (func) {
            func(frame);
        } else {
            printf("Instrução %02x não implementada\n", opcode);
            break;
        }
    }
}

