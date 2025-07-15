#ifndef tokenize_h
#define tokenize_h

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TOKEN_LET,

    TOKEN_SINGLE_EQUALS,

    TOKEN_INTEGER,
    TOKEN_FLOAT,
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
    uint8_t       count;
    uint8_t       capacity;
} KeywordTable;

typedef struct {
    char      *lexeme;
    TokenType  type;
    uint32_t   line;
    uint32_t   column;
} Token;

typedef struct {
    char         *filePath;
    char         *source;
    
    uint32_t      position;
    uint32_t      line;
    uint32_t      column;

    Token        *tokens;
    uint32_t      tokenCount;
    uint32_t      tokenCapacity;

    KeywordTable *table;

    bool          hadErr;
    bool          debug;
} Lexer;

Lexer newLexer(char *filePath, char *source, bool debug);
void  freeLexer(Lexer *lexer);

void  tokenize(Lexer *lexer);

#endif