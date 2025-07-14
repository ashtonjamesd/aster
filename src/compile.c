#include <stdio.h>
#include <stdlib.h>

#include "compile.h"
#include "tokenize.h"

Compiler newCompiler(char *source) {
    Compiler compiler;
    compiler.source = source;

    return compiler;
}

ExecResult compile(Compiler *compiler) {
    Lexer lexer = newLexer(compiler->source);
    tokenize(&lexer);

    if (lexer.hadErr) {
        freeLexer(&lexer);
        return EXEC_COMPILE_ERR;
    }

    freeLexer(&lexer);
    free(compiler->source);

    // ..

    return EXEC_OK;
}