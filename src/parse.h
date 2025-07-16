#ifndef parse_h
#define parse_h

#include "tokenize.h"
#include "expr.h"

typedef struct {
    AstExpr **exprs;
    int       exprCount;
    int       exprCapacity;
} Ast;

typedef struct {
    char  *filePath;

    Token *tokens;
    int    tokenCount;

    int    position;
    Ast    ast;

    bool   hadErr;
    bool   debug;
} Parser;

Parser newParser(char *filePath, Token *tokens, int tokenCount, bool debug);
void freeParser(Parser *parser);

void parse(Parser *parser);

#endif