#ifndef compile_h
#define compile_h

#include "cli.h"

typedef struct {
    char *source;
} Compiler;

Compiler newCompiler(char *source);
ExecResult compile(Compiler *compiler);

#endif