#ifndef tokenize_h
#define tokenize_h

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TOKEN_LET,
    TOKEN_FN,
    TOKEN_RETURN,
    TOKEN_STRUCT,
    TOKEN_INTERFACE,
    TOKEN_NEXT,
    TOKEN_STOP,

    TOKEN_SINGLE_EQUALS,
    TOKEN_COLON,
    TOKEN_STAR,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_AMPERSAND,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_SLASH,
    TOKEN_MODULO,

    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_WHILE,
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

    bool       hadLeadingWhitespace;
} Token;

typedef struct {
    char         *filePath;
    char         *source;
    uint32_t      sourceLength;

    uint32_t      position;
    uint32_t      line;
    uint32_t      column;

    Token        *tokens;
    uint32_t      tokenCount;
    uint32_t      tokenCapacity;

    KeywordTable *table;

    bool          hadErr;
    bool          debug;

    bool          hadLeadingWhitespace;
} Lexer;

Lexer newLexer(char *filePath, char *source, bool debug);
void  freeLexer(Lexer *lexer);

void  tokenize(Lexer *lexer);

#endif