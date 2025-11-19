//
// Created by lucas on 18/11/2025.
// Updated by Henrique on 18/11/2025
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "executorInstrucoes.h"
#include "leitor.h"

// Variável global para armazenar a Constant Pool (necessária para resolver referências em tempo de execução)
static cp_info *GLOBAL_CP = NULL;
static byte2 GLOBAL_CP_COUNT = 0;

// Vetor global de ponteiros para funções
InstrucaoFunc instrucoes_exec[256];

// ---------------------- Funções de execução ----------------------

int pop_stack(Frame *frame) {
    if (frame->sp < 0) {
        fprintf(stderr, "Erro de Stack Overflow/Underflow.\n");
        exit(EXIT_FAILURE);
    }
    return frame->operand_stack[frame->sp--];
}

void push_stack(Frame *frame, int value) {
    if (frame->sp >= frame->max_stack - 1) {
        fprintf(stderr, "Erro: Stack Overflow. Max stack: %d\n", frame->max_stack);
        exit(EXIT_FAILURE);
    }
    frame->operand_stack[++frame->sp] = value;
}


void exec_iconst_0(Frame *frame) {
    push_stack(frame, 0);
}

void exec_iadd(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 + v2);
}

void exec_return(Frame *frame) {
    frame->pc = frame->code_length;
}

// ---------------------- NOVAS INSTRUÇÕES MÍNIMAS (I/O) ----------------------

void exec_ldc(Frame *frame) {
    byte1 index = frame->code[frame->pc++];

    cp_info *cp_entry = GLOBAL_CP + index - 1;
    
    if (cp_entry->tag == CONSTANT_Integer) {
        push_stack(frame, decodeIntegerInfo(cp_entry));
    } else if (cp_entry->tag == CONSTANT_String) {
        push_stack(frame, cp_entry->UnionCP.CONSTANT_String.string_index);
    } else {
        fprintf(stderr, "Erro: LDC para tag %d não suportada.\n", cp_entry->tag);
        exit(EXIT_FAILURE);
    }
}

void exec_getstatic(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    char *ref_string = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    
    if (strstr(ref_string, "java/lang/System.out")) {
        push_stack(frame, SYSTEM_OUT_REF);
    } else {
        fprintf(stderr, "Erro: GETSTATIC em campo estático não suportado: %s\n", ref_string);
        exit(EXIT_FAILURE);
    }

    free(ref_string);
}

void exec_invokevirtual(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    char *method_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    
    if (strstr(method_ref, "println:(Ljava/lang/String;)V")) {
        int string_index = pop_stack(frame);
        int out_ref = pop_stack(frame); // Deve ser SYSTEM_OUT_REF
        
        if (out_ref == SYSTEM_OUT_REF) {
            cp_info *cp_entry = GLOBAL_CP + string_index - 1;
            if (cp_entry->tag == CONSTANT_String) {
                cp_info *string_value_entry = GLOBAL_CP + cp_entry->UnionCP.CONSTANT_String.string_index - 1;
                char *output_str = decodeStringUTF8(string_value_entry);
                printf("%s\n", output_str);
                free(output_str);
            } else {
                printf("Valor (int/ref) na pilha: %d\n", string_index); 
            }
        }
    } else {
        fprintf(stderr, "Erro: INVOKEVIRTUAL para método %s não suportado.\n", method_ref);
        exit(EXIT_FAILURE);
    }
    
    free(method_ref);
}

// ---------------------- Funções de Setup/Busca ----------------------

// Função de inicialização do ambiente de execução (Global CP)
void inicializarAmbiente(ClassFile *classFile) {
    GLOBAL_CP = classFile->constant_pool;
    GLOBAL_CP_COUNT = classFile->constant_pool_count;
    // Aqui iriam as inicializações de Statics
}

code_attribute* getMethodCode(ClassFile *classFile, const char* name, const char* descriptor) {
    for (int i = 0; i < classFile->methods_count; i++) {
        method_info *method = &classFile->methods[i];
        
        char *method_name = decodeStringUTF8(classFile->constant_pool + method->name_index - 1);
        char *method_desc = decodeStringUTF8(classFile->constant_pool + method->descriptor_index - 1);
        
        if (strcmp(method_name, name) == 0 && strcmp(method_desc, descriptor) == 0) {
            free(method_name);
            free(method_desc);
            
            // Busca o Code Attribute dentro do método
            for (int j = 0; j < method->attributes_count; j++) {
                attribute_info *attr = method->attributes[j];
                char *attr_name = decodeStringUTF8(classFile->constant_pool + attr->attribute_name_index - 1);
                
                if (strcmp(attr_name, "Code") == 0) {
                    free(attr_name);
                    return (code_attribute*)attr->info;
                }
                free(attr_name);
            }
        }
        free(method_name);
        free(method_desc);
    }
    return NULL;
}

// ---------------------- Inicialização ----------------------

void inicializarInstrucoes(void) {
    for (int i = 0; i < 256; i++) {
        instrucoes_exec[i] = NULL;
    }

    // Instruções implementadas
    instrucoes_exec[nop] = NULL; // NOP
    instrucoes_exec[iconst_0] = exec_iconst_0;
    instrucoes_exec[iadd] = exec_iadd;
    instrucoes_exec[inst_return] = exec_return;

    // Novos Mínimos
    instrucoes_exec[ldc] = exec_ldc;
    instrucoes_exec[getstatic] = exec_getstatic;
    instrucoes_exec[invokevirtual] = exec_invokevirtual;
    
    // Placeholder para opcodes comuns (para evitar falha imediata)
    instrucoes_exec[iload] = NULL;
    instrucoes_exec[aload_0] = NULL;
    // ... muitos outros seriam necessários
}

// ---------------------- Loop de execução ----------------------

void executar(Frame *frame) {
    while (frame->pc < frame->code_length) {
        byte1 opcode = frame->code[frame->pc++];
        InstrucaoFunc func = instrucoes_exec[opcode];
        if (func) {
            func(frame);
        } else {
            printf("Instrução %02x não implementada (PC: %d)\n", opcode, frame->pc - 1);
            break;
        }
    }
}
