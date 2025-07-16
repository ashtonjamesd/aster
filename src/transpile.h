#ifndef transpile_h
#define transpiler_h

#include "parse.h"

typedef struct {
    Ast   ast;
    FILE *fptr;
} Transpiler;

Transpiler newTranspiler(FILE *fptr, Ast ast);
void transpile(Transpiler *transpiler);

#endif