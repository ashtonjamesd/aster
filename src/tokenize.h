#ifndef tokenize_h
#define tokenize_h

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TOKEN_LET,

    TOKEN_SINGLE_EQUALS,

    TOKEN_INTEGER,
    TOKEN_IDENTIFIER,

    TOKEN_EOF,
    TOKEN_BAD,
} TokenType;

typedef struct {
    char     *name;
    TokenType type;
} TokenKeyword;

typedef struct {
    TokenKeyword *data;
    size_t        count;
    size_t        capacity;
} KeywordTable;

typedef struct {
    char      *lexeme;
    TokenType  type;
    uint32_t   line;
    uint32_t   column;
} Token;

typedef struct {
    char         *source;
    
    uint32_t      position;
    uint32_t      line;
    uint32_t      column;

    Token        *tokens;
    uint32_t      token_count;
    uint32_t      token_capacity;

    KeywordTable *table;

    bool          hadErr;
} Lexer;

Lexer newLexer(char *source);
void  freeLexer(Lexer *lexer);

void  tokenize(Lexer *lexer);

#endif