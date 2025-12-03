//
// Criado em 27/10/2025.
//

#ifndef SB_METODOINSTRUCOES_H
#define SB_METODOINSTRUCOES_H
#include "catalogoCodigosInstrucoes.h"

/*
 * Estrutura e construção da tabela de instruções
 * ---------------------------------------------
 * `instruction` descreve metadados de cada opcode (nome, opcode numerico,
 * número de argumentos e tipos de argumentos). A função `InstructionBuild`
 * cria e inicializa a tabela de todas as instruções suportadas.
 */
typedef struct instruction{
    char instr_name[50];
    byte1 opcode;
    byte1 numarg;
    int *tipoarg;
}instruction;


instruction* InstructionBuild(void);

#endif //SB_METODOINSTRUCOES_H

