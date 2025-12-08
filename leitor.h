

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
#include "java_frontend.h"

byte1 byte1Read(FILE *fp);                                                                
byte2 byte2Read(FILE *fp);                                                                
byte4 byte4Read(FILE *fp);                                                                

ClassFile * readFile(char *);                                                       
cp_info * readConstantPool(FILE * fp, byte2 constantpoolcount);                        
method_info * readMethod(FILE * fp, byte2 methodscount, cp_info *cp);                  
char* decodeInstructionOp(cp_info *cp, byte2 index, byte2 sizeCP);                        
char* decodeCode(cp_info *cp,byte2 sizeCP,byte1 *code, byte4 length,instruction *instrucoes);  
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
signature_attribute * readSignature(FILE * fp);                                     
char* organizingFlags(char* flagsOrdemInversa);                                     
stackMapTable_attribute * readStackMapTable(FILE * fp);                             
stack_map_frame * readStackMapFrame(FILE * fp);
int setOffsetPrinting(int posicao, byte1 offset);                                      
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
void printClassFile(ClassFile *, FILE *);                                           
void freeInstructions(instruction *instrucoes);
void freeConstantPool(cp_info *cp, byte2 count);
void freeMethod(method_info method);
void freeClassFile(ClassFile *cf);
char* getUtf8FromConstantPool(cp_info *cp, byte2 index, byte2 sizeCP) ;
#endif
