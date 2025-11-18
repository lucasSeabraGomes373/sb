//
// Created by lucas on 18/11/2025.
//

#ifndef EXECUTOR_INSTRUCOES_H
#define EXECUTOR_INSTRUCOES_H

#include "frame.h"
#include "catalogoCodigosInstrucoes.h"

// Tipo de função para execução de instruções
typedef void (*InstrucaoFunc)(Frame *frame);

// Tabela de funções de execução indexada por opcode
extern InstrucaoFunc instrucoes_exec[256];

// Inicializa o vetor de ponteiros com as funções de execução
void inicializarInstrucoes(void);

// Executa o loop principal de instruções de um frame
void executar(Frame *frame);

#endif // EXECUTOR_INSTRUCOES_H

