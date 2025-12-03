//
// Criado por lucas em 18/11/2025.
// Atualizado por Henrique em 18/11/2025.
//

#ifndef EXECUTOR_INSTRUCOES_H
#define EXECUTOR_INSTRUCOES_H

#include "frames.h"
#include "catalogoCodigosInstrucoes.h"
#include "formatoClassFile.h"

#define SYSTEM_OUT_REF 0xCAFE 

/*
 * Executor de instruções
 * ----------------------
 * Declara funções e tipos usados pelo executor de bytecodes:
 * - `InstrucaoFunc`: assinatura de funções que implementam instruções
 * - `inicializarInstrucoes`: monta a tabela de handlers de instruções
 * - `inicializarAmbiente`: prepara estruturas globais antes da execução
 * - `executar`: loop de execução de bytecodes a partir de um `Frame`
 * - `getMethodCode`: retorna o `code_attribute` de um método dado nome/descriptor
 * - `create_object_ref`: helper que simula/gera uma referência de objeto
 */
typedef void (*InstrucaoFunc)(Frame *frame);
extern InstrucaoFunc instrucoes_exec[256];
void inicializarInstrucoes(void);
void inicializarAmbiente(ClassFile *classFile);
void executar(Frame *frame);
code_attribute* getMethodCode(ClassFile *classFile, const char* name, const char* descriptor);
int create_object_ref(void);

#endif 

