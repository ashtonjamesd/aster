#include <stdio.h>
#include <stdlib.h>

#include "compile.h"
#include "tokenize.h"
#include "parse.h"
#include "analyze.h"
#include "transpile.h"

Compiler newCompiler(char *source, CompilerConfig config) {
    Compiler compiler;
    compiler.source = source;
    compiler.config = config;

    return compiler;
}

ExecResult compile(Compiler *compiler) {
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

    AsterCTranspiler aster = newTranspiler(parser.ast);
    transpile(&aster);

    freeParser(&parser);
    freeLexer(&lexer);
    
    return EXEC_OK;
}