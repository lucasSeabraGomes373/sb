//
// Created on 27/10/2025.
//
// Utilizamos como base do PDF "especificação do formato .class"

#ifndef	ESTRUTURACLASSFILE_H
#define ESTRUTURACLASSFILE_H

#include "byteTypes.h"
#include "constantPool.h"
#include "accessFlags.h"
#include "attributeStructs.h"
	
//Estrutura ClassFile
typedef struct{
	byte4 				magic;
	byte2 				minor_version;
	byte2 				major_version;
	byte2 				constant_pool_count;
	cp_info 		*constant_pool;          // [constant_pool_count-1] 
	byte2 				access_flags;
	byte2 				this_class;
	byte2 				super_class;
	byte2 				interfaces_count;
	byte2 				*interfaces;	        // [interfaces_count] 
	byte2 				fields_count;
	field_info 		*fields;		        // [fileds_count] 
	byte2 				methods_count;
	method_info 	*methods;		        // [methods_count] 
	byte2 				attributes_count;
	attribute_info ** attributes;           // [attributes_count] 
}ClassFile;

#endif
