#include "leitor.h"
#include "constantPool.h"
#include "attributeStructs.h"
#include <stdio.h>
#include <string.h>

// Functions for decoding specific attributes like Exceptions, Signature, SourceFile, and InnerClasses

void decodeExceptionsAttribute(FILE *fp, exceptions_attribute *excpAux, cp_info *constant_pool) {
    int contadorExcp = 0;
    char *exceptionIndexString;
    fprintf(fp, "Nr.    Exception      Verbose\n");
    for (byte2 *indexExcp = excpAux->exception_index_table; indexExcp < excpAux->exception_index_table + excpAux->number_of_exceptions; indexExcp++) {
        exceptionIndexString = decodeNIeNT(constant_pool, *indexExcp, 1);
        fprintf(fp, "%d    cp_info#%d      %s\n", contadorExcp, *indexExcp, exceptionIndexString);
        free(exceptionIndexString);
        contadorExcp++;
    }
}

void decodeSignatureAttribute(FILE *fp, signature_attribute *sig, cp_info *constant_pool) {
    char *Signature_Index = decodeStringUTF8(constant_pool - 1 + sig->signature_index);
    fprintf(fp, "Signature index: cp_info#%d <%s>\n", sig->signature_index, Signature_Index);
}

void decodeSourceFileAttribute(FILE *fp, source_file_attribute *SourceFile, cp_info *constant_pool) {
    fprintf(fp, "Source File Name Index:        cp_info#%d <%s>\n", SourceFile->source_file_index, decodeStringUTF8(constant_pool + SourceFile->source_file_index - 1));
}

void decodeInnerClassesAttribute(FILE *fp, innerClasses_attribute *innerC, cp_info *constant_pool) {
    fprintf(fp, "Nr.    Inner Class      Outer Class    Inner Name    Access Flags\n");
    char *innerClassString, *outerClassString, *innerNameIndex, *accessFlagsInner;
    classes **vetorClasses = innerC->classes_vector;
    for (int posicaoInncerC = 0; posicaoInncerC < innerC->number_of_classes; posicaoInncerC++) {
        innerClassString = decodeNIeNT(constant_pool, (*(vetorClasses + posicaoInncerC))->inner_class_info_index, 1);
        outerClassString = decodeNIeNT(constant_pool, (*(vetorClasses + posicaoInncerC))->outer_class_info_index, 1);
        innerNameIndex = decodeStringUTF8(constant_pool - 1 + (*(vetorClasses + posicaoInncerC))->inner_name_index);
        accessFlagsInner = decodeAccessFlags((*(vetorClasses + posicaoInncerC))->inner_class_access_flags);
        fprintf(fp, "%d    cp_info#%d      cp_info#%d    cp_info#%d    0x%04x\n", posicaoInncerC, (*(vetorClasses + posicaoInncerC))->inner_class_info_index, (*(vetorClasses + posicaoInncerC))->outer_class_info_index, (*(vetorClasses + posicaoInncerC))->inner_name_index, (*(vetorClasses + posicaoInncerC))->inner_class_access_flags);
        fprintf(fp, "      %s    %s    %s      %s\n", innerClassString, outerClassString, innerNameIndex, accessFlagsInner);
        free(innerClassString);
        free(outerClassString);
        free(innerNameIndex);
        free(accessFlagsInner);
    }
}
