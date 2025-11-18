//
// Created on 27/10/2025.
//

#ifndef SB_METODOINSTRUCOES_H
#define SB_METODOINSTRUCOES_H
#include "catalogoCodigosInstrucoes.h"

typedef struct instruction{
    char instr_name[50];
    byte1 opcode;
    byte1 numarg;
    int *tipoarg;
}instruction;


instruction* InstructionBuild(void);

#endif //SB_METODOINSTRUCOES_H

