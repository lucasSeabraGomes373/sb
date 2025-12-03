// inits.h - executa e gerencia helpers de inicialização de classes
#ifndef INITS_H
#define INITS_H

#include "leitor.h"

/*
 * run_clinit_and_load_deps
 * ------------------------
 * Assinatura pública para garantir que os inicializadores estáticos
 * (`<clinit>`) da classe passada e de suas dependências sejam executados.
 * O comportamento detalhado está implementado em `inits.c`.
 */
void run_clinit_and_load_deps(ClassFile *classFile, const char *base_dir);

#endif // INITS_H
