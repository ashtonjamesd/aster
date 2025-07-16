#include <stdio.h>
#include <stdlib.h>

#include "compile.h"
#include "tokenize.h"
#include "parse.h"
#include "analyze.h"
#include "transpile.h"

AsterCompiler newCompiler(char *source, AsterConfig config) {
    AsterCompiler compiler;
    compiler.source = source;
    compiler.config = config;

    return compiler;
}

ExecResult compileToC(AsterCompiler *compiler) {
    Lexer lexer = newLexer(compiler->config.path, compiler->source, compiler->config.lexerDebug);
    tokenize(&lexer);

    if (lexer.hadErr) {
        freeLexer(&lexer);
        
        return EXEC_COMPILE_ERR;
    }

    free(compiler->source);
    
    Parser parser = newParser(
        compiler->config.path, lexer.tokens, lexer.tokenCount, 
        compiler->config.parserDebug
    );
    parse(&parser);

    if (parser.hadErr) {
        freeParser(&parser);
        freeLexer(&lexer);

        return EXEC_COMPILE_ERR;
    }

    Analyzer analyzer = newAnalyzer(&parser);
    analyze(&analyzer);

    FILE *fptr = fopen("out.c", "w");
    if (!fptr) {
        fprintf(stderr, "unable to open c source output\n");
        return EXEC_FAIL;
    }

    Transpiler transpiler = newTranspiler(fptr, parser.ast);
    transpile(&transpiler);

    freeParser(&parser);
    freeLexer(&lexer);
    
    return EXEC_OK;
}