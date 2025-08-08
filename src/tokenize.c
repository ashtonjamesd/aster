#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "tokenize.h"
#include "err.h"

#define MAX_KEYWORDS UCHAR_MAX

static void newKeyword(Lexer *l, char *name, TokenType type) {
    if (l->table->count == MAX_KEYWORDS) {
        exitWithInternalCompilerError("keywords limit reached");
    }

    if (l->table->count >= l->table->capacity) {
        l->table->capacity *= 2;
        l->table->data = realloc(l->table->data, sizeof(TokenKeyword) * l->table->capacity);
        
        if (!l->table->data) {
            exitWithInternalCompilerError("failed to realloc keyword table");
        }
    }

    TokenKeyword keyword = {
        .name = name,
        .type = type,
    };

    l->table->data[l->table->count++] = keyword;
}

static void freeKeywords(KeywordTable *table) {
    free(table->data);
    free(table);
}

static void initKeywords(Lexer *l) {
    newKeyword(l, "let", TOKEN_LET);
    newKeyword(l, "fn", TOKEN_FN);
    newKeyword(l, "return", TOKEN_RETURN);
    newKeyword(l, "struct", TOKEN_STRUCT);
    newKeyword(l, "interface", TOKEN_INTERFACE);
    newKeyword(l, "true", TOKEN_TRUE);
    newKeyword(l, "false", TOKEN_FALSE);
    newKeyword(l, "while", TOKEN_WHILE);
    newKeyword(l, "next", TOKEN_NEXT);
    newKeyword(l, "stop", TOKEN_STOP);
    newKeyword(l, "if", TOKEN_IF);
    newKeyword(l, "then", TOKEN_THEN);
    newKeyword(l, "else", TOKEN_ELSE);
    newKeyword(l, "pub", TOKEN_PUB);
    newKeyword(l, "for", TOKEN_FOR);
    newKeyword(l, "in", TOKEN_IN);
    newKeyword(l, "not", TOKEN_NOT);
    newKeyword(l, "and", TOKEN_AND);
    newKeyword(l, "or", TOKEN_OR);
    newKeyword(l, "xor", TOKEN_XOR);
    newKeyword(l, "mod", TOKEN_MOD);
    newKeyword(l, "sizeof", TOKEN_SIZEOF);
    newKeyword(l, "as", TOKEN_AS);
    newKeyword(l, "match", TOKEN_MATCH);
    newKeyword(l, "enum", TOKEN_ENUM);
    newKeyword(l, "const", TOKEN_CONST);
    newKeyword(l, "defer", TOKEN_DEFER);
    newKeyword(l, "embed", TOKEN_EMBED);
}

Lexer newLexer(char *filePath, char *source, bool debug) {
    Lexer l;

    l.filePath = filePath;
    l.source = source;
    
    l.position = 0;
    l.line = 1;
    l.column = 0;

    l.tokenCount = 0;
    l.tokenCapacity = 1;
    l.tokens = malloc(sizeof(Token) * l.tokenCapacity);

    l.table = malloc(sizeof(KeywordTable));
    l.table->count = 0;
    l.table->capacity = 1;
    l.table->data = malloc(sizeof(TokenKeyword));
    initKeywords(&l);

    l.hadErr = false;
    l.debug = debug;
    l.hadLeadingWhitespace = true;

    return l;
}

void freeLexer(Lexer *l) {
    for (uint32_t i = 0; i < l->tokenCount; i++) {
        free(l->tokens[i].lexeme);
    }

    free(l->tokens);
}

static inline bool isEnd(Lexer *l) {
    return l->position >= l->sourceLength;
}

static inline void advance(Lexer *l) {
    l->position++;
    l->column++;
}

static inline char currentChar(Lexer *l) {
    return isEnd(l) ? '\0' : l->source[l->position];
}

static inline bool shouldPeek(Lexer *l, bool (*predicate)(char)) {
    return !isEnd(l) && predicate(currentChar(l));
}

static Token newToken(char *lexeme, TokenType type, Lexer *l) {
    Token token;
    token.lexeme = strdup(lexeme);
    token.type = type;
    token.column = l->column;
    token.line = l->line;
    token.hadLeadingWhitespace = l->hadLeadingWhitespace;

    return token;
}

static inline Token badToken(Lexer *l) {
    return newToken("BAD", TOKEN_BAD, l);
}

static char *copyLexeme(Lexer *l, uint32_t start) {
    uint32_t length = l->position - start;
    char *lexeme = malloc(length + 1);
    strncpy(lexeme, l->source + start, length);
    lexeme[length] = '\0';

    return lexeme;
}

static inline bool tokenizeNumberPredicate(char c) {
    return isdigit(c) || c == '.';
}

static Token tokenizeNumber(Lexer *l) {
    uint32_t start = l->position;

    bool hasDecimal = false;
    while (shouldPeek(l, tokenizeNumberPredicate)) {
        if (!hasDecimal && currentChar(l) == '.') {
            hasDecimal = true;
        } else if (currentChar(l) == '.') {
            compileErrFromTokenize(l, "invalid numeric format");
            return badToken(l);
        }

        advance(l);
    }

    char *lexeme = copyLexeme(l, start);
    
    TokenType type = hasDecimal ? TOKEN_FLOAT : TOKEN_INTEGER;
    Token token = newToken(lexeme, type, l);
    free(lexeme);

    return token;
}

static inline bool identifierPredicate(char c) {
    return isalpha(c) || isdigit(c) || c == '_';
}

static Token tokenizeIdentifier(Lexer *l) {
    uint32_t start = l->position;
    while (shouldPeek(l, identifierPredicate)) {
        advance(l);
    }

    char *lexeme = copyLexeme(l, start);

    TokenType type = TOKEN_IDENTIFIER;
    for (uint8_t i = 0; i < l->table->count; i++) {
        if (strcmp(lexeme, l->table->data[i].name) == 0) {
            type = l->table->data[i].type;
            break;
        }
    }

    Token token = newToken(lexeme, type, l);
    free(lexeme);

    return token;
}

static Token tokenizeSymbol(Lexer *l) {
    char c = currentChar(l);
    advance(l);

    switch (c) {
        case '=': {
            if (currentChar(l) == '>') {
                advance(l);
                return newToken("=>", TOKEN_LAMBDA, l);
            } else if (currentChar(l) == '=') {
                advance(l);
                return newToken("==", TOKEN_DOUBLE_EQUALS, l);
            }

            return newToken("=", TOKEN_SINGLE_EQUALS, l);
        }
        case '!': {
            if (currentChar(l) == '=') {
                advance(l);
                return newToken("!=", TOKEN_NOT_EQUALS, l);
            }

            return newToken("!", TOKEN_NOT, l);
        }
        case ':': return newToken(":", TOKEN_COLON, l);
        case '*': return newToken("*", TOKEN_STAR, l);
        case '(': return newToken("(", TOKEN_LEFT_PAREN, l);
        case ')': return newToken(")", TOKEN_RIGHT_PAREN, l);
        case '{': return newToken("{", TOKEN_LEFT_BRACE, l);
        case '}': return newToken("}", TOKEN_RIGHT_BRACE, l);
        case ',': return newToken(",", TOKEN_COMMA, l);
        case ';': return newToken(";", TOKEN_SEMICOLON, l);
        case '@': return newToken("@", TOKEN_AT, l);
        case '\"': return newToken("\"", TOKEN_DOUBLE_QUOTE, l);
        case '|': {
            if (currentChar(l) == '|') {
                advance(l);
                return newToken("||", TOKEN_OR, l);
            }

            return newToken("|", TOKEN_PIPE, l);
        }
        case '&': {
            if (currentChar(l) == '&') {
                advance(l);
                return newToken("!=", TOKEN_AND, l);
            }

            return newToken("&", TOKEN_AMPERSAND, l);
        }
        case '^': return newToken("^", TOKEN_CARET, l);
        case '~': return newToken("~", TOKEN_TILDE, l);
        case '+': return newToken("+", TOKEN_PLUS, l);
        case '-': return newToken("-", TOKEN_MINUS, l);
        case '/': return newToken("/", TOKEN_SLASH, l);
        case '%': return newToken("%", TOKEN_MODULO, l);
        case '.': return newToken(".", TOKEN_DOT, l);
        case '>': {
            if (currentChar(l) == '=') {
                advance(l);
                return newToken(">=", TOKEN_GREATER_THAN_EQUALS, l);
            } else if (currentChar(l) == '>') {
                advance(l);
                return newToken(">>", TOKEN_SHIFT_RIGHT, l);
            }

            return newToken(">", TOKEN_GREATER_THAN, l);
        }
        case '<': {
            if (currentChar(l) == '=') {
                advance(l);
                return newToken("<=", TOKEN_LESS_THAN_EQUALS, l);
            } else if (currentChar(l) == '<') {
                advance(l);
                return newToken("<<", TOKEN_SHIFT_LEFT, l);
            }

            return newToken("<", TOKEN_LESS_THAN, l);
        }
        default:  return badToken(l);
    }
}

static inline bool tokenizeStringPredicate(char c) {
    return c != '\"';
}

static Token tokenizeString(Lexer *l) {
    advance(l);
    
    int start = l->position;
    while (shouldPeek(l, tokenizeStringPredicate)) {
        advance(l);
    }

    if (isEnd(l)) {
        compileErrFromTokenize(l, "unterminated string literal");
        return badToken(l);
    }

    l->position++;
    char *lexeme = copyLexeme(l, start - 1);
    l->position--;

    advance(l);

    return newToken(lexeme, TOKEN_STRING, l);
}

static inline bool tokenizeCharPredicate(char c) {
    return c != '\'';
}

static Token tokenizeChar(Lexer *l) {
    advance(l);

    int start = l->position;
    while (shouldPeek(l, tokenizeCharPredicate)) {
        advance(l);
    }
    char *lexeme = copyLexeme(l, start);
    advance(l);

    return newToken(lexeme, TOKEN_CHAR, l);
}

static Token tokenizeNext(Lexer *l) {
    char c = currentChar(l);

    if (isdigit(c)) return tokenizeNumber(l);
    else if (isalpha(c) || c == '_') return tokenizeIdentifier(l);
    else if (c == '\"') return tokenizeString(l);
    else if (c == '\'') return tokenizeChar(l);

    return tokenizeSymbol(l);
}

static void printTokens(Lexer *l) {
    for (uint32_t i = 0; i < l->tokenCount; i++) {
        Token token = l->tokens[i];

        printf("%s: %d at %d:%d\n", token.lexeme, token.type, token.line, token.column);
    }
}

static void addToken(Token token, Lexer *l) {
    if (l->tokenCount >= l->tokenCapacity) {
        l->tokenCapacity *= 2;
        l->tokens = realloc(l->tokens, sizeof(Token) * l->tokenCapacity);
        
        if (!l->table->data) {
            exitWithInternalCompilerError("failed to reallocate tokens pointer");
        }
    }

    l->tokens[l->tokenCount++] = token;
}

static void skipWhitespace(Lexer *l) {
    l->hadLeadingWhitespace = false;

    while (true) {
        char c = currentChar(l);

        while (isspace(c)) {
            l->hadLeadingWhitespace = true;
            if (c == '\n') {
                l->line++;
                l->column = 0;
            }
            advance(l);
            c = currentChar(l);
        }

        if (c == '/' && l->position + 1 < l->sourceLength && l->source[l->position + 1] == '/') {
            while (c != '\n' && !isEnd(l)) {
                advance(l);
                c = currentChar(l);
            }
            continue;
        }

        break;
    }
}


void tokenize(Lexer *l) {
    l->sourceLength = strlen(l->source);

    while (!isEnd(l)) {
        skipWhitespace(l);
        if (isEnd(l)) break;

        Token token = tokenizeNext(l);
        addToken(token, l);

        if (token.type == TOKEN_BAD) break;
    }

    addToken(newToken("EOF", TOKEN_EOF, l), l);
    freeKeywords(l->table);

    if (l->debug) printTokens(l);
}