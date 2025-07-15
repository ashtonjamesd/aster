#ifndef expr_h
#define expr_h

typedef struct AstExpr AstExpr;

typedef enum {
    AST_INTEGER_LITERAL,
    AST_FLOAT_LITERAL,
    AST_IDENTIFIER,
    AST_LET,
    
    AST_ERR_EXPR,
} AstType;

typedef struct {
    int value;
} IntegerLiteralExpr;

typedef struct {
    float value;
} FloatLiteralExpr;

typedef struct {
    char *name;
} IdentifierExpr;

typedef struct {
    IdentifierExpr name;
    AstExpr       *value;
} LetDeclaration;

typedef struct {
    char dummy;
} ErrorExpr;

struct AstExpr {
    AstType type;

    union {
        IntegerLiteralExpr asInteger;
        FloatLiteralExpr   asFloat;
        ErrorExpr          asErr;
        IdentifierExpr     asIdentifer;
        LetDeclaration     asLet;
    };
};

AstExpr *newIntegerExpr(int value);
AstExpr *newFloatExpr(float value);
AstExpr *newIdentifierExpr(char *name);

AstExpr *newLetDeclaration(AstExpr *name, AstExpr *value);

AstExpr *newErrExpr();

#endif