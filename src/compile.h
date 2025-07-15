#ifndef compile_h
#define compile_h

#include <stdbool.h>

#include "cli.h"

typedef struct {
    bool  lexerDebug;
    bool  parserDebug;
    char *path;
} CompilerConfig;

typedef struct {
    char          *source;
    CompilerConfig config;
} Compiler;

Compiler newCompiler(char *source, CompilerConfig config);
ExecResult compile(Compiler *compiler);

#endif