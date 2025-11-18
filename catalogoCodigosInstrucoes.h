// SUMÁRIO DE INSTRUÇÕES DA JVM
// Pego do PDF "JVM8 parte 3 de 3"

#ifndef INSTRUCOES_H
#define INSTRUCOES_H

#include "byteTypes.h"


typedef enum instrucoes_op {
  // Constantes
  nop = 0x00, aconst_null, iconst_m1, iconst_0, iconst_1, iconst_2, iconst_3, iconst_4, iconst_5,
  lconst_0, lconst_1, fconst_0, fconst_1, fconst_2, dconst_0, dconst_1,

  // Carregamento de valores
  bipush = 0x10, sipush, ldc, ldc_w, ldc2_w,
  iload = 0x15, lload, fload, dload, aload,

  // Carregamento indexado
  iload_0 = 0x1a, iload_1, iload_2, iload_3,

  lload_0 = 0x1e, lload_1, lload_2, lload_3,

  fload_0 = 0x22, fload_1, fload_2, fload_3,

  dload_0 = 0x26, dload_1, dload_2, dload_3,

  aload_0 = 0x2a, aload_1, aload_2, aload_3,

  // Carregamento de arrays
  iaload = 0x2e, laload, faload, daload, aaload, baload, caload, saload,

  // Armazenamento de valores
  istore = 0x36, lstore, fstore, dstore, astore,

  // Armazenamento indexado
  istore_0 = 0x3b, istore_1, istore_2, istore_3,

  lstore_0 = 0x3f, lstore_1, lstore_2, lstore_3,

  fstore_0 = 0x43, fstore_1, fstore_2, fstore_3,

  dstore_0 = 0x47, dstore_1, dstore_2, dstore_3,

  astore_0 = 0x4b, astore_1, astore_2, astore_3,

  // Armazenamento em arrays
  iastore = 0x4f, lastore, fastore, dastore, aastore, bastore, castore, sastore,

  // Operações de pilha
  pop = 0x57, pop2, dup, dup_x1, dup_x2, dup2, dup2_x1, dup2_x2, swap,

  // Operações aritméticas
  iadd = 0x60, ladd, fadd, dadd,
  isub = 0x64, lsub, fsub, dsub,
  imul = 0x68, lmul, fmul, dmul,
  idiv = 0x6c, inst_ldiv, fdiv, ddiv,
  irem = 0x70, lrem, frem, inst_drem,
  ineg = 0x74, lneg, fneg, dneg,

  // Operações lógicas e de deslocamento
  ishl = 0x78, lshl, ishr, lshr, iushr, lushr,
  iand = 0x7e, land, ior, lor, ixor, lxor,  iinc,

  // Conversões
  i2l = 0x85, i2f, i2d, l2i, l2f, l2d,
  f2i = 0x8b, f2l, f2d, d2i, d2l, d2f,
  i2b = 0x91, i2c, i2s,

  // Comparações
  lcmp = 0x94, fcmpl, fcmpg, dcmpl, dcmpg,

  // Instruções condicionais
  ifeq = 0x99, ifne, iflt, ifge, ifgt, ifle,
  if_icmpeq = 0x9f, if_icmpne, if_icmplt, if_icmpge, if_icmpgt, if_icmple,
  if_acmpeg = 0xa5, if_acmpne,

  // Controle de fluxo
  inst_goto = 0xa7, jsr, ret, tableswitch, lookupswitch,

  // Retornos
  ireturn = 0xac, lreturn, freturn, dreturn, areturn, inst_return,

  // Manipulação de campos e métodos
  getstatic = 0xb2, putstatic, getfield, putfield,
  invokevirtual, invokespecial, invokestatic, invokeinterface,

  // Criação e manipulação de objetos
  inst_new = 0xbb, newarray, anewarray, arraylength, athrow, checkcast, instanceof,
  monitorenter, monitorexit,

  // Instruções diversas
  wide = 0xc4, multianewarray, ifnull, ifnonnull, goto_w, jsr_w,
} instrucoes_op;


#endif