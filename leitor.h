//
// Created on 27/10/2025.
//
#ifndef LEITOR_H
#define LEITOR_H

#include <stdio.h>
#include <stdlib.h>
#include "byteTypes.h"
#include "constantPool.h"
#include "accessFlags.h"
#include "attributeStructs.h"
#include "formatoClassFile.h"
#include "metodoInstrucoes.h"

/* Assinatura dos métodos */
byte1 byte1Read(FILE *fp);                                                                // Lê um byte do arquivo e retorna o byte1 
byte2 byte2Read(FILE *fp);                                                                // Lê dois byte
byte4 byte4Read(FILE *fp);                                                                // Lê quatro bytes

/* Funções Auxiliares */
ClassFile * readFile(char *);                                                       // Função que vai ler o arquivo .class
cp_info * readConstantPool(FILE * fp, byte2 constantpoolcount);                        // Lê a constant pool do .class
method_info * readMethod(FILE * fp, byte2 methodscount, cp_info *cp);                  // Lê o método de uma classe
char* decodeInstructionOp(cp_info *cp, byte2 index, byte2 sizeCP);                        // Decodifica os argumentos de uma instrução
char* decodeCode(cp_info *cp,byte2 sizeCP,byte1 *code, byte4 length,instruction *instrucoes);  // Decodifica o código do método
field_info * readField(FILE * fp, byte2 fields_count, cp_info * cp);
byte2 * readInterfaces(FILE * fp, byte2 size);
innerClasses_attribute * readInnerClasses(FILE * fp, cp_info * cp);
exceptions_attribute * readExceptionsAttribute(FILE * fp);
classes * readClasses(FILE * fp);
constantValue_attribute * readConstantValue(FILE * fp);
double decodeDoubleInfo(cp_info * cp);
uint64_t decodeLongInfo(cp_info * cp);
float decodeFloatInfo(cp_info * cp);
int decodeIntegerInfo(cp_info * cp);
signature_attribute * readSignature(FILE * fp);                                     // Lê as flags recebidas (do acesso)
char* organizingFlags(char* flagsOrdemInversa);                                     // Organiza as flags
stackMapTable_attribute * readStackMapTable(FILE * fp);                             // Lê o stack table
stack_map_frame * readStackMapFrame(FILE * fp);
int setOffsetPrinting(int posicao, byte1 offset);                                      // Organiza o offset do stack frame
verification_type_info * readVerificationTypeInfo(FILE * fp);
code_attribute * readCode(FILE * fp, cp_info *cp);
line_number_table * readLineNumberTable(FILE * fp, cp_info *cp);
exception_table * readExceptionTable (FILE * fp, byte2 size);
attribute_info * readAttributes (FILE * fp, cp_info *cp);
source_file_attribute * readSourceFile (FILE * fp);
char* searchNameTag(byte1 tag);
char* decodeStringUTF8(cp_info *cp);
char* decodeNIeNT(cp_info *cp, byte2 index,byte1 tipo);
char* decodeAccessFlags(byte2 flag);
void printClassFile(ClassFile *, FILE *);                                           // Imprime o arquivo .class
void freeInstructions(instruction *instrucoes);
void freeConstantPool(cp_info *cp, byte2 count);
#endif
