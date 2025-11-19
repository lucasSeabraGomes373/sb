//
// Created on 27/10/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "byteTypes.h"
#include "formatoClassFile.h"
#include "metodoInstrucoes.h"

const int count_enum = jsr_w - nop; // ultima instrução - primeira -> define numero total

instruction* InstructionBuild(void){

	instruction *instructions = malloc(count_enum*sizeof(instruction)); // Alocação de memória para todas as instruções

	for (int i = 0;i<count_enum;i++){
		switch(i){          // Case switch para definir o opcode, argumento e nome de cada instrução
			case nop:
				strcpy(instructions[i].instr_name,"nop"); // O i define o numero da instrução e o que segue ele (.instr_name) é o nome dentro da struct
				instructions[i].opcode = nop; // Opcode não usado agora, será usado futuramente na jvm. 
				instructions[i].numarg = 0; // Usado na função decodificarCode para gerar um switch case e definir as operações que precisam ser aplicadas para ser funcional
			break;
			case aconst_null:
				strcpy(instructions[i].instr_name,"aconst_null");
				instructions[i].opcode = aconst_null;
				instructions[i].numarg = 0;
			break;
			case iconst_m1:
				strcpy(instructions[i].instr_name,"iconst_m1");
				instructions[i].opcode = iconst_m1;
				instructions[i].numarg = 0;
			break;
			case iconst_0:
				strcpy(instructions[i].instr_name,"iconst_0");
				instructions[i].opcode = iconst_0;
				instructions[i].numarg = 0;
			break;
			case iconst_1:
				strcpy(instructions[i].instr_name,"iconst_1");
				instructions[i].opcode = iconst_1;
				instructions[i].numarg = 0;
			break;
			case iconst_2:
				strcpy(instructions[i].instr_name,"iconst_2");
				instructions[i].opcode = iconst_2;
				instructions[i].numarg = 0;
			break;
			case iconst_3:
				strcpy(instructions[i].instr_name,"iconst_3");
				instructions[i].opcode = iconst_3;
				instructions[i].numarg = 0;
			break;
			case iconst_4:
				strcpy(instructions[i].instr_name,"iconst_4");
				instructions[i].opcode = iconst_4;
				instructions[i].numarg = 0;
			break;
			case iconst_5:
				strcpy(instructions[i].instr_name,"iconst_5");
				instructions[i].opcode = iconst_5;
				instructions[i].numarg = 0;
			break;
			case lconst_0:
				strcpy(instructions[i].instr_name,"lconst_0");
				instructions[i].opcode = lconst_0;
				instructions[i].numarg = 0;
			break;
			case lconst_1:
				strcpy(instructions[i].instr_name,"lconst_1");
				instructions[i].opcode = lconst_1;
				instructions[i].numarg = 0;
			break;
			case fconst_0:
				strcpy(instructions[i].instr_name,"fconst_0");
				instructions[i].opcode = fconst_0;
				instructions[i].numarg = 0;
			break;
			case fconst_1:
				strcpy(instructions[i].instr_name,"fconst_1");
				instructions[i].opcode = fconst_1;
				instructions[i].numarg = 0;
			break;
			case fconst_2:
				strcpy(instructions[i].instr_name,"fconst_2");
				instructions[i].opcode = fconst_2;
				instructions[i].numarg = 0;
			break;
			case dconst_0:
				strcpy(instructions[i].instr_name,"dconst_0");
				instructions[i].opcode = dconst_0;
				instructions[i].numarg = 0;
			break;
			case dconst_1:
				strcpy(instructions[i].instr_name,"dconst_1");
				instructions[i].opcode = dconst_1;
				instructions[i].numarg = 0;
			break;
			case bipush:
				strcpy(instructions[i].instr_name,"bipush");
				instructions[i].opcode = bipush;
				instructions[i].numarg = 1;
			break;
			case sipush:
				strcpy(instructions[i].instr_name,"sipush");
				instructions[i].opcode = sipush;
				instructions[i].numarg = 2;
			break;
			case ldc:
				strcpy(instructions[i].instr_name,"ldc");
				instructions[i].opcode = ldc;
				instructions[i].numarg = 1;
			break;
			case ldc_w:
				strcpy(instructions[i].instr_name,"ldc_w");
				instructions[i].opcode = ldc_w;
				instructions[i].numarg = 2;
			break;
			case ldc2_w:
				strcpy(instructions[i].instr_name,"ldc2_w");
				instructions[i].opcode = ldc2_w;
				instructions[i].numarg = 2;
			break;
			case iload:
				strcpy(instructions[i].instr_name,"iload");
				instructions[i].opcode = iload;
				instructions[i].numarg = 1;
			break;
			case lload:
				strcpy(instructions[i].instr_name,"lload");
				instructions[i].opcode = lload;
				instructions[i].numarg = 1;
			break;
			case fload:
				strcpy(instructions[i].instr_name,"fload");
				instructions[i].opcode = fload;
				instructions[i].numarg = 1;
			break;
			case dload:
				strcpy(instructions[i].instr_name,"dload");
				instructions[i].opcode = dload;
				instructions[i].numarg = 1;
			break;
			case aload:
				strcpy(instructions[i].instr_name,"aload");
				instructions[i].opcode = aload;
				instructions[i].numarg = 1;
			break;
			case iload_0:
				strcpy(instructions[i].instr_name,"iload_0");
				instructions[i].opcode = iload_0;
				instructions[i].numarg = 0;
			break;
			case iload_1:
				strcpy(instructions[i].instr_name,"iload_1");
				instructions[i].opcode = iload_1;
				instructions[i].numarg = 0;
			break;
			case iload_2:
				strcpy(instructions[i].instr_name,"iload_2");
				instructions[i].opcode = iload_2;
				instructions[i].numarg = 0;
			break;
			case iload_3:
				strcpy(instructions[i].instr_name,"iload_3");
				instructions[i].opcode = iload_3;
				instructions[i].numarg = 0;
			break;
			case lload_0:
				strcpy(instructions[i].instr_name,"lload_0");
				instructions[i].opcode = lload_0;
				instructions[i].numarg = 0;
			break;
			case lload_1:
				strcpy(instructions[i].instr_name,"lload_1");
				instructions[i].opcode = lload_1;
				instructions[i].numarg = 0;
			break;
			case lload_2:
				strcpy(instructions[i].instr_name,"lload_2");
				instructions[i].opcode = lload_2;
				instructions[i].numarg = 0;
			break;
			case lload_3:
				strcpy(instructions[i].instr_name,"lload_3");
				instructions[i].opcode = lload_3;
				instructions[i].numarg = 0;
			break;
			case fload_0:
				strcpy(instructions[i].instr_name,"fload_0");
				instructions[i].opcode = fload_0;
				instructions[i].numarg = 0;
			break;
			case fload_1:
				strcpy(instructions[i].instr_name,"fload_1");
				instructions[i].opcode = fload_1;
				instructions[i].numarg = 0;
			break;
			case fload_2:
				strcpy(instructions[i].instr_name,"fload_2");
				instructions[i].opcode = fload_2;
				instructions[i].numarg = 0;
			break;
			case fload_3:
				strcpy(instructions[i].instr_name,"fload_3");
				instructions[i].opcode = fload_3;
				instructions[i].numarg = 0;
			break;
			case dload_0:
				strcpy(instructions[i].instr_name,"dload_0");
				instructions[i].opcode = dload_0;
				instructions[i].numarg = 0;
			break;
			case dload_1:
				strcpy(instructions[i].instr_name,"dload_1");
				instructions[i].opcode = dload_1;
				instructions[i].numarg = 0;
			break;
			case dload_2:
				strcpy(instructions[i].instr_name,"dload_2");
				instructions[i].opcode = dload_2;
				instructions[i].numarg = 0;
			break;
			case dload_3:
				strcpy(instructions[i].instr_name,"dload_3");
				instructions[i].opcode = dload_3;
				instructions[i].numarg = 0;
			break;
			case aload_0:
				strcpy(instructions[i].instr_name,"aload_0");
				instructions[i].opcode = aload_0;
				instructions[i].numarg = 0;
			break;
			case aload_1:
				strcpy(instructions[i].instr_name,"aload_1");
				instructions[i].opcode = aload_1;
				instructions[i].numarg = 0;
			break;
			case aload_2:
				strcpy(instructions[i].instr_name,"aload_2");
				instructions[i].opcode = aload_2;
				instructions[i].numarg = 0;
			break;
			case aload_3:
				strcpy(instructions[i].instr_name,"aload_3");
				instructions[i].opcode = aload_3;
				instructions[i].numarg = 0;
			break;
			case iaload:
				strcpy(instructions[i].instr_name,"iaload");
				instructions[i].opcode = iaload;
				instructions[i].numarg = 0;
			break;
			case laload:
				strcpy(instructions[i].instr_name,"laload");
				instructions[i].opcode = laload;
				instructions[i].numarg = 0;
			break;
			case faload:
				strcpy(instructions[i].instr_name,"faload");
				instructions[i].opcode = faload;
				instructions[i].numarg = 0;
			break;
			case daload:
				strcpy(instructions[i].instr_name,"daload");
				instructions[i].opcode = daload;
				instructions[i].numarg = 0;
			break;
			case aaload:
				strcpy(instructions[i].instr_name,"aaload");
				instructions[i].opcode = aaload;
				instructions[i].numarg = 0;
			break;
			case baload:
				strcpy(instructions[i].instr_name,"baload");
				instructions[i].opcode = baload;
				instructions[i].numarg = 0;
			break;
			case caload:
				strcpy(instructions[i].instr_name,"caload");
				instructions[i].opcode = caload;
				instructions[i].numarg = 0;
			break;
			case saload:
				strcpy(instructions[i].instr_name,"saload");
				instructions[i].opcode = saload;
				instructions[i].numarg = 0;
			break;
			case istore:
				strcpy(instructions[i].instr_name,"istore");
				instructions[i].opcode = istore;
				instructions[i].numarg = 1;
			break;
			case lstore:
				strcpy(instructions[i].instr_name,"lstore");
				instructions[i].opcode = lstore;
				instructions[i].numarg = 1;
			break;
			case fstore:
				strcpy(instructions[i].instr_name,"fstore");
				instructions[i].opcode = fstore;
				instructions[i].numarg = 1;
			break;
			case dstore:
				strcpy(instructions[i].instr_name,"dstore");
				instructions[i].opcode = dstore;
				instructions[i].numarg = 1;
			break;
			case astore:
				strcpy(instructions[i].instr_name,"astore");
				instructions[i].opcode = astore;
				instructions[i].numarg = 1;
			break;
			case istore_0:
				strcpy(instructions[i].instr_name,"istore_0");
				instructions[i].opcode = istore_0;
				instructions[i].numarg = 0;
			break;
			case istore_1:
				strcpy(instructions[i].instr_name,"istore_1");
				instructions[i].opcode = istore_1;
				instructions[i].numarg = 0;
			break;
			case istore_2:
				strcpy(instructions[i].instr_name,"istore_2");
				instructions[i].opcode = istore_2;
				instructions[i].numarg = 0;
			break;
			case istore_3:
				strcpy(instructions[i].instr_name,"istore_3");
				instructions[i].opcode = istore_3;
				instructions[i].numarg = 0;
			break;
			case lstore_0:
				strcpy(instructions[i].instr_name,"lstore_0");
				instructions[i].opcode = lstore_0;
				instructions[i].numarg = 0;
			break;
			case lstore_1:
				strcpy(instructions[i].instr_name,"lstore_1");
				instructions[i].opcode = lstore_1;
				instructions[i].numarg = 0;
			break;
			case lstore_2:
				strcpy(instructions[i].instr_name,"lstore_2");
				instructions[i].opcode = lstore_2;
				instructions[i].numarg = 0;
			break;
			case lstore_3:
				strcpy(instructions[i].instr_name,"lstore_3");
				instructions[i].opcode = lstore_3;
				instructions[i].numarg = 0;
			break;
			case fstore_0:
				strcpy(instructions[i].instr_name,"fstore_0");
				instructions[i].opcode = fstore_0;
				instructions[i].numarg = 0;
			break;
			case fstore_1:
				strcpy(instructions[i].instr_name,"fstore_1");
				instructions[i].opcode = fstore_1;
				instructions[i].numarg = 0;
			break;
			case fstore_2:
				strcpy(instructions[i].instr_name,"fstore_2");
				instructions[i].opcode = fstore_2;
				instructions[i].numarg = 0;
			break;
			case fstore_3:
				strcpy(instructions[i].instr_name,"fstore_3");
				instructions[i].opcode = fstore_3;
				instructions[i].numarg = 0;
			break;
			case dstore_0:
				strcpy(instructions[i].instr_name,"dstore_0");
				instructions[i].opcode = dstore_0;
				instructions[i].numarg = 0;
			break;
			case dstore_1:
				strcpy(instructions[i].instr_name,"dstore_1");
				instructions[i].opcode = dstore_1;
				instructions[i].numarg = 0;
			break;
			case dstore_2:
				strcpy(instructions[i].instr_name,"dstore_2");
				instructions[i].opcode = dstore_2;
				instructions[i].numarg = 0;
			break;
			case dstore_3:
				strcpy(instructions[i].instr_name,"dstore_3");
				instructions[i].opcode = dstore_3;
				instructions[i].numarg = 0;
			break;
			case astore_0:
				strcpy(instructions[i].instr_name,"astore_0");
				instructions[i].opcode = astore_0;
				instructions[i].numarg = 0;
			break;
			case astore_1:
				strcpy(instructions[i].instr_name,"astore_1");
				instructions[i].opcode = astore_1;
				instructions[i].numarg = 0;
			break;
			case astore_2:
				strcpy(instructions[i].instr_name,"astore_2");
				instructions[i].opcode = astore_2;
				instructions[i].numarg = 0;
			break;
			case astore_3:
				strcpy(instructions[i].instr_name,"astore_3");
				instructions[i].opcode = astore_3;
				instructions[i].numarg = 0;
			break;
			case iastore:
				strcpy(instructions[i].instr_name,"iastore");
				instructions[i].opcode = iastore;
				instructions[i].numarg = 0;
			break;
			case lastore:
				strcpy(instructions[i].instr_name,"lastore");
				instructions[i].opcode = lastore;
				instructions[i].numarg = 0;
			break;
			case fastore:
				strcpy(instructions[i].instr_name,"fastore");
				instructions[i].opcode = fastore;
				instructions[i].numarg = 0;
			break;
			case dastore:
				strcpy(instructions[i].instr_name,"dastore");
				instructions[i].opcode = dastore;
				instructions[i].numarg = 0;
			break;
			case aastore:
				strcpy(instructions[i].instr_name,"aastore");
				instructions[i].opcode = aastore;
				instructions[i].numarg = 0;
			break;
			case bastore:
				strcpy(instructions[i].instr_name,"bastore");
				instructions[i].opcode = bastore;
				instructions[i].numarg = 0;
			break;
			case castore:
				strcpy(instructions[i].instr_name,"castore");
				instructions[i].opcode = castore;
				instructions[i].numarg = 0;
			break;
			case sastore:
				strcpy(instructions[i].instr_name,"sastore");
				instructions[i].opcode = sastore;
				instructions[i].numarg = 0;
			break;
			case pop:
				strcpy(instructions[i].instr_name,"pop");
				instructions[i].opcode = pop;
				instructions[i].numarg = 0;
			break;
			case pop2:
				strcpy(instructions[i].instr_name,"pop2");
				instructions[i].opcode = pop2;
				instructions[i].numarg = 0;
			break;
			case dup:
				strcpy(instructions[i].instr_name,"dup");
				instructions[i].opcode = dup;
				instructions[i].numarg = 0;
			break;
			case dup_x1:
				strcpy(instructions[i].instr_name,"dup_x1");
				instructions[i].opcode = dup_x1;
				instructions[i].numarg = 0;
			break;
			case dup_x2:
				strcpy(instructions[i].instr_name,"dup_x2");
				instructions[i].opcode = dup_x2;
				instructions[i].numarg = 0;
			break;
			case dup2:
				strcpy(instructions[i].instr_name,"dup2");
				instructions[i].opcode = dup2;
				instructions[i].numarg = 0;
			break;
			case dup2_x1:
				strcpy(instructions[i].instr_name,"dup2_x1");
				instructions[i].opcode = dup2_x1;
				instructions[i].numarg = 0;
			break;
			case dup2_x2:
				strcpy(instructions[i].instr_name,"dup2_x2");
				instructions[i].opcode = dup2_x2;
				instructions[i].numarg = 0;
			break;
			case swap:
				strcpy(instructions[i].instr_name,"swap");
				instructions[i].opcode = swap;
				instructions[i].numarg = 0;
			break;
			case iadd:
				strcpy(instructions[i].instr_name,"iadd");
				instructions[i].opcode = iadd;
				instructions[i].numarg = 0;
			break;
			case ladd:
				strcpy(instructions[i].instr_name,"ladd");
				instructions[i].opcode = ladd;
				instructions[i].numarg = 0;
			break;
			case fadd:
				strcpy(instructions[i].instr_name,"fadd");
				instructions[i].opcode = fadd;
				instructions[i].numarg = 0;
			break;
			case dadd:
				strcpy(instructions[i].instr_name,"dadd");
				instructions[i].opcode = dadd;
				instructions[i].numarg = 0;
			break;
			case isub:
				strcpy(instructions[i].instr_name,"isub");
				instructions[i].opcode = isub;
				instructions[i].numarg = 0;
			break;
			case lsub:
				strcpy(instructions[i].instr_name,"lsub");
				instructions[i].opcode = lsub;
				instructions[i].numarg = 0;
			break;
			case fsub:
				strcpy(instructions[i].instr_name,"fsub");
				instructions[i].opcode = fsub;
				instructions[i].numarg = 0;
			break;
			case dsub:
				strcpy(instructions[i].instr_name,"dsub");
				instructions[i].opcode = dsub;
				instructions[i].numarg = 0;
			break;
			case imul:
				strcpy(instructions[i].instr_name,"imul");
				instructions[i].opcode = imul;
				instructions[i].numarg = 0;
			break;
			case lmul:
				strcpy(instructions[i].instr_name,"lmul");
				instructions[i].opcode = lmul;
				instructions[i].numarg = 0;
			break;
			case fmul:
				strcpy(instructions[i].instr_name,"fmul");
				instructions[i].opcode = fmul;
				instructions[i].numarg = 0;
			break;
			case dmul:
				strcpy(instructions[i].instr_name,"dmul");
				instructions[i].opcode = dmul;
				instructions[i].numarg = 0;
			break;
			case idiv:
				strcpy(instructions[i].instr_name,"idiv");
				instructions[i].opcode = idiv;
				instructions[i].numarg = 0;
			break;
			case inst_ldiv:
				strcpy(instructions[i].instr_name,"ldiv");
				instructions[i].opcode = inst_ldiv;
				instructions[i].numarg = 0;
			break;
			case fdiv:
				strcpy(instructions[i].instr_name,"fdiv");
				instructions[i].opcode = fdiv;
				instructions[i].numarg = 0;
			break;
			case ddiv:
				strcpy(instructions[i].instr_name,"ddiv");
				instructions[i].opcode = ddiv;
				instructions[i].numarg = 0;
			break;
			case irem:
				strcpy(instructions[i].instr_name,"irem");
				instructions[i].opcode = irem;
				instructions[i].numarg = 0;
			break;
			case lrem:
				strcpy(instructions[i].instr_name,"lrem");
				instructions[i].opcode = lrem;
				instructions[i].numarg = 0;
			break;
			case frem:
				strcpy(instructions[i].instr_name,"frem");
				instructions[i].opcode = frem;
				instructions[i].numarg = 0;
			break;
			case inst_drem:
				strcpy(instructions[i].instr_name,"drem");
				instructions[i].opcode = inst_drem;
				instructions[i].numarg = 0;
			break;
			case ineg:
				strcpy(instructions[i].instr_name,"ineg");
				instructions[i].opcode = ineg;
				instructions[i].numarg = 0;
			break;
			case lneg:
				strcpy(instructions[i].instr_name,"lneg");
				instructions[i].opcode = lneg;
				instructions[i].numarg = 0;
			break;
			case fneg:
				strcpy(instructions[i].instr_name,"fneg");
				instructions[i].opcode = fneg;
				instructions[i].numarg = 0;
			break;
			case dneg:
				strcpy(instructions[i].instr_name,"dneg");
				instructions[i].opcode = dneg;
				instructions[i].numarg = 0;
			break;
			case ishl:
				strcpy(instructions[i].instr_name,"ishl");
				instructions[i].opcode = ishl;
				instructions[i].numarg = 0;
			break;
			case lshl:
				strcpy(instructions[i].instr_name,"lshl");
				instructions[i].opcode = lshl;
				instructions[i].numarg = 0;
			break;
			case ishr:
				strcpy(instructions[i].instr_name,"ishr");
				instructions[i].opcode = ishr;
				instructions[i].numarg = 0;
			break;
			case lshr:
				strcpy(instructions[i].instr_name,"lshr");
				instructions[i].opcode = lshr;
				instructions[i].numarg = 0;
			break;
			case iushr:
				strcpy(instructions[i].instr_name,"iushr");
				instructions[i].opcode = iushr;
				instructions[i].numarg = 0;
			break;
			case lushr:
				strcpy(instructions[i].instr_name,"lushr");
				instructions[i].opcode = lushr;
				instructions[i].numarg = 0;
			break;
			case iand:
				strcpy(instructions[i].instr_name,"iand");
				instructions[i].opcode = iand;
				instructions[i].numarg = 0;
			break;
			case land:
				strcpy(instructions[i].instr_name,"land");
				instructions[i].opcode = land;
				instructions[i].numarg = 0;
			break;
			case ior:
				strcpy(instructions[i].instr_name,"ior");
				instructions[i].opcode = ior;
				instructions[i].numarg = 0;
			break;
			case lor:
				strcpy(instructions[i].instr_name,"lor");
				instructions[i].opcode = lor;
				instructions[i].numarg = 0;
			break;
			case ixor:
				strcpy(instructions[i].instr_name,"ixor");
				instructions[i].opcode = ixor;
				instructions[i].numarg = 0;
			break;
			case lxor:
				strcpy(instructions[i].instr_name,"lxor");
				instructions[i].opcode = lxor;
				instructions[i].numarg = 0;
			break;
			case iinc:
				strcpy(instructions[i].instr_name,"iinc");
				instructions[i].opcode = iinc;
				instructions[i].numarg = 2;
			break;
			case i2l:
				strcpy(instructions[i].instr_name,"i2l");
				instructions[i].opcode = i2l;
				instructions[i].numarg = 0;
			break;
			case i2f:
				strcpy(instructions[i].instr_name,"i2f");
				instructions[i].opcode = i2f;
				instructions[i].numarg = 0;
			break;
			case i2d:
				strcpy(instructions[i].instr_name,"i2d");
				instructions[i].opcode = i2d;
				instructions[i].numarg = 0;
			break;
			case l2i:
				strcpy(instructions[i].instr_name,"l2i");
				instructions[i].opcode = l2i;
				instructions[i].numarg = 0;
			break;
			case l2f:
				strcpy(instructions[i].instr_name,"l2f");
				instructions[i].opcode = l2f;
				instructions[i].numarg = 0;
			break;
			case l2d:
				strcpy(instructions[i].instr_name,"l2d");
				instructions[i].opcode = l2d;
				instructions[i].numarg = 0;
			break;
			case f2i:
				strcpy(instructions[i].instr_name,"f2i");
				instructions[i].opcode = f2i;
				instructions[i].numarg = 0;
			break;
			case f2l:
				strcpy(instructions[i].instr_name,"f2l");
				instructions[i].opcode = f2l;
				instructions[i].numarg = 0;
			break;
			case f2d:
				strcpy(instructions[i].instr_name,"f2d");
				instructions[i].opcode = f2d;
				instructions[i].numarg = 0;
			break;
			case d2i:
				strcpy(instructions[i].instr_name,"d2i");
				instructions[i].opcode = d2i;
				instructions[i].numarg = 0;
			break;
			case d2l:
				strcpy(instructions[i].instr_name,"d2l");
				instructions[i].opcode = d2l;
				instructions[i].numarg = 0;
			break;
			case d2f:
				strcpy(instructions[i].instr_name,"d2f");
				instructions[i].opcode = d2f;
				instructions[i].numarg = 0;
			break;
			case i2b:
				strcpy(instructions[i].instr_name,"i2b");
				instructions[i].opcode = i2b;
				instructions[i].numarg = 0;
			break;
			case i2c:
				strcpy(instructions[i].instr_name,"i2c");
				instructions[i].opcode = i2c;
				instructions[i].numarg = 0;
			break;
			case i2s:
				strcpy(instructions[i].instr_name,"i2s");
				instructions[i].opcode = i2s;
				instructions[i].numarg = 0;
			break;
			case lcmp:
				strcpy(instructions[i].instr_name,"lcmp");
				instructions[i].opcode = lcmp;
				instructions[i].numarg = 0;
			break;
			case fcmpl:
				strcpy(instructions[i].instr_name,"fcmpl");
				instructions[i].opcode = fcmpl;
				instructions[i].numarg = 0;
			break;
			case fcmpg:
				strcpy(instructions[i].instr_name,"fcmpg");
				instructions[i].opcode = fcmpg;
				instructions[i].numarg = 0;
			break;
			case dcmpl:
				strcpy(instructions[i].instr_name,"dcmpl");
				instructions[i].opcode = dcmpl;
				instructions[i].numarg = 0;
			break;
			case dcmpg:
				strcpy(instructions[i].instr_name,"dcmpg");
				instructions[i].opcode = dcmpg;
				instructions[i].numarg = 0;
			break;
			case ifeq:
				strcpy(instructions[i].instr_name,"ifeq");
				instructions[i].opcode = ifeq;
				instructions[i].numarg = 2;
			break;
			case ifne:
				strcpy(instructions[i].instr_name,"ifne");
				instructions[i].opcode = ifne;
				instructions[i].numarg = 2;
			break;
			case iflt:
				strcpy(instructions[i].instr_name,"iflt");
				instructions[i].opcode = iflt;
				instructions[i].numarg = 2;
			break;
			case ifge:
				strcpy(instructions[i].instr_name,"ifge");
				instructions[i].opcode = ifge;
				instructions[i].numarg = 2;
			break;
			case ifgt:
				strcpy(instructions[i].instr_name,"ifgt");
				instructions[i].opcode = ifgt;
				instructions[i].numarg = 2;
			break;
			case ifle:
				strcpy(instructions[i].instr_name,"ifle");
				instructions[i].opcode = ifle;
				instructions[i].numarg = 2;
			break;
			case if_icmpeq:
				strcpy(instructions[i].instr_name,"if_icmpeq");
				instructions[i].opcode = if_icmpeq;
				instructions[i].numarg = 2;
			break;
			case if_icmpne:
				strcpy(instructions[i].instr_name,"if_icmpne");
				instructions[i].opcode = if_icmpne;
				instructions[i].numarg = 2;
			break;
			case if_icmplt:
				strcpy(instructions[i].instr_name,"if_icmplt");
				instructions[i].opcode = if_icmplt;
				instructions[i].numarg = 2;
			break;
			case if_icmpge:
				strcpy(instructions[i].instr_name,"if_icmpge");
				instructions[i].opcode = if_icmpge;
				instructions[i].numarg = 2;
			break;
			case if_icmpgt:
				strcpy(instructions[i].instr_name,"if_icmpgt");
				instructions[i].opcode = if_icmpgt;
				instructions[i].numarg = 2;
			break;
			case if_icmple:
				strcpy(instructions[i].instr_name,"if_icmple");
				instructions[i].opcode = if_icmple;
				instructions[i].numarg = 2;
			break;
			case if_acmpeq:
				strcpy(instructions[i].instr_name,"if_acmpeg");
				instructions[i].opcode = if_acmpeq;
				instructions[i].numarg = 2;
			break;
			case if_acmpne:
				strcpy(instructions[i].instr_name,"if_acmpne");
				instructions[i].opcode = if_acmpne;
				instructions[i].numarg = 2;
			break;
			case inst_goto:
				strcpy(instructions[i].instr_name,"goto");
				instructions[i].opcode = inst_goto;
				instructions[i].numarg = 2;
			break;
			case jsr:
				strcpy(instructions[i].instr_name,"jsr");
				instructions[i].opcode = jsr;
				instructions[i].numarg = 2;
			break;
			case ret:
				strcpy(instructions[i].instr_name,"ret");
				instructions[i].opcode = ret;
				instructions[i].numarg = 1;
			break;
			case tableswitch:
				strcpy(instructions[i].instr_name,"tableswitch");
				instructions[i].opcode = tableswitch;
				instructions[i].numarg = 0;
			break;
			case lookupswitch:
				strcpy(instructions[i].instr_name,"lookupswitch");
				instructions[i].opcode = lookupswitch;
				instructions[i].numarg = 0;
			break;
			case ireturn:
				strcpy(instructions[i].instr_name,"ireturn");
				instructions[i].opcode = ireturn;
				instructions[i].numarg = 0;
			break;
			case lreturn:
				strcpy(instructions[i].instr_name,"lreturn");
				instructions[i].opcode = lreturn;
				instructions[i].numarg = 0;
			break;
			case freturn:
				strcpy(instructions[i].instr_name,"freturn");
				instructions[i].opcode = freturn;
				instructions[i].numarg = 0;
			break;
			case dreturn:
				strcpy(instructions[i].instr_name,"dreturn");
				instructions[i].opcode = dreturn;
				instructions[i].numarg = 0;
			break;
			case areturn:
				strcpy(instructions[i].instr_name,"areturn");
				instructions[i].opcode = areturn;
				instructions[i].numarg = 0;
			break;
			case inst_return:
				strcpy(instructions[i].instr_name,"return");
				instructions[i].opcode = inst_return;
				instructions[i].numarg = 0;
			break;
			case getstatic:
				strcpy(instructions[i].instr_name,"getstatic");
				instructions[i].opcode = getstatic;
				instructions[i].numarg = 2;
			break;
			case putstatic:
				strcpy(instructions[i].instr_name,"putstatic");
				instructions[i].opcode = putstatic;
				instructions[i].numarg = 2;
			break;
			case getfield:
				strcpy(instructions[i].instr_name,"getfield");
				instructions[i].opcode = getfield;
				instructions[i].numarg = 2;
			break;
			case putfield:
				strcpy(instructions[i].instr_name,"putfield");
				instructions[i].opcode = putfield;
				instructions[i].numarg = 2;
			break;
			case invokevirtual:
				strcpy(instructions[i].instr_name,"invokevirtual");
				instructions[i].opcode = invokevirtual;
				instructions[i].numarg = 2;
			break;
			case invokespecial:
				strcpy(instructions[i].instr_name,"invokespecial");
				instructions[i].opcode = invokespecial;
				instructions[i].numarg = 2;
			break;
			case invokestatic:
				strcpy(instructions[i].instr_name,"invokestatic");
				instructions[i].opcode = invokestatic;
				instructions[i].numarg = 2;
			break;
			case invokeinterface:
				strcpy(instructions[i].instr_name,"invokeinterface");
				instructions[i].opcode = invokeinterface;
				instructions[i].numarg = 4;
			break;
			case inst_new:
				strcpy(instructions[i].instr_name,"new");
				instructions[i].opcode = inst_new;
				instructions[i].numarg = 2;
			break;
			case newarray:
				strcpy(instructions[i].instr_name,"newarray");
				instructions[i].opcode = newarray;
				instructions[i].numarg = 1;
			break;
			case anewarray:
				strcpy(instructions[i].instr_name,"anewarray");
				instructions[i].opcode = anewarray;
				instructions[i].numarg = 2;
			break;
			case arraylength:
				strcpy(instructions[i].instr_name,"arraylength");
				instructions[i].opcode = arraylength;
				instructions[i].numarg = 0;
			break;
			case athrow:
				strcpy(instructions[i].instr_name,"athrow");
				instructions[i].opcode = athrow;
				instructions[i].numarg = 0;
			break;
			case checkcast:
				strcpy(instructions[i].instr_name,"checkcast");
				instructions[i].opcode = checkcast;
				instructions[i].numarg = 2;
			break;
			case instanceof:
				strcpy(instructions[i].instr_name,"instanceof");
				instructions[i].opcode = instanceof;
				instructions[i].numarg = 2;
			break;
			case monitorenter:
				strcpy(instructions[i].instr_name,"monitorenter");
				instructions[i].opcode = monitorenter;
				instructions[i].numarg = 0;
			break;
			case monitorexit:
				strcpy(instructions[i].instr_name,"monitorexit");
				instructions[i].opcode = monitorexit;
				instructions[i].numarg = 0;
			break;
			case wide:
				strcpy(instructions[i].instr_name,"wide");
				instructions[i].opcode = wide;
				instructions[i].numarg = 5;
			break;
			case multianewarray:
				strcpy(instructions[i].instr_name,"multianewarray");
				instructions[i].opcode = multianewarray;
				instructions[i].numarg = 3;
			break;
			case ifnull:
				strcpy(instructions[i].instr_name,"ifnull");
				instructions[i].opcode = ifnull;
				instructions[i].numarg = 2;
			break;
			case ifnonnull:
				strcpy(instructions[i].instr_name,"ifnonnull");
				instructions[i].opcode = ifnonnull;
				instructions[i].numarg = 2;
			break;
			case goto_w:
				strcpy(instructions[i].instr_name,"goto_w");
				instructions[i].opcode = goto_w;
				instructions[i].numarg = 4;
			break;
			case jsr_w:
				strcpy(instructions[i].instr_name,"jsr_w");
				instructions[i].opcode = jsr_w;
				instructions[i].numarg = 4;
			break;

			default:
				strcpy(instructions[i].instr_name,"undefined");
				instructions[i].opcode = 255;
				instructions[i].numarg = -1;
			break;
		}

		if(instructions[i].numarg>0){
			instructions[i].tipoarg = malloc(instructions[i].numarg*sizeof(int));
		}
		else{
			instructions[i].tipoarg = NULL;
		}
	}

	return(instructions);
}
void freeInstructions(instruction *instrucoes) {
	if (!instrucoes) return;

	for (int i = 0; i < count_enum; i++) {
		if (instrucoes[i].tipoarg) {
			free(instrucoes[i].tipoarg);
		}
	}

	free(instrucoes);
}
