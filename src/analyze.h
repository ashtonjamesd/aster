#ifndef analyze_h
#define analyze_h

#include "cli.h"
#include "parse.h"

typedef struct {
    bool isConstant;
    long value;
} ConstEvalResult;

typedef struct {
    char    *name;
    AstExpr *declaration;
} Symbol;

typedef struct {
    Symbol *symbols;
    int     count;
    int     capacity;
} Scope;

typedef struct {
    Scope *scopes;
    int    depth;
    int    capacity;
} SymbolTable;

typedef struct {
    bool        hadErr;

    bool        insideLoop;

    Parser     *parser;
    SymbolTable table;
} Analyzer;


Analyzer newAnalyzer(Parser *parser);
void freeAnalyzer(Analyzer *analyzer);

void analyze(Analyzer *analyzer);

#endif