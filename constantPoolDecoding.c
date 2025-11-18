#include "leitor.h"
#include "constantPool.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// Functions for decoding constant pool entries

char* decodeInstructionOp(cp_info *cp,byte2 index, byte2 sizeCP){

	char *retorno = malloc(100*sizeof(char));
	char *stringNomeClasse;
	char *stringNomeMetodo;
	char *stringGeral;
	char *ponteiro2pontos;
	cp_info *cp_aux = cp+index-1;


	if (index < sizeCP) {
		switch(cp_aux->tag){
			case CONSTANT_Methodref:

				stringNomeClasse = decodeNIeNT(cp,cp_aux->UnionCP.CONSTANT_Methodref.class_index,1);

				stringNomeMetodo = decodeNIeNT(cp,cp_aux->UnionCP.CONSTANT_Methodref.name_and_type_index,2);

				ponteiro2pontos = strchr(stringNomeMetodo,':');
				*ponteiro2pontos = '\0';


				strcpy(retorno,"<");
				strcat(retorno,stringNomeClasse);
				strcat(retorno,".");
				strcat(retorno,stringNomeMetodo);
				strcat(retorno,">");
				break;

			case CONSTANT_Fieldref:

				stringNomeClasse = decodeNIeNT(cp,cp_aux->UnionCP.CONSTANT_Fieldref.class_index,1);
				stringGeral = decodeNIeNT(cp,cp_aux->UnionCP.CONSTANT_Fieldref.name_and_type_index,2);

				ponteiro2pontos = strchr(stringGeral,':');
				*ponteiro2pontos = '\0';

				strcpy(retorno,"<");
				strcat(retorno,stringNomeClasse);
				strcat(retorno,".");
				strcat(retorno,stringGeral);
				strcat(retorno,">");
				break;

			case CONSTANT_String:

				stringGeral = decodeNIeNT(cp,cp_aux->UnionCP.CONSTANT_String.string_index,3);

				strcpy(retorno,"<");
				strcat(retorno,stringGeral);
				strcat(retorno,">");
				break;
			default:
				strcpy(retorno,"undefined");
				break;
		}
	} else {
		sprintf(retorno,"%d",index);
	}
	return(retorno);
}


char* decodeAccessFlags(byte2 flag) {
	char *retorno = malloc(256 * sizeof(char));
	if (!retorno) return NULL;
	strcpy(retorno, "");

	if (flag & ACC_PUBLIC)       strcat(retorno, "PUBLIC; ");
	if (flag & ACC_PRIVATE)      strcat(retorno, "PRIVATE; ");
	if (flag & ACC_PROTECTED)    strcat(retorno, "PROTECTED; ");
	if (flag & ACC_STATIC)       strcat(retorno, "STATIC; ");
	if (flag & ACC_FINAL)        strcat(retorno, "FINAL; ");
	if (flag & ACC_SYNCHRONIZED) strcat(retorno, "SYNCHRONIZED; ");
	if (flag & ACC_VOLATILE)     strcat(retorno, "VOLATILE; ");
	if (flag & ACC_TRANSIENT)    strcat(retorno, "TRANSIENT; ");
	if (flag & ACC_ABSTRACT)     strcat(retorno, "ABSTRACT; ");
	if (flag & ACC_STRICT)       strcat(retorno, "STRICTFP; ");
	if (flag & ACC_SYNTHETIC)    strcat(retorno, "SYNTHETIC; ");
	if (flag & ACC_ENUM)         strcat(retorno, "ENUM; ");
	if (flag & ACC_BRIDGE)       strcat(retorno, "BRIDGE; ");
	if (flag & ACC_VARARGS)      strcat(retorno, "VARARGS; ");

	return retorno;
}


char* organizingFlags(char* flagsOrdemInversa) {
	if (!flagsOrdemInversa) return strdup("");

	char *token = strtok(flagsOrdemInversa, ";");
	char *result = strdup("");
	char *temp;

	while (token) {
		temp = malloc(strlen(result) + strlen(token) + 2); // espaço + '\0'
		if (!temp) {
			free(result);
			return NULL;
		}
		sprintf(temp, "%s %s", token, result);
		free(result);
		result = temp;
		token = strtok(NULL, ";");
	}

	return result;
}

double decodeDoubleInfo(cp_info *cp) {
	uint64_t bits = ((uint64_t)cp->UnionCP.CONSTANT_Double.high_bytes << 32) |
					 cp->UnionCP.CONSTANT_Double.low_bytes;
	double value;
	memcpy(&value, &bits, sizeof(double));
	return value;
}


uint64_t decodeLongInfo (cp_info * cp) {
	return ((((uint64_t)cp->UnionCP.CONSTANT_Long.high_bytes)<<32) | ((uint64_t)cp->UnionCP.CONSTANT_Long.low_bytes));
}

int decodeIntegerInfo (cp_info * cp) {

	byte4 valor = cp->UnionCP.CONSTANT_Integer.bytes;
	return valor;
}

float decodeFloatInfo(cp_info *cp) {
	byte4 valor = cp->UnionCP.CONSTANT_Float.bytes;
	int sinal = ((valor >> 31) == 0) ? 1 : -1;
	int expon = ((valor >> 23) & 0xff);
	int mant = (expon == 0) ? (valor & 0x7fffff) << 1 : (valor & 0x7fffff) | 0x800000;

	return sinal * mant * pow(2, expon - 150);
}



char* decodeNIeNT(cp_info *cp, byte2 index, byte1 tipo) {
	cp_info *aux = cp + index - 1;
	cp_info *aux2, *aux3;

	switch (tipo) {
		case 1:
			aux2 = cp + aux->UnionCP.CONSTANT_Class.name_index - 1;
			return decodeStringUTF8(aux2);

		case 2:
			aux2 = cp + aux->UnionCP.CONSTANT_NameAndType.name_index - 1;
			aux3 = cp + aux->UnionCP.CONSTANT_NameAndType.descriptor_index - 1;

			char *nome = decodeStringUTF8(aux2);
			char *desc = decodeStringUTF8(aux3);
			char *retorno = malloc(strlen(nome) + strlen(desc) + 2); // +1 for ':' +1 for '\0'

			if (!retorno) return NULL;
			sprintf(retorno, "%s:%s", nome, desc);

			free(nome);
			free(desc);
			return retorno;

		case 3: case 4: case 5: case 6:
			return decodeStringUTF8(aux);
	}

	return strdup("undefined");
}



char* decodeStringUTF8(cp_info *cp) {
	if (cp == NULL || cp->UnionCP.CONSTANT_UTF8.bytes == NULL || cp->UnionCP.CONSTANT_UTF8.length == 0) {
		return strdup(""); // retorna string vazia segura
	}

	char* retorno = malloc((cp->UnionCP.CONSTANT_UTF8.length + 1) * sizeof(char));
	if (!retorno) return NULL;

	for (byte1 i = 0; i < cp->UnionCP.CONSTANT_UTF8.length; i++) {
		retorno[i] = (char) cp->UnionCP.CONSTANT_UTF8.bytes[i];
	}
	retorno[cp->UnionCP.CONSTANT_UTF8.length] = '\0';

	return retorno;
}