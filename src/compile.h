#ifndef compile_h
#define compile_h

#include <stdbool.h>

#include "cli.h"

typedef struct {
    bool  lexerDebug;
    bool  parserDebug;
    char *path;
} AsterConfig;

typedef struct {
    char          *source;
    AsterConfig config;
} AsterCompiler;

AsterCompiler newCompiler(char *source, AsterConfig config);
ExecResult compileToC(AsterCompiler *compiler);

#endif