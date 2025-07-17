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
    TOKEN_WHILE,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_PUB,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_NOT,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_XOR,
    TOKEN_MOD,
    TOKEN_SIZEOF,
    TOKEN_AS,
    TOKEN_MATCH,
    TOKEN_ENUM,

    TOKEN_SINGLE_EQUALS,
    TOKEN_COLON,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,

    TOKEN_STAR,
    TOKEN_AMPERSAND,
    TOKEN_PIPE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_SLASH,
    TOKEN_MODULO,
    TOKEN_LAMBDA,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_THAN_EQUALS,
    TOKEN_LESS_THAN_EQUALS,
    TOKEN_DOUBLE_EQUALS,
    TOKEN_NOT_EQUALS,
    TOKEN_TILDE,
    TOKEN_CARET,
    TOKEN_SHIFT_LEFT,
    TOKEN_SHIFT_RIGHT,

    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_FALSE,
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