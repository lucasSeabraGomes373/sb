#ifndef JAVA_FRONTEND_H
#define JAVA_FRONTEND_H

#include "formatoClassFile.h"

/*
 * Front-end Java (subconjunto)
 * ---------------------------
 * Fornece uma função que converte um arquivo `.java` (apenas um subconjunto
 * da linguagem) em uma estrutura `ClassFile` em memória para testes e exemplos.
 * Retorna NULL se ocorrer erro durante parsing/compilação.
 */
ClassFile* compile_java_to_classfile(const char *java_path);

#endif
