





#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "executorInstrucoes.h"
#include "leitor.h" 
#include "catalogoCodigosInstrucoes.h" 


static cp_info *GLOBAL_CP = NULL;
static byte2 GLOBAL_CP_COUNT = 0;


#define HEAP_STORE_CAP 512
typedef struct { int obj_ref; unsigned short field_index; int value; } HeapEntry;
static HeapEntry HEAP_STORE[HEAP_STORE_CAP];
static int HEAP_COUNT = 0;

static int NEXT_OBJ_REF = 3; 

int create_object_ref(void) {
    if (HEAP_COUNT >= HEAP_STORE_CAP) {
        fprintf(stderr, "Erro: heap simulado cheio, não é possível criar novo objeto.\n");
        exit(EXIT_FAILURE);
    }
    int ref = NEXT_OBJ_REF++;
    
    HEAP_STORE[HEAP_COUNT].obj_ref = ref;
    HEAP_STORE[HEAP_COUNT].field_index = 0;
    HEAP_STORE[HEAP_COUNT].value = 0;
    HEAP_COUNT++;
    return ref;
}


InstrucaoFunc instrucoes_exec[256];



int pop_stack(Frame *frame) {
    if (frame->sp < 0) {
        fprintf(stderr, "Erro de Stack Underflow (PC: %d).\n", frame->pc);
        exit(EXIT_FAILURE);
    }
    return frame->operand_stack[frame->sp--];
}

void push_stack(Frame *frame, int value) {
    
    
    
    if (frame->sp + 1 >= frame->max_stack) {
        fprintf(stderr, "Erro: Stack Overflow. Max stack: %d (PC: %d)\n", frame->max_stack, frame->pc);
        exit(EXIT_FAILURE);
    }
    frame->operand_stack[++frame->sp] = value;
}


static void push_long(Frame *frame, int64_t value) {
    int high = (int)(value >> 32);
    int low = (int)(value & 0xFFFFFFFF);
    
    push_stack(frame, high);
    push_stack(frame, low);
}

static int64_t pop_long(Frame *frame) {
    int low = pop_stack(frame);
    int high = pop_stack(frame);
    uint32_t lowu = (uint32_t)low;
    return (((int64_t)high) << 32) | lowu;
}


static void push_double(Frame *frame, double d) {
    int64_t bits;
    memcpy(&bits, &d, sizeof(bits));
    push_long(frame, bits);
}

static double pop_double(Frame *frame) {
    int64_t bits = pop_long(frame);
    double d;
    memcpy(&d, &bits, sizeof(d));
    return d;
}



void exec_iconst_0(Frame *frame) {
    push_stack(frame, 0);
}

void exec_iconst_m1(Frame *frame) { push_stack(frame, -1); }
void exec_iconst_1(Frame *frame) { push_stack(frame, 1); }
void exec_iconst_2(Frame *frame) { push_stack(frame, 2); }
void exec_iconst_3(Frame *frame) { push_stack(frame, 3); }
void exec_iconst_4(Frame *frame) { push_stack(frame, 4); }
void exec_iconst_5(Frame *frame) { push_stack(frame, 5); }

void exec_iadd(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 + v2);
}

void exec_return(Frame *frame) {
    frame->pc = frame->code_length; 
    printf("--- RETORNO do Método (RETURN) ---\n");
}



void exec_isub(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 - v2);
}

void exec_imul(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 * v2);
}

void exec_idiv(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    if (v2 == 0) {
        fprintf(stderr, "Erro: Divisão por zero \n");
        exit(EXIT_FAILURE);
        }
    push_stack(frame, v1 / v2);
}

void exec_irem(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    if (v2 == 0) {
        fprintf(stderr, "Erro: Divisão por zero \n");
        exit(EXIT_FAILURE);
        }
    push_stack(frame, v1 % v2);
}

void exec_ineg(Frame *frame) {
    int v = pop_stack(frame);
    push_stack(frame, -v);
}


void exec_ladd(Frame *frame) {
    int64_t v2 = pop_long(frame);
    int64_t v1 = pop_long(frame);
    push_long(frame, v1 + v2);
}

void exec_lsub(Frame *frame) {
    int64_t v2 = pop_long(frame);
    int64_t v1 = pop_long(frame);
    push_long(frame, v1 - v2);
}

void exec_lmul(Frame *frame) {
    int64_t v2 = pop_long(frame);
    int64_t v1 = pop_long(frame);
    push_long(frame, v1 * v2);
}

void exec_ldiv(Frame *frame) {
    int64_t v2 = pop_long(frame);
    int64_t v1 = pop_long(frame);
    if (v2 == 0LL) {
        fprintf(stderr, "Erro: Divisão por zero (ldiv)\n");
        exit(EXIT_FAILURE);
    }
    push_long(frame, v1 / v2);
}

void exec_lrem(Frame *frame) {
    int64_t v2 = pop_long(frame);
    int64_t v1 = pop_long(frame);
    if (v2 == 0LL) {
        fprintf(stderr, "Erro: Divisão por zero (lrem)\n");
        exit(EXIT_FAILURE);
    }
    push_long(frame, v1 % v2);
}

void exec_lneg(Frame *frame) {
    int64_t v = pop_long(frame);
    push_long(frame, -v);
}


static int float_to_intbits(float f) {
    int i;
    memcpy(&i, &f, sizeof(int));
    return i;
}

static float intbits_to_float(int i) {
    float f;
    memcpy(&f, &i, sizeof(float));
    return f;
}

void exec_fadd(Frame *frame) {
    int iv2 = pop_stack(frame);
    int iv1 = pop_stack(frame);
    float v2 = intbits_to_float(iv2);
    float v1 = intbits_to_float(iv1);
    float r = v1 + v2;
    push_stack(frame, float_to_intbits(r));
}

void exec_fsub(Frame *frame) {
    int iv2 = pop_stack(frame);
    int iv1 = pop_stack(frame);
    float v2 = intbits_to_float(iv2);
    float v1 = intbits_to_float(iv1);
    float r = v1 - v2;
    push_stack(frame, float_to_intbits(r));
}

void exec_fmul(Frame *frame) {
    int iv2 = pop_stack(frame);
    int iv1 = pop_stack(frame);
    float v2 = intbits_to_float(iv2);
    float v1 = intbits_to_float(iv1);
    float r = v1 * v2;
    push_stack(frame, float_to_intbits(r));
}

void exec_fdiv(Frame *frame) {
    int iv2 = pop_stack(frame);
    int iv1 = pop_stack(frame);
    float v2 = intbits_to_float(iv2);
    float v1 = intbits_to_float(iv1);
    if (v2 == 0.0f) {
        fprintf(stderr, "Erro: Divisão por zero (fdiv)\n");
        exit(EXIT_FAILURE);
    }
    float r = v1 / v2;
    push_stack(frame, float_to_intbits(r));
}

void exec_frem(Frame *frame) {
    int iv2 = pop_stack(frame);
    int iv1 = pop_stack(frame);
    float v2 = intbits_to_float(iv2);
    float v1 = intbits_to_float(iv1);
    if (v2 == 0.0f) {
        fprintf(stderr, "Erro: Divisão por zero (frem)\n");
        exit(EXIT_FAILURE);
    }
    float r = fmodf(v1, v2);
    push_stack(frame, float_to_intbits(r));
}

void exec_fneg(Frame *frame) {
    int iv = pop_stack(frame);
    float v = intbits_to_float(iv);
    v = -v;
    push_stack(frame, float_to_intbits(v));
}


void exec_ldc2_w(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    cp_info *cp_entry = GLOBAL_CP + index - 1;
    if (cp_entry->tag == CONSTANT_Long) {
        uint64_t v = decodeLongInfo(cp_entry);
        push_long(frame, (int64_t)v);
    } else if (cp_entry->tag == CONSTANT_Double) {
        double d = decodeDoubleInfo(cp_entry);
        int64_t bits;
        memcpy(&bits, &d, sizeof(bits));
        push_long(frame, bits);
    } else {
        fprintf(stderr, "Erro: LDC2_W para tag %d não suportada.\n", cp_entry->tag);
        exit(EXIT_FAILURE);
    }
}


void exec_ldc_w(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    cp_info *cp_entry = GLOBAL_CP + index - 1;
    
    if (cp_entry->tag == CONSTANT_Integer) {
        push_stack(frame, decodeIntegerInfo(cp_entry));
    } else if (cp_entry->tag == CONSTANT_String) {
        push_stack(frame, cp_entry->UnionCP.CONSTANT_String.string_index);
    } else {
        fprintf(stderr, "Erro: LDC_W para tag %d não suportada.\n", cp_entry->tag);
        exit(EXIT_FAILURE);
    }
}


void exec_print_int(Frame *frame) {
    int v = pop_stack(frame);
    printf("%d\n", v);
}


void exec_dadd(Frame *frame) {
    double v2 = pop_double(frame);
    double v1 = pop_double(frame);
    push_double(frame, v1 + v2);
}

void exec_dsub(Frame *frame) {
    double v2 = pop_double(frame);
    double v1 = pop_double(frame);
    push_double(frame, v1 - v2);
}

void exec_dmul(Frame *frame) {
    double v2 = pop_double(frame);
    double v1 = pop_double(frame);
    push_double(frame, v1 * v2);
}

void exec_ddiv(Frame *frame) {
    double v2 = pop_double(frame);
    double v1 = pop_double(frame);
    
    
    
    push_double(frame, v1 / v2);
}

void exec_drem(Frame *frame) {
    double v2 = pop_double(frame);
    double v1 = pop_double(frame);
    
    
    double r = fmod(v1, v2);
    push_double(frame, r);
}

void exec_dneg(Frame *frame) {
    double v = pop_double(frame);
    push_double(frame, -v);
}

void exec_dconst_0(Frame *frame) { push_double(frame, 0.0); }
void exec_dconst_1(Frame *frame) { push_double(frame, 1.0); }

void exec_dload(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    int high = frame->local_variables[index];
    int low = frame->local_variables[index + 1];
    uint32_t lowu = (uint32_t)low;
    int64_t bits = (((int64_t)high) << 32) | lowu;
    push_long(frame, bits);
}
void exec_dload_0(Frame *frame) { int high = frame->local_variables[0]; int low = frame->local_variables[1]; uint32_t lowu=(uint32_t)low; int64_t bits=(((int64_t)high)<<32)|lowu; push_long(frame,bits); }
void exec_dload_1(Frame *frame) { int high = frame->local_variables[1]; int low = frame->local_variables[2]; uint32_t lowu=(uint32_t)low; int64_t bits=(((int64_t)high)<<32)|lowu; push_long(frame,bits); }
void exec_dload_2(Frame *frame) { int high = frame->local_variables[2]; int low = frame->local_variables[3]; uint32_t lowu=(uint32_t)low; int64_t bits=(((int64_t)high)<<32)|lowu; push_long(frame,bits); }
void exec_dload_3(Frame *frame) { int high = frame->local_variables[3]; int low = frame->local_variables[4]; uint32_t lowu=(uint32_t)low; int64_t bits=(((int64_t)high)<<32)|lowu; push_long(frame,bits); }

void exec_dstore(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    int64_t bits = pop_long(frame);
    frame->local_variables[index] = (int)(bits >> 32);
    frame->local_variables[index + 1] = (int)(bits & 0xFFFFFFFF);
}
void exec_dstore_0(Frame *frame) { int64_t bits = pop_long(frame); frame->local_variables[0]=(int)(bits>>32); frame->local_variables[1]=(int)(bits&0xFFFFFFFF); }
void exec_dstore_1(Frame *frame) { int64_t bits = pop_long(frame); frame->local_variables[1]=(int)(bits>>32); frame->local_variables[2]=(int)(bits&0xFFFFFFFF); }
void exec_dstore_2(Frame *frame) { int64_t bits = pop_long(frame); frame->local_variables[2]=(int)(bits>>32); frame->local_variables[3]=(int)(bits&0xFFFFFFFF); }
void exec_dstore_3(Frame *frame) { int64_t bits = pop_long(frame); frame->local_variables[3]=(int)(bits>>32); frame->local_variables[4]=(int)(bits&0xFFFFFFFF); }


void exec_fconst_0(Frame *frame) { push_stack(frame, float_to_intbits(0.0f)); }
void exec_fconst_1(Frame *frame) { push_stack(frame, float_to_intbits(1.0f)); }
void exec_fconst_2(Frame *frame) { push_stack(frame, float_to_intbits(2.0f)); }

void exec_fstore(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    int val = pop_stack(frame);
    if (index < frame->max_locals) {
        frame->local_variables[index] = val;
    } else {
        fprintf(stderr, "Erro: fstore com índice fora de bounds (%d)\n", index);
        exit(EXIT_FAILURE);
    }
}
void exec_fstore_0(Frame *frame) { int val = pop_stack(frame); frame->local_variables[0] = val; }
void exec_fstore_1(Frame *frame) { int val = pop_stack(frame); frame->local_variables[1] = val; }
void exec_fstore_2(Frame *frame) { int val = pop_stack(frame); frame->local_variables[2] = val; }
void exec_fstore_3(Frame *frame) { int val = pop_stack(frame); frame->local_variables[3] = val; }


void exec_lcmp(Frame *frame) {
    int64_t v2 = pop_long(frame);
    int64_t v1 = pop_long(frame);
    if (v1 > v2) push_stack(frame, 1);
    else if (v1 == v2) push_stack(frame, 0);
    else push_stack(frame, -1);
}

void exec_fcmpl(Frame *frame) {
    int iv2 = pop_stack(frame);
    int iv1 = pop_stack(frame);
    float v2 = intbits_to_float(iv2);
    float v1 = intbits_to_float(iv1);
    if (isnan(v1) || isnan(v2)) push_stack(frame, -1);
    else if (v1 > v2) push_stack(frame, 1);
    else if (v1 == v2) push_stack(frame, 0);
    else push_stack(frame, -1);
}

void exec_fcmpg(Frame *frame) {
    int iv2 = pop_stack(frame);
    int iv1 = pop_stack(frame);
    float v2 = intbits_to_float(iv2);
    float v1 = intbits_to_float(iv1);
    if (isnan(v1) || isnan(v2)) push_stack(frame, 1);
    else if (v1 > v2) push_stack(frame, 1);
    else if (v1 == v2) push_stack(frame, 0);
    else push_stack(frame, -1);
}

void exec_dcmpl(Frame *frame) {
    int64_t iv2 = pop_long(frame);
    int64_t iv1 = pop_long(frame);
    double v2 = *(double*)&iv2;
    double v1 = *(double*)&iv1;
    if (isnan(v1) || isnan(v2)) push_stack(frame, -1);
    else if (v1 > v2) push_stack(frame, 1);
    else if (v1 == v2) push_stack(frame, 0);
    else push_stack(frame, -1);
}

void exec_dcmpg(Frame *frame) {
    int64_t iv2 = pop_long(frame);
    int64_t iv1 = pop_long(frame);
    double v2 = *(double*)&iv2;
    double v1 = *(double*)&iv1;
    if (isnan(v1) || isnan(v2)) push_stack(frame, 1);
    else if (v1 > v2) push_stack(frame, 1);
    else if (v1 == v2) push_stack(frame, 0);
    else push_stack(frame, -1);
}


void exec_pop(Frame *frame) {
    pop_stack(frame);
}

void exec_pop2(Frame *frame) {
    pop_stack(frame);
    pop_stack(frame);
}


void exec_aconst_null(Frame *frame) {
    push_stack(frame, 0); 
}


void exec_arraylength(Frame *frame) {
    int arr_ref = pop_stack(frame);
    printf("Aviso: ARRAYLENGTH de array Ref %d (simulação, push 10)\n", arr_ref);
    push_stack(frame, 10);
}

void exec_aaload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: AALOAD array Ref %d[%d] (simulação, push %d)\n", arr_ref, index, arr_ref);
    push_stack(frame, arr_ref);
}

void exec_aastore(Frame *frame) {
    int value = pop_stack(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: AASTORE array Ref %d[%d] = %d (simulação)\n", arr_ref, index, value);
}

void exec_iaload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: IALOAD array Ref %d[%d] (simulação, push 0)\n", arr_ref, index);
    push_stack(frame, 0);
}

void exec_iastore(Frame *frame) {
    int value = pop_stack(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: IASTORE array Ref %d[%d] = %d (simulação)\n", arr_ref, index, value);
}

void exec_baload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: BALOAD array Ref %d[%d] (simulação, push 0)\n", arr_ref, index);
    push_stack(frame, 0);
}

void exec_bastore(Frame *frame) {
    int value = pop_stack(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: BASTORE array Ref %d[%d] = %d (simulação)\n", arr_ref, index, value);
}

void exec_caload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: CALOAD array Ref %d[%d] (simulação, push 0)\n", arr_ref, index);
    push_stack(frame, 0);
}

void exec_castore(Frame *frame) {
    int value = pop_stack(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: CASTORE array Ref %d[%d] = %d (simulação)\n", arr_ref, index, value);
}

void exec_saload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: SALOAD array Ref %d[%d] (simulação, push 0)\n", arr_ref, index);
    push_stack(frame, 0);
}

void exec_sastore(Frame *frame) {
    int value = pop_stack(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: SASTORE array Ref %d[%d] = %d (simulação)\n", arr_ref, index, value);
}

void exec_laload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: LALOAD array Ref %d[%d] (simulação, push 0)\n", arr_ref, index);
    push_long(frame, 0LL);
}

void exec_lastore(Frame *frame) {
    int64_t value = pop_long(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    (void)value;
    printf("Aviso: LASTORE array Ref %d[%d] (simulação)\n", arr_ref, index);
}

void exec_faload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: FALOAD array Ref %d[%d] (simulação, push 0.0)\n", arr_ref, index);
    push_stack(frame, float_to_intbits(0.0f));
}

void exec_fastore(Frame *frame) {
    int value = pop_stack(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    (void)value;
    printf("Aviso: FASTORE array Ref %d[%d] (simulação)\n", arr_ref, index);
}

void exec_daload(Frame *frame) {
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    printf("Aviso: DALOAD array Ref %d[%d] (simulação, push 0.0)\n", arr_ref, index);
    push_long(frame, 0LL);
}

void exec_dastore(Frame *frame) {
    int64_t value = pop_long(frame);
    int index = pop_stack(frame);
    int arr_ref = pop_stack(frame);
    (void)value;
    printf("Aviso: DASTORE array Ref %d[%d] (simulação)\n", arr_ref, index);
}


void exec_multianewarray(Frame *frame) {
    
    int cp_idx = ((frame->code[frame->pc] << 8) | frame->code[frame->pc+1]);
    frame->pc += 2;
    
    int dimensions = frame->code[frame->pc];
    frame->pc++;
    
    for (int i = 0; i < dimensions && frame->sp > 0; i++) {
        pop_stack(frame);
    }
    
    push_stack(frame, 1);
    (void)cp_idx;
    printf("Aviso: MULTIANEWARRAY criado com %d dimensões (simulação, push 1)\n", dimensions);
}


void exec_i2l(Frame *frame) {
    int v = pop_stack(frame);
    push_long(frame, (int64_t)v);
}

void exec_i2f(Frame *frame) {
    int v = pop_stack(frame);
    push_stack(frame, float_to_intbits((float)v));
}

void exec_i2d(Frame *frame) {
    int v = pop_stack(frame);
    push_double(frame, (double)v);
}

void exec_l2i(Frame *frame) {
    int64_t v = pop_long(frame);
    push_stack(frame, (int)v);
}

void exec_l2f(Frame *frame) {
    int64_t v = pop_long(frame);
    push_stack(frame, float_to_intbits((float)v));
}

void exec_l2d(Frame *frame) {
    int64_t v = pop_long(frame);
    push_double(frame, (double)v);
}

void exec_f2i(Frame *frame) {
    int iv = pop_stack(frame);
    float v = intbits_to_float(iv);
    push_stack(frame, (int)v);
}

void exec_f2l(Frame *frame) {
    int iv = pop_stack(frame);
    float v = intbits_to_float(iv);
    push_long(frame, (int64_t)v);
}

void exec_f2d(Frame *frame) {
    int iv = pop_stack(frame);
    float v = intbits_to_float(iv);
    push_double(frame, (double)v);
}

void exec_d2i(Frame *frame) {
    double v = pop_double(frame);
    push_stack(frame, (int)v);
}

void exec_d2l(Frame *frame) {
    double v = pop_double(frame);
    push_long(frame, (int64_t)v);
}

void exec_d2f(Frame *frame) {
    double v = pop_double(frame);
    push_stack(frame, float_to_intbits((float)v));
}

void exec_i2c(Frame *frame) {
    int v = pop_stack(frame);
    push_stack(frame, (int)(unsigned short)v);
}


void exec_invokeinterface(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;
    byte1 count = frame->code[frame->pc++];
    byte1 zero = frame->code[frame->pc++];
    (void)index; (void)count; (void)zero;
    printf("Aviso: INVOKEINTERFACE (simulação)\n");
}



void exec_ishl(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 << (v2 & 0x1F)); 
}

void exec_ishr(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 >> (v2 & 0x1F)); 
}

void exec_iand(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 & v2);
}

void exec_ior(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 | v2);
}

void exec_ixor(Frame *frame) {
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    push_stack(frame, v1 ^ v2);
}

void exec_iinc(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    byte1 const_val = frame->code[frame->pc++];
    int increment = (signed char)const_val;
    frame->local_variables[index] += increment; 

}




static int16_t read_branch_offset(Frame *frame) {
    unsigned char b1 = frame->code[frame->pc++];
    unsigned char b2 = frame->code[frame->pc++];
    int16_t off = (int16_t)((b1 << 8) | b2);
    return off;
}

void exec_if_icmpeq(Frame *frame) {
    int16_t off = read_branch_offset(frame);
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    int opcode_pos = frame->pc - 3; 
    if (v1 == v2) frame->pc = opcode_pos + off;
}

void exec_if_icmpne(Frame *frame) {
    int16_t off = read_branch_offset(frame);
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    int opcode_pos = frame->pc - 3;
    if (v1 != v2) frame->pc = opcode_pos + off;
}

void exec_if_icmplt(Frame *frame) {
    int16_t off = read_branch_offset(frame);
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    int opcode_pos = frame->pc - 3;
    if (v1 < v2) frame->pc = opcode_pos + off;
}

void exec_if_icmpge(Frame *frame) {
    int16_t off = read_branch_offset(frame);
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    int opcode_pos = frame->pc - 3;
    if (v1 >= v2) frame->pc = opcode_pos + off;
}

void exec_if_icmpgt(Frame *frame) {
    int16_t off = read_branch_offset(frame);
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    int opcode_pos = frame->pc - 3;
    if (v1 > v2) frame->pc = opcode_pos + off;
}

void exec_if_icmple(Frame *frame) {
    int16_t off = read_branch_offset(frame);
    int v2 = pop_stack(frame);
    int v1 = pop_stack(frame);
    int opcode_pos = frame->pc - 3;
    if (v1 <= v2) frame->pc = opcode_pos + off;
}


void exec_goto(Frame *frame) {
    
    frame->pc += 2;
}


void exec_ifeq(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}

void exec_ifne(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}

void exec_iflt(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}

void exec_ifge(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}

void exec_ifgt(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}

void exec_ifle(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}

void exec_ifnull(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}

void exec_ifnonnull(Frame *frame) {
    frame->pc += 2;
    pop_stack(frame);
}


void exec_goto_w(Frame *frame) {
    frame->pc += 4;
}


void exec_tableswitch(Frame *frame) {
    
    
    int opcode_pos = frame->pc - 1;

    
    int pad = (4 - (opcode_pos % 4)) % 4;
    frame->pc += pad;

    
    uint32_t udef = ((uint32_t)frame->code[frame->pc] << 24) |
                    ((uint32_t)frame->code[frame->pc+1] << 16) |
                    ((uint32_t)frame->code[frame->pc+2] << 8) |
                    ((uint32_t)frame->code[frame->pc+3]);
    int32_t default_offset = (int32_t)udef;
    frame->pc += 4;

    uint32_t ulow = ((uint32_t)frame->code[frame->pc] << 24) |
                    ((uint32_t)frame->code[frame->pc+1] << 16) |
                    ((uint32_t)frame->code[frame->pc+2] << 8) |
                    ((uint32_t)frame->code[frame->pc+3]);
    int32_t low = (int32_t)ulow;
    frame->pc += 4;

    uint32_t uhigh = ((uint32_t)frame->code[frame->pc] << 24) |
                    ((uint32_t)frame->code[frame->pc+1] << 16) |
                    ((uint32_t)frame->code[frame->pc+2] << 8) |
                    ((uint32_t)frame->code[frame->pc+3]);
    int32_t high = (int32_t)uhigh;
    frame->pc += 4;

    int32_t npairs = high - low + 1;
    if (npairs < 0) npairs = 0; 

    int index = pop_stack(frame);
    int32_t branch_offset = default_offset;

    if (index >= low && index <= high && npairs > 0) {
        int idx = index - low;
        
        uint32_t uoff = ((uint32_t)frame->code[frame->pc + idx*4] << 24) |
                        ((uint32_t)frame->code[frame->pc + idx*4 + 1] << 16) |
                        ((uint32_t)frame->code[frame->pc + idx*4 + 2] << 8) |
                        ((uint32_t)frame->code[frame->pc + idx*4 + 3]);
        branch_offset = (int32_t)uoff;
    }

    
    frame->pc = opcode_pos + branch_offset;
}


void exec_wide(Frame *frame) {
    
    
    frame->pc++;
    byte1 op = frame->code[frame->pc];
    frame->pc++;
    
    if (op == iload || op == fload || op == aload || op == istore || 
        op == fstore || op == astore || op == lload || op == dload || 
        op == lstore || op == dstore || op == ret) {
        frame->pc += 2;  
    } else if (op == iinc) {
        frame->pc += 4;  
    }
}






void exec_aload_0(Frame *frame) {
    
    push_stack(frame, frame->local_variables[0]);
}


void exec_iload_1(Frame *frame) {
    push_stack(frame, frame->local_variables[1]);
}


void exec_aload_2(Frame *frame) {
    push_stack(frame, frame->local_variables[2]);
}


void exec_iload_3(Frame *frame) {
    push_stack(frame, frame->local_variables[3]);
}


void exec_iload(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    if (index < frame->max_locals) {
        push_stack(frame, frame->local_variables[index]);
    } else {
        fprintf(stderr, "Erro: iload com índice fora de bounds (%d)\n", index);
        exit(EXIT_FAILURE);
    }
}

void exec_istore_2(Frame *frame) {
    int value = pop_stack(frame);
    frame->local_variables[2] = value;
}


void exec_istore(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    int value = pop_stack(frame);
    if (index < frame->max_locals) {
        frame->local_variables[index] = value;
    } else {
        fprintf(stderr, "Erro: istore com índice fora de bounds (%d)\n", index);
        exit(EXIT_FAILURE);
    }
}

void exec_istore_0(Frame *frame) {
    int value = pop_stack(frame);
    frame->local_variables[0] = value;
}

void exec_istore_1(Frame *frame) {
    int value = pop_stack(frame);
    frame->local_variables[1] = value;
}

void exec_istore_3(Frame *frame) {
    int value = pop_stack(frame);
    frame->local_variables[3] = value;
}


void exec_aload_3(Frame *frame) {
    push_stack(frame, frame->local_variables[3]);
}


void exec_aload(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    if (index < frame->max_locals) {
        push_stack(frame, frame->local_variables[index]);
    } else {
        fprintf(stderr, "Erro: aload com índice fora de bounds (%d)\n", index);
        exit(EXIT_FAILURE);
    }
}



void exec_sipush(Frame *frame) {
    byte1 byte1 = frame->code[frame->pc++];
    byte2 byte2 = frame->code[frame->pc++];
    
    short value = (short)((byte1 << 8) | byte2);
    push_stack(frame, value);
}




void exec_i2b(Frame *frame) {
    int v = pop_stack(frame);
    push_stack(frame, (signed char)v);

}



void exec_i2s(Frame *frame) {
    int v = pop_stack(frame);
    push_stack(frame, (short)v);
}




void exec_putfield(Frame *frame) {
    
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2; 

    int value = pop_stack(frame); 
    int obj_ref = pop_stack(frame); 

    char *field_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);

    
    int found = 0;
    for (int i = 0; i < HEAP_COUNT; i++) {
        if (HEAP_STORE[i].obj_ref == obj_ref && HEAP_STORE[i].field_index == index) {
            HEAP_STORE[i].value = value;
            found = 1; break;
        }
    }
    if (!found && HEAP_COUNT < HEAP_STORE_CAP) {
        HEAP_STORE[HEAP_COUNT].obj_ref = obj_ref;
        HEAP_STORE[HEAP_COUNT].field_index = index;
        HEAP_STORE[HEAP_COUNT].value = value;
        HEAP_COUNT++;
    }

    printf("--- [PUTFIELD] Gravando valor %d no campo %s do Objeto Ref: %d ---\n", value, field_ref, obj_ref);

    free(field_ref);
}

void exec_getfield(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;
    int obj_ref = pop_stack(frame);
    char *field_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);

    int found = 0; int val = 0;
    for (int i = 0; i < HEAP_COUNT; i++) {
        if (HEAP_STORE[i].obj_ref == obj_ref && HEAP_STORE[i].field_index == index) {
            val = HEAP_STORE[i].value; found = 1; break;
        }
    }
    if (!found) {
        
        val = 0;
    }
    printf("Aviso: GETFIELD %s de Objeto Ref %d (simulação, push %d)\n", field_ref, obj_ref, val);
    push_stack(frame, val);
    free(field_ref);
}

void exec_newarray(Frame *frame) {
    int count = pop_stack(frame); 
    byte1 atype = frame->code[frame->pc++]; 
    printf("--- [NEWARRAY] Criando array de tipo %d com tamanho %d ---\n", atype, count);
    
    push_stack(frame, 99); 
}

void exec_putstatic(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;
    char *field_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    printf("Aviso: PUTSTATIC %s ignorado (simulação).\n", field_ref);
    free(field_ref);
}


void exec_invokespecial(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;

    char *method_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    
    
    if (strstr(method_ref, "<init>")) {
        
        int object_ref = pop_stack(frame);
        printf("--- [INVOKESPECIAL] Chamando construtor %s para Objeto Ref: %d ---\n", method_ref, object_ref);
    } else {
        fprintf(stderr, "Erro: INVOKESPECIAL para método %s não suportado. (PC: %d)\n", method_ref, frame->pc - 3);
        free(method_ref);
        exit(EXIT_FAILURE);
    }
    free(method_ref);
}


void exec_new(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc+1];
    frame->pc += 2;
    (void)index; 
    
    int ref = create_object_ref();
    push_stack(frame, ref);
    printf("--- [NEW] Criando objeto simulado Ref: %d ---\n", ref);
}


void exec_lconst_0(Frame *frame) {
    push_long(frame, 0LL);
}

void exec_lconst_1(Frame *frame) {
    push_long(frame, 1LL);
}

void exec_lload(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    int high = frame->local_variables[index];
    int low = frame->local_variables[index + 1];
    uint32_t lowu = (uint32_t)low;
    int64_t val = (((int64_t)high) << 32) | lowu;
    push_long(frame, val);
}

void exec_lload_0(Frame *frame) { 
    int high = frame->local_variables[0];
    int low = frame->local_variables[1];
    uint32_t lowu = (uint32_t)low;
    int64_t val = (((int64_t)high) << 32) | lowu;
    push_long(frame, val);
}
void exec_lload_1(Frame *frame) { 
    int high = frame->local_variables[1];
    int low = frame->local_variables[2];
    uint32_t lowu = (uint32_t)low;
    int64_t val = (((int64_t)high) << 32) | lowu;
    push_long(frame, val);
}
void exec_lload_2(Frame *frame) { 
    int high = frame->local_variables[2];
    int low = frame->local_variables[3];
    uint32_t lowu = (uint32_t)low;
    int64_t val = (((int64_t)high) << 32) | lowu;
    push_long(frame, val);
}
void exec_lload_3(Frame *frame) { 
    int high = frame->local_variables[3];
    int low = frame->local_variables[4];
    uint32_t lowu = (uint32_t)low;
    int64_t val = (((int64_t)high) << 32) | lowu;
    push_long(frame, val);
}

void exec_lstore(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    int64_t val = pop_long(frame);
    frame->local_variables[index] = (int)(val >> 32);
    frame->local_variables[index + 1] = (int)(val & 0xFFFFFFFF);
}

void exec_lstore_0(Frame *frame) {
    int64_t val = pop_long(frame);
    frame->local_variables[0] = (int)(val >> 32);
    frame->local_variables[1] = (int)(val & 0xFFFFFFFF);
}
void exec_lstore_1(Frame *frame) {
    int64_t val = pop_long(frame);
    frame->local_variables[1] = (int)(val >> 32);
    frame->local_variables[2] = (int)(val & 0xFFFFFFFF);
}
void exec_lstore_2(Frame *frame) {
    int64_t val = pop_long(frame);
    frame->local_variables[2] = (int)(val >> 32);
    frame->local_variables[3] = (int)(val & 0xFFFFFFFF);
}
void exec_lstore_3(Frame *frame) {
    int64_t val = pop_long(frame);
    frame->local_variables[3] = (int)(val >> 32);
    frame->local_variables[4] = (int)(val & 0xFFFFFFFF);
}



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
        
        printf("Aviso: GETSTATIC %s simulado (push 0).\n", ref_string);
        push_stack(frame, 0);
    }

    free(ref_string);
}

void exec_astore_0(Frame *frame) {
    int ref = pop_stack(frame);
    frame->local_variables[0] = ref;
}

void exec_astore_2(Frame *frame) {
    int ref = pop_stack(frame);
    frame->local_variables[2] = ref;
}

void exec_astore_3(Frame *frame) {
    int ref = pop_stack(frame);
    frame->local_variables[3] = ref;
}


void exec_astore(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    int ref = pop_stack(frame);
    if (index < frame->max_locals) {
        frame->local_variables[index] = ref;
    } else {
        fprintf(stderr, "Erro: astore com índice fora de bounds (%d)\n", index);
        exit(EXIT_FAILURE);
    }
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

void exec_fload_0(Frame *frame) { push_stack(frame, frame->local_variables[0]); }
void exec_fload_1(Frame *frame) { push_stack(frame, frame->local_variables[1]); }
void exec_fload_2(Frame *frame) { push_stack(frame, frame->local_variables[2]); }
void exec_fload_3(Frame *frame) { push_stack(frame, frame->local_variables[3]); }

void exec_fload(Frame *frame) {
    byte1 index = frame->code[frame->pc++];
    if (index < frame->max_locals) {
        push_stack(frame, frame->local_variables[index]);
    } else {
        fprintf(stderr, "Erro: fload com índice fora de bounds (%d)\n", index);
        exit(EXIT_FAILURE);
    }
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
        int ref = pop_stack(frame); 
        
        if (ref == SYSTEM_OUT_REF) {
             printf("[OUTPUT]: %d (ou String Ref)\n", arg);
        }
    } else {
        printf("Aviso: INVOKEVIRTUAL %s ignorado (pop args).\n", method_ref);
        pop_stack(frame); 
    }
    
    free(method_ref);
}

void exec_invokestatic(Frame *frame) {
    byte2 index = (frame->code[frame->pc] << 8) | frame->code[frame->pc + 1];
    frame->pc += 2; 
    char *method_ref = decodeInstructionOp(GLOBAL_CP, index, GLOBAL_CP_COUNT);
    printf("--- [INVOKESTATIC] Chamando método estático: %s ---\n", method_ref);
    free(method_ref);
    
}



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



void inicializarInstrucoes(void) {
    for (int i = 0; i < 256; i++) {
        instrucoes_exec[i] = NULL;
    }

    
    instrucoes_exec[nop] = NULL; 
    instrucoes_exec[iconst_0] = exec_iconst_0;
    instrucoes_exec[iadd] = exec_iadd;
    instrucoes_exec[inst_return] = exec_return;     

    
    instrucoes_exec[isub] = exec_isub;
    instrucoes_exec[imul] = exec_imul;
    instrucoes_exec[idiv] = exec_idiv;
    instrucoes_exec[irem] = exec_irem;
    instrucoes_exec[ineg] = exec_ineg;
    
    instrucoes_exec[ladd] = exec_ladd;
    instrucoes_exec[lsub] = exec_lsub;
    instrucoes_exec[lmul] = exec_lmul;
    instrucoes_exec[inst_ldiv] = exec_ldiv;
    instrucoes_exec[lrem] = exec_lrem;
    instrucoes_exec[lneg] = exec_lneg;
    
    instrucoes_exec[fadd] = exec_fadd;
    instrucoes_exec[fsub] = exec_fsub;
    instrucoes_exec[fmul] = exec_fmul;
    instrucoes_exec[fdiv] = exec_fdiv;
    instrucoes_exec[frem] = exec_frem;
    instrucoes_exec[fneg] = exec_fneg;

    

    instrucoes_exec[ishl] = exec_ishl;
    instrucoes_exec[ishr] = exec_ishr;
    instrucoes_exec[iand] = exec_iand;
    instrucoes_exec[ior] = exec_ior;
    instrucoes_exec[ixor] = exec_ixor;
    instrucoes_exec[iinc] = exec_iinc;

    

    instrucoes_exec[i2b] = exec_i2b;
    instrucoes_exec[i2s] = exec_i2s;

    
    instrucoes_exec[sipush] = exec_sipush;



    
    instrucoes_exec[aload_0] = exec_aload_0; 
    instrucoes_exec[aload_1] = exec_aload_1; 
    instrucoes_exec[aload_2] = exec_aload_2; 
    instrucoes_exec[aload_3] = exec_aload_3;
    instrucoes_exec[aload] = exec_aload;

    
    instrucoes_exec[lconst_0] = exec_lconst_0;
    instrucoes_exec[lconst_1] = exec_lconst_1;
    instrucoes_exec[lload] = exec_lload;
    instrucoes_exec[lload_0] = exec_lload_0;
    instrucoes_exec[lload_1] = exec_lload_1;
    instrucoes_exec[lload_2] = exec_lload_2;
    instrucoes_exec[lload_3] = exec_lload_3;
    instrucoes_exec[lstore] = exec_lstore;
    instrucoes_exec[lstore_0] = exec_lstore_0;
    instrucoes_exec[lstore_1] = exec_lstore_1;
    instrucoes_exec[lstore_2] = exec_lstore_2;
    instrucoes_exec[lstore_3] = exec_lstore_3;

    
    instrucoes_exec[dadd] = exec_dadd;
    instrucoes_exec[dsub] = exec_dsub;
    instrucoes_exec[dmul] = exec_dmul;
    instrucoes_exec[ddiv] = exec_ddiv;
    instrucoes_exec[inst_drem] = exec_drem;
    instrucoes_exec[dneg] = exec_dneg;
    instrucoes_exec[dconst_0] = exec_dconst_0;
    instrucoes_exec[dconst_1] = exec_dconst_1;
    instrucoes_exec[dload] = exec_dload;
    instrucoes_exec[dload_0] = exec_dload_0;
    instrucoes_exec[dload_1] = exec_dload_1;
    instrucoes_exec[dload_2] = exec_dload_2;
    instrucoes_exec[dload_3] = exec_dload_3;
    instrucoes_exec[dstore] = exec_dstore;
    instrucoes_exec[dstore_0] = exec_dstore_0;
    instrucoes_exec[dstore_1] = exec_dstore_1;
    instrucoes_exec[dstore_2] = exec_dstore_2;
    instrucoes_exec[dstore_3] = exec_dstore_3;

    instrucoes_exec[iload_0] = exec_iload_0;
    instrucoes_exec[iload_1] = exec_iload_1; 
    instrucoes_exec[iload_2] = exec_iload_2;
    instrucoes_exec[iload_3] = exec_iload_3; 
    instrucoes_exec[iload] = exec_iload;

    
    instrucoes_exec[fload_0] = exec_fload_0;
    instrucoes_exec[fload_1] = exec_fload_1;
    instrucoes_exec[fload_2] = exec_fload_2;
    instrucoes_exec[fload_3] = exec_fload_3;
    instrucoes_exec[fload] = exec_fload;

    instrucoes_exec[astore_0] = exec_astore_0;
    instrucoes_exec[astore_1] = exec_astore_1;
    instrucoes_exec[astore_2] = exec_astore_2;
    instrucoes_exec[astore_3] = exec_astore_3;
    instrucoes_exec[astore] = exec_astore;
    instrucoes_exec[istore] = exec_istore;
    instrucoes_exec[istore_0] = exec_istore_0;
    instrucoes_exec[istore_1] = exec_istore_1;
    instrucoes_exec[istore_2] = exec_istore_2;
    instrucoes_exec[istore_3] = exec_istore_3;

    
    instrucoes_exec[fstore] = exec_fstore;
    instrucoes_exec[fstore_0] = exec_fstore_0;
    instrucoes_exec[fstore_1] = exec_fstore_1;
    instrucoes_exec[fstore_2] = exec_fstore_2;
    instrucoes_exec[fstore_3] = exec_fstore_3;

    
    instrucoes_exec[lcmp] = exec_lcmp;
    instrucoes_exec[fcmpl] = exec_fcmpl;
    instrucoes_exec[fcmpg] = exec_fcmpg;

    
    instrucoes_exec[pop] = exec_pop;
    instrucoes_exec[pop2] = exec_pop2;

    
    instrucoes_exec[aconst_null] = exec_aconst_null;
    instrucoes_exec[fconst_0] = exec_fconst_0;
    instrucoes_exec[fconst_1] = exec_fconst_1;
    instrucoes_exec[fconst_2] = exec_fconst_2;

    
    instrucoes_exec[iaload] = exec_iaload;
    instrucoes_exec[laload] = exec_laload;
    instrucoes_exec[faload] = exec_faload;
    instrucoes_exec[daload] = exec_daload;
    instrucoes_exec[aaload] = exec_aaload;
    instrucoes_exec[baload] = exec_baload;
    instrucoes_exec[caload] = exec_caload;
    instrucoes_exec[saload] = exec_saload;
    instrucoes_exec[iastore] = exec_iastore;
    instrucoes_exec[lastore] = exec_lastore;
    instrucoes_exec[fastore] = exec_fastore;
    instrucoes_exec[dastore] = exec_dastore;
    instrucoes_exec[aastore] = exec_aastore;
    instrucoes_exec[bastore] = exec_bastore;
    instrucoes_exec[castore] = exec_castore;
    instrucoes_exec[sastore] = exec_sastore;
    instrucoes_exec[arraylength] = exec_arraylength;

    
    instrucoes_exec[i2l] = exec_i2l;
    instrucoes_exec[i2f] = exec_i2f;
    instrucoes_exec[i2d] = exec_i2d;
    instrucoes_exec[l2i] = exec_l2i;
    instrucoes_exec[l2f] = exec_l2f;
    instrucoes_exec[l2d] = exec_l2d;
    instrucoes_exec[f2i] = exec_f2i;
    instrucoes_exec[f2l] = exec_f2l;
    instrucoes_exec[f2d] = exec_f2d;
    instrucoes_exec[d2i] = exec_d2i;
    instrucoes_exec[d2l] = exec_d2l;
    instrucoes_exec[d2f] = exec_d2f;
    instrucoes_exec[i2c] = exec_i2c;

    
    instrucoes_exec[dup] = exec_dup;         
    instrucoes_exec[inst_new] = exec_new; 
    instrucoes_exec[putfield] = exec_putfield; 
    instrucoes_exec[putstatic] = exec_putstatic; 
    instrucoes_exec[newarray] = exec_newarray;
    instrucoes_exec[bipush] = exec_bipush;  
    
    
    instrucoes_exec[ldc] = exec_ldc;
    instrucoes_exec[ldc_w] = exec_ldc_w;
    instrucoes_exec[ldc2_w] = exec_ldc2_w;
    instrucoes_exec[getstatic] = exec_getstatic;
    instrucoes_exec[invokevirtual] = exec_invokevirtual;
    instrucoes_exec[invokespecial] = exec_invokespecial; 
    instrucoes_exec[invokestatic] = exec_invokestatic;
    instrucoes_exec[invokeinterface] = exec_invokeinterface;
    
    instrucoes_exec[if_icmpeq] = exec_if_icmpeq;
    instrucoes_exec[if_icmpne] = exec_if_icmpne;
    instrucoes_exec[if_icmplt] = exec_if_icmplt;
    instrucoes_exec[if_icmpge] = exec_if_icmpge;
    instrucoes_exec[if_icmpgt] = exec_if_icmpgt;
    instrucoes_exec[if_icmple] = exec_if_icmple;
    
    
    instrucoes_exec[ifeq] = exec_ifeq;
    instrucoes_exec[ifne] = exec_ifne;
    instrucoes_exec[iflt] = exec_iflt;
    instrucoes_exec[ifge] = exec_ifge;
    instrucoes_exec[ifgt] = exec_ifgt;
    instrucoes_exec[ifle] = exec_ifle;
    instrucoes_exec[ifnull] = exec_ifnull;
    instrucoes_exec[ifnonnull] = exec_ifnonnull;
    instrucoes_exec[inst_goto] = exec_goto;
    instrucoes_exec[goto_w] = exec_goto_w;
    instrucoes_exec[tableswitch] = exec_tableswitch;
    instrucoes_exec[multianewarray] = exec_multianewarray;
    instrucoes_exec[wide] = exec_wide;
    instrucoes_exec[fcmpg] = exec_fcmpg;
    instrucoes_exec[dcmpl] = exec_dcmpl;
    instrucoes_exec[dcmpg] = exec_dcmpg;
    
    instrucoes_exec[print_int] = exec_print_int;
    
    instrucoes_exec[getfield] = exec_getfield;
    
    
    instrucoes_exec[iconst_m1] = exec_iconst_m1;
    instrucoes_exec[iconst_1] = exec_iconst_1;
    instrucoes_exec[iconst_2] = exec_iconst_2;
    instrucoes_exec[iconst_3] = exec_iconst_3;
    instrucoes_exec[iconst_4] = exec_iconst_4;
    instrucoes_exec[iconst_5] = exec_iconst_5;
}



void executar(Frame *frame) {
    while (frame->pc < frame->code_length) {
        byte1 opcode = frame->code[frame->pc];
        InstrucaoFunc func = instrucoes_exec[opcode];
        
        if (func) {
            frame->pc++; 
            func(frame);
        } else {
            printf("Instrução %02x não implementada (PC: %d)\n", opcode, frame->pc);
            break;
        }
    }
}
