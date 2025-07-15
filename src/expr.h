#ifndef expr_h
#define expr_h

typedef struct AstExpr AstExpr;

typedef enum {
    AST_INTEGER_LITERAL,
    AST_FLOAT_LITERAL,
    AST_IDENTIFIER,
    AST_LET,
    AST_TYPE_EXPR,
    AST_ASSIGN_EXPR,
    AST_FUNCTION_DECLARATION,
    AST_BLOCK,
    AST_RETURN,
    
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
    char   *name;
    uint8_t ptrDepth; 
} TypeExpr;

typedef struct {
    char    *name;
    AstExpr *value;
    TypeExpr type;
} LetDeclaration;

typedef struct {
    char    *name;
    AstExpr *value;
} AssignmentExpr;

typedef struct {
    AstExpr **body;
    int       count;
    int       capacity;
} BlockExpr;

typedef struct {
    char     *name;
    TypeExpr  returnType;
    BlockExpr block;
} FunctionDeclaration;

typedef struct {
    AstExpr *value;
} ReturnStatement;

typedef struct {
    char dummy;
} ErrorExpr;

struct AstExpr {
    AstType type;

    union {
        IntegerLiteralExpr  asInteger;
        FloatLiteralExpr    asFloat;
        ErrorExpr           asErr;
        IdentifierExpr      asIdentifer;
        LetDeclaration      asLet;
        TypeExpr            asType;
        AssignmentExpr      asAssign;
        FunctionDeclaration asFunction;
        BlockExpr           asBlock;
        ReturnStatement     asReturn;
    };
};

AstExpr *newIntegerExpr(int value);
AstExpr *newFloatExpr(float value);
AstExpr *newIdentifierExpr(char *name);

AstExpr *newLetDeclaration(char *name, AstExpr *type, AstExpr *value);
AstExpr *newTypeExpr(char *name, uint8_t ptrDepth);
AstExpr *newAssignExpr(char *name, AstExpr *value);
AstExpr *newFunctionDeclaration(char *name, AstExpr *returnType, AstExpr *body);
AstExpr *newBlockExpr(AstExpr **body, int count, int capacity);
AstExpr *newReturnStatement(AstExpr *expr);

AstExpr *newErrExpr();

#endif