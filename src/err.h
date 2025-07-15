#ifndef err_h
#define err_h

#include "parse.h"

void compileErrFromParse(Parser *parser, char *message);
void compileErrFromTokenize(Lexer *lexer, char *message);

#endif