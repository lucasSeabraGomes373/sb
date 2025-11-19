//
// Created by lucas on 18/11/2025.
// Updated by Henrique on 18/11/2025
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "executorInstrucoes.h"
#include "leitor.h" // Incluído para decodeStringUTF8, decodeIntegerInfo, etc.
#include "catalogoCodigosInstrucoes.h" // Para os enums dos opcodes

// Variável global para armazenar a Constant Pool
static cp_info *GLOBAL_CP = NULL;
static byte2 GLOBAL_CP_COUNT = 0;

// Vetor global de ponteiros para funções
InstrucaoFunc instrucoes_exec[256];

// ---------------------- Funções Auxiliares (Pop/Push) ----------------------

int pop_stack(Frame *frame) {
    if (frame->sp < 0) {
        fprintf(stderr, "Erro de Stack Underflow (PC: %d).\n", frame->pc);
        exit(EXIT_FAILURE);
    }
    return frame->operand_stack[frame->sp--];
}

void push_stack(Frame *frame, int value) {
    if (frame->sp >= frame->max_stack - 1) {
        fprintf(stderr, "Erro: Stack Overflow. Max stack: %d (PC: %d)\n", frame->max_stack, frame->pc);
        exit(EXIT_FAILURE);
    }
    frame->operand_stack[++frame->sp] = value;
}

// ---------------------- Implementações de Execução Básica ----------------------

void exec_iconst_0(Frame *frame) {
    push_stack(frame, 0);
}

void exec_iadd(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 + v2);
}

void exec_return(Frame *frame) {
    frame->pc = frame->code_length; // Finaliza execução deste método
    printf("--- RETORNO do Método (RETURN) ---\n");
}

// ---------------------- NOVAS IMPLEMENTAÇÕES DE ACESSO LOCAL ----------------------

// ALOAD_0 (0x2A) - Carrega a referência local 0 ('this')
void exec_aload_0(Frame *frame) {
    // Referência está no slot 0 de local_variables (configurado no main.c)
    push_stack(frame, frame->local_variables[0]);
}

// ILOAD_1 (0x1B) - Carrega int local 1
void exec_iload_1(Frame *frame) {
    push_stack(frame, frame->local_variables[1]);
}

// ALOAD_2 (0x2C) - Carrega ref local 2
void exec_aload_2(Frame *frame) {
    push_stack(frame, frame->local_variables[2]);
}

// ILOAD_3 (0x1D) - Carrega int local 3
void exec_iload_3(Frame *frame) {
    push_stack(frame, frame->local_variables[3]);
}

void exec_istore_2(Frame *frame) {
    int value = pop_stack(frame);
    frame->local_variables[2] = value;
}

// ALOAD_3 (0x2D) - Carrega ref local 3
void exec_aload_3(Frame *frame) {
    push_stack(frame, frame->local_variables[3]);
}

// ---------------------- CONTROLE DE OBJETOS E CAMPOS ----------------------

// PUTFIELD (0xB5) - Grava valor em campo de instância
void exec_putfield(Frame *frame) {
    // Opcodes com argumentos precisam ler os bytes seguintes do bytecode
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2; // Avança PC pelos 2 bytes de argumento
    
    int value = pop_stack(frame); // Valor a ser escrito
    int obj_ref = pop_stack(frame); // Referência do objeto
    
    char *field_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);

    // SIMULAÇÃO: Na JVM real, o valor seria gravado na HEAP.
    // Aqui imprimimos para provar que a instrução funcionou.
    printf("--- [PUTFIELD] Gravando valor %d no campo %s do Objeto Ref: %d ---\n", value, field_ref, obj_ref);

    free(field_ref);
}

void exec_newarray(Frame *frame) {
    int count = pop_stack(frame); // Número de elementos no array
    byte1 atype = frame->code[frame->pc++]; // Tipo do array (byte, int, etc.)
    printf("--- [NEWARRAY] Criando array de tipo %d com tamanho %d ---\n", atype, count);
    // Simulação: empilha uma referência fictícia para o array
    push_stack(frame, 99); // 99 é uma referência fictícia
}

void exec_putstatic(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;
    char *field_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    printf("Aviso: PUTSTATIC %s ignorado (simulação).\n", field_ref);
    free(field_ref);
}

// INVOKESPECIAL (0xB7) - Chama construtores/métodos privados
void exec_invokespecial(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    char *method_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    
    // Construtores: remove 'this' e argumentos da pilha
    if (strstr(method_ref, "<init>")) {
        // Simplificação: assume construtor de Object (sem args) ou remove apenas o 'this'
        int object_ref = pop_stack(frame);
        printf("--- [INVOKESPECIAL] Chamando construtor %s para Objeto Ref: %d ---\n", method_ref, object_ref);
    } else {
        fprintf(stderr, "Erro: INVOKESPECIAL para método %s não suportado. (PC: %d)\n", method_ref, frame->pc - 3);
        free(method_ref);
        exit(EXIT_FAILURE);
    }
    free(method_ref);
}

// NEW (0xBB) - Simulação básica se necessário
void exec_new(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;
    
    // Apenas empilha uma referência fictícia nova (ex: 2)
    push_stack(frame, 2); 
    printf("--- [NEW] Criando objeto simulado Ref: 2 ---\n");
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

void exec_bipush(Frame *frame) {
    int value = (signed char)frame->code[frame->pc++];
    push_stack(frame, value);
}

void exec_astore_1(Frame *frame) {
    int ref = pop_stack(frame);
    frame->local_variables[1] = ref;
}

void exec_getstatic(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    char *ref_string = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    
    if (strstr(ref_string, "java/lang/System.out")) {
        push_stack(frame, SYSTEM_OUT_REF);
    } else {
        // Apenas ignora ou avisa, para não travar em outros estáticos
        printf("Aviso: GETSTATIC %s simulado (push 0).\n", ref_string);
        push_stack(frame, 0);
    }

    free(ref_string);
}

void exec_astore_0(Frame *frame) {
    int ref = pop_stack(frame);
    frame->local_variables[0] = ref;
}

void exec_aload_1(Frame *frame) {
    push_stack(frame, frame->local_variables[1]);
}

void exec_dup(Frame *frame) {
    int value = frame->operand_stack[frame->sp];
    push_stack(frame, value);
}

void exec_iload_0(Frame *frame) {
    push_stack(frame, frame->local_variables[0]);
}

void exec_iload_2(Frame *frame) {
    push_stack(frame, frame->local_variables[2]);
}

void exec_invokevirtual(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    char *method_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    
    if (strstr(method_ref, "println")) {
        int arg = pop_stack(frame);
        int ref = pop_stack(frame); // Deve ser SYSTEM_OUT_REF
        
        if (ref == SYSTEM_OUT_REF) {
             printf("[OUTPUT]: %d (ou String Ref)\n", arg);
        }
    } else {
        printf("Aviso: INVOKEVIRTUAL %s ignorado (pop args).\n", method_ref);
        pop_stack(frame); // Tenta limpar pilha
    }
    
    free(method_ref);
}

void exec_invokestatic(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2; // Avança o PC
    char *method_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    printf("--- [INVOKESTATIC] Chamando método estático: %s ---\n", method_ref);
    free(method_ref);
    // Simulação: não faz nada além de imprimir
}

// ---------------------- Funções de Setup/Busca ----------------------

void inicializarAmbiente(ClassFile *classFile) {
    GLOBAL_CP = classFile->constant_pool;
    GLOBAL_CP_COUNT = classFile->constant_pool_count;
}

code_attribute* getMethodCode(ClassFile *classFile, const char* name, const char* descriptor) {
    for (int i = 0; i < classFile->methods_count; i++) {
        method_info *method = &classFile->methods[i];
        
        char *method_name = decodeStringUTF8(classFile->constant_pool + method->name_index - 1);
        char *method_desc = decodeStringUTF8(classFile->constant_pool + method->descriptor_index - 1);
        
        if (strcmp(method_name, name) == 0 && strcmp(method_desc, descriptor) == 0) {
            free(method_name);
            free(method_desc);
            
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

// ---------------------- Inicialização da Tabela ----------------------

void inicializarInstrucoes(void) {
    for (int i = 0; i < 256; i++) {
        instrucoes_exec[i] = NULL;
    }

    // Instruções Básicas
    instrucoes_exec[nop] = NULL; 
    instrucoes_exec[iconst_0] = exec_iconst_0;
    instrucoes_exec[iadd] = exec_iadd;
    instrucoes_exec[inst_return] = exec_return;     

    // Acesso Local (para o construtor da Carta e outros)
    instrucoes_exec[aload_0] = exec_aload_0; 
    instrucoes_exec[aload_1] = exec_aload_1; 
    instrucoes_exec[aload_2] = exec_aload_2; 
    instrucoes_exec[aload_3] = exec_aload_3;

    instrucoes_exec[iload_0] = exec_iload_0;
    instrucoes_exec[iload_1] = exec_iload_1; 
    instrucoes_exec[iload_2] = exec_iload_2;
    instrucoes_exec[iload_3] = exec_iload_3; 

    instrucoes_exec[astore_0] = exec_astore_0;
    instrucoes_exec[astore_1] = exec_astore_1;
    instrucoes_exec[istore_2] = exec_istore_2; 

    // Controle de Objeto
    instrucoes_exec[dup] = exec_dup;         
    instrucoes_exec[inst_new] = exec_new; 
    instrucoes_exec[putfield] = exec_putfield; 
    instrucoes_exec[putstatic] = exec_putstatic; 
    instrucoes_exec[newarray] = exec_newarray;
    instrucoes_exec[bipush] = exec_bipush;  
    
    // I/O
    instrucoes_exec[ldc] = exec_ldc;
    instrucoes_exec[getstatic] = exec_getstatic;
    instrucoes_exec[invokevirtual] = exec_invokevirtual;
    instrucoes_exec[invokespecial] = exec_invokespecial; 
    instrucoes_exec[invokestatic] = exec_invokestatic; 
    
    // Placeholders para evitar crash imediato em constantes simples
    instrucoes_exec[iconst_1] = exec_iconst_0; 
    instrucoes_exec[iconst_2] = exec_iconst_0; 
    instrucoes_exec[iconst_3] = exec_iconst_0; 
}

// ---------------------- Loop de execução ----------------------

void executar(Frame *frame) {
    while (frame->pc < frame->code_length) {
        byte1 opcode = frame->code[frame->pc];
        InstrucaoFunc func = instrucoes_exec[opcode];
        
        if (func) {
            frame->pc++; // Avança PC (1 byte do opcode)
            func(frame);
        } else {
            printf("Instrução %02x não implementada (PC: %d)\n", opcode, frame->pc);
            break;
        }
    }
}