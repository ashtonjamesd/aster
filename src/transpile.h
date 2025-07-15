#ifndef transpile_h
#define transpiler_h

#include "parse.h"

typedef struct {
    Ast ast;
} AsterCTranspiler;

AsterCTranspiler newTranspiler(Ast ast);
void transpile(AsterCTranspiler *transpiler);

#endif