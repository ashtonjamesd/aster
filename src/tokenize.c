#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenize.h"

static void newKeyword(Lexer *lexer, char *name, TokenType type) {
    if (lexer->table->count >= lexer->table->capacity) {
        lexer->table->capacity *= 2;
        lexer->table->data = realloc(lexer->table->data, sizeof(TokenKeyword) * lexer->table->capacity);
    }

    TokenKeyword keyword = {
        .name = name,
        .type = type,
    };

    lexer->table->data[lexer->table->count++] = keyword;
}

static void freeKeywords(KeywordTable *table) {
    free(table->data);
    free(table);
}

static void initKeywords(Lexer *lexer) {
    newKeyword(lexer, "let", TOKEN_LET);
}

Lexer newLexer(char *source) {
    Lexer lexer;
    
    lexer.source = source;
    
    lexer.position = 0;
    lexer.line = 1;
    lexer.column = 0;

    lexer.token_count = 0;
    lexer.token_capacity = 1;
    lexer.tokens = malloc(sizeof(Token) * lexer.token_capacity);

    lexer.table = malloc(sizeof(KeywordTable));
    lexer.table->count = 0;
    lexer.table->capacity = 1;
    lexer.table->data = malloc(sizeof(TokenKeyword));
    initKeywords(&lexer);

    lexer.hadErr = false;

    return lexer;
}

void freeLexer(Lexer *lexer) {
    for (uint32_t i = 0; i < lexer->token_count; i++) {
        free(lexer->tokens[i].lexeme);
    }

    free(lexer->tokens);
}

static bool isEnd(Lexer *lexer) {
    return lexer->position >= (uint32_t)strlen(lexer->source);
}

static void advance(Lexer *lexer) {
    lexer->position++;
    lexer->column++;
}

static char currentChar(Lexer *lexer) {
    return lexer->source[lexer->position];
}

static Token newToken(char *lexeme, TokenType type, Lexer *lexer) {
    Token token;
    token.lexeme = strdup(lexeme);
    token.type = type;
    token.column = lexer->column;
    token.line = lexer->line;

    return token;
}

static inline Token badToken(Lexer *lexer) {
    return newToken("BAD", TOKEN_BAD, lexer);
}

static char *copyLexeme(Lexer *lexer, uint32_t start) {
    uint32_t length = lexer->position - start;
    char *lexeme = malloc(length + 1);
    strncpy(lexeme, lexer->source + start, length);
    lexeme[length] = '\0';

    return lexeme;
}

static Token tokenizeNumber(Lexer *lexer) {
    uint32_t start = lexer->position;
    while (!isEnd(lexer) && isdigit(currentChar(lexer))) {
        advance(lexer);
    }

    char *lexeme = copyLexeme(lexer, start);

    Token token = newToken(lexeme, TOKEN_INTEGER, lexer);
    free(lexeme);

    return token;
}

static Token tokenizeIdentifier(Lexer *lexer) {
    uint32_t start = lexer->position;
    while (!isEnd(lexer) && isalpha(currentChar(lexer))) {
        advance(lexer);
    }

    char *lexeme = copyLexeme(lexer, start);

    TokenType type = TOKEN_IDENTIFIER;
    for (size_t i = 0; i < lexer->table->count; i++) {
        if (strcmp(lexeme, lexer->table->data[i].name) == 0) {
            type = lexer->table->data[i].type;
            break;
        }
    }

    Token token = newToken(lexeme, type, lexer);
    free(lexeme);

    return token;
}

static Token tokenizeSymbol(Lexer *lexer) {
    char c = currentChar(lexer);
    advance(lexer);

    switch (c) {
        case '=': return newToken("=", TOKEN_SINGLE_EQUALS, lexer);
        default:  return badToken(lexer);
    }
}

static Token tokenizeChar(Lexer *lexer) {
    if (isdigit(currentChar(lexer))) return tokenizeNumber(lexer);
    if (isalpha(currentChar(lexer))) return tokenizeIdentifier(lexer);

    return tokenizeSymbol(lexer);
}

static void printTokens(Lexer *lexer) {
    for (uint32_t i = 0; i < lexer->token_count; i++) {
        Token token = lexer->tokens[i];

        printf("%s: %d at %d:%d\n", token.lexeme, token.type, token.line, token.column);
    }
}

static void addToken(Token token, Lexer *lexer) {
    if (lexer->token_count >= lexer->token_capacity) {
        lexer->token_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->token_capacity);
    }

    lexer->tokens[lexer->token_count++] = token;
}

static void skipWhitespace(Lexer *lexer) {
    while (isspace(currentChar(lexer))) {
        if (currentChar(lexer) == '\n') {
            lexer->line++;
            lexer->column = 0;
        }
        advance(lexer);
    }
}

void tokenize(Lexer *lexer) {
    while (!isEnd(lexer)) {
        skipWhitespace(lexer);
        if (isEnd(lexer)) break;

        Token token = tokenizeChar(lexer);
        addToken(token, lexer);
    }

    addToken(newToken("EOF", TOKEN_EOF, lexer), lexer);
    freeKeywords(lexer->table);

    printTokens(lexer);
}