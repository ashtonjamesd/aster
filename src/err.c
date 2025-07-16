#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "err.h"
#include "parse.h"
#include "tokenize.h"
#include "analyze.h"

void compileErrFromParse(Parser *parser, char *message) {
    parser->hadErr = true;
    
    Token errToken = parser->tokens[parser->position];

    if (parser->position >= parser->tokenCount) {
        errToken = parser->tokens[parser->position - 1];
    }

    fprintf(stderr, "\nerror at %d:%d in %s\n", errToken.line, errToken.column, parser->filePath);
    fprintf(stderr, "%s\n", message);

    int startIndex = 0;
    int endIndex = 0;

    for (int i = 0; i < parser->tokenCount; i++) {
        if (parser->tokens[i].line == errToken.line) {
            startIndex = i;

            while (parser->tokens[i++].line == errToken.line);

            endIndex = i - 1;
            break;
        }
    }

    printf("  %d|  ", errToken.line);
    for (int i = startIndex; i < endIndex; i++) {
        if (parser->tokens[i].type == TOKEN_EOF) break;

        if (parser->tokens[i].hadLeadingWhitespace) printf(" ");
        printf("%s", parser->tokens[i].lexeme);
    }
    printf("\n\n");
}

void compileErrFromTokenize(Lexer *lexer, char *message) {
    lexer->hadErr = true;

    fprintf(stderr, "\nerror at %d:%d in %s\n", lexer->line, lexer->column, lexer->filePath);
    fprintf(stderr, "%s\n", message);

    int start = lexer->position;
    while (start > 0 && lexer->source[start - 1] != '\n') {
        start--;
    }

    int end = lexer->position;
    while (lexer->source[end] != '\0' && lexer->source[end] != '\n') {
        end++;
    }

    printf("  %d|  ", lexer->line);
    for (int i = start; i < end; i++) {
        putchar(lexer->source[i]);
    }
    printf("\n\n");
}

void compileErrFromAnalyzer(Analyzer *analyzer, char *message) {
    analyzer->hadErr = true;

    printf("\n");
    printf("in %s\n", analyzer->parser->filePath);
    printf("%s", message);
    printf("\n");
}

void exitWithInternalCompilerError(char *err) {
    fprintf(stderr, "\ninternal compiler error: %s\n\n", err);
    exit(1);
}