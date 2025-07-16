#ifndef err_h
#define err_h

#include "parse.h"
#include "analyze.h"

void compileErrFromParse(Parser *parser, char *message);
void compileErrFromTokenize(Lexer *lexer, char *message);
void compileErrFromAnalyzer(Analyzer *analyzer, char *message);

void exitWithInternalCompilerError(char *err);

#endif