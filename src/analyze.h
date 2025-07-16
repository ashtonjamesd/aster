#ifndef analyze_h
#define analyze_h

#include "cli.h"
#include "parse.h"

typedef struct {
    bool hadErr;

    Parser *parser;
} Analyzer;


Analyzer newAnalyzer(Parser *parser);
void analyze(Analyzer *analyzer);

#endif