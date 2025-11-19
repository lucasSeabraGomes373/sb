//
// Created by lucas on 18/11/2025.
// Updated by Henrique on 18/11/2025.
//

#ifndef EXECUTOR_INSTRUCOES_H
#define EXECUTOR_INSTRUCOES_H

#include "frames.h"
#include "catalogoCodigosInstrucoes.h"
#include "formatoClassFile.h"

#define SYSTEM_OUT_REF 0xCAFE 

typedef void (*InstrucaoFunc)(Frame *frame);
extern InstrucaoFunc instrucoes_exec[256];
void inicializarInstrucoes(void);
void inicializarAmbiente(ClassFile *classFile);
void executar(Frame *frame);
code_attribute* getMethodCode(ClassFile *classFile, const char* name, const char* descriptor);

#endif 

