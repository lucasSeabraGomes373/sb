#ifndef JAVA_FRONTEND_H
#define JAVA_FRONTEND_H

#include "formatoClassFile.h"

// Parse a limited subset of Java and produce an in-memory ClassFile
// Returns NULL on error.
ClassFile* compile_java_to_classfile(const char *java_path);

#endif
