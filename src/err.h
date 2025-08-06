#ifndef err_h
#define err_h

#include "parse.h"
#include "analyze.h"

void compileErrFromParse(Parser *parser, char *message);
void compileWarningFromParse(Parser *parser, char *message);

void compileErrFromTokenize(Lexer *lexer, char *message);
void compileErrFromAnalyzer(Analyzer *analyzer, const char *format, ...);

void exitWithInternalCompilerError(char *err);

#endif