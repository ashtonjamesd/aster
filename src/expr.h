#ifndef expr_h
#define expr_h

typedef struct AstExpr AstExpr;

typedef enum {
    AST_INTEGER_LITERAL,
    AST_FLOAT_LITERAL,
    AST_CHAR_LITERAL,
    AST_STRING_LITERAL,
    AST_BOOL_LITERAL,
    AST_IDENTIFIER,
    AST_LET,
    AST_TYPE_EXPR,
    AST_ASSIGN_EXPR,
    AST_FUNCTION_DECLARATION,
    AST_BLOCK,
    AST_RETURN,
    AST_FUNCTION_PARAMETER,
    AST_STRUCT_DECLARATION,
    AST_STRUCT_FIELD,
    AST_WHILE,
    AST_NEXT,
    AST_STOP,
    AST_UNARY,
    AST_CALL_EXPR,
    AST_BINARY,
    AST_TERNARY,
    
    AST_ERR_EXPR,
} AstType;

typedef enum {
    OP_ADDRESS_OF,
    OP_DEREF,
    OP_PLUS,
    OP_MINUS,
    OP_LESS_THAN,
    OP_GREATER_THAN,
} OperatorType;

typedef struct {
    long value;
} IntegerLiteralExpr;

typedef struct {
    float value;
} FloatLiteralExpr;

typedef struct {
    char *value;
} CharLiteralExpr;

typedef struct {
    char *value;
} StringLiteralExpr;

typedef struct {
    char *name;
} IdentifierExpr;

typedef struct {
    bool value;
} BoolLiteralExpr;

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
    uint8_t  ptrDepth;
} AssignmentExpr;

typedef struct {
    OperatorType operator;
    AstExpr     *right;
} UnaryExpr;

typedef struct {
    AstExpr     *left;
    OperatorType operator;
    AstExpr     *right;
} BinaryExpr;

typedef struct {
    AstExpr **body;
    int       count;
    int       capacity;
} BlockExpr;

typedef struct {
    char    *name;
    TypeExpr type;
} FunctionParameter;

typedef struct {
    char     *name;

    int       argCount;
    int       argCapacity;
    AstExpr **arguments;
} CallExpr;

typedef struct {
    char              *name;
    TypeExpr           returnType;
    BlockExpr          block;

    int                paramCount;
    int                paramCapacity;
    FunctionParameter *parameters;

    bool               isLambda;
    AstExpr           *lambdaExpr;
} FunctionDeclaration;

typedef struct {
    char    *name;
    TypeExpr type;
} StructField;

typedef struct {
    char     *name;
    bool      isInterface;

    int       memberCount;
    int       memberCapacity;
    AstExpr **members;
} StructDeclaration;

typedef struct {
    AstExpr *value;
} ReturnStatement;

typedef struct {
    char dummy;
} ErrorExpr;

typedef struct {
    AstExpr *condition;
    BlockExpr block;
} WhileStatement;

typedef struct {
    char dummy;
} NextStatement;

typedef struct {
    char dummy;
} StopStatement;

typedef struct {
    AstExpr *condition;
    AstExpr *trueExpr;
    AstExpr *falseExpr;
} TernaryExpression;

struct AstExpr {
    AstType type;

    union {
        IntegerLiteralExpr  asInteger;
        FloatLiteralExpr    asFloat;
        StringLiteralExpr   asString;
        CharLiteralExpr     asChar;
        IdentifierExpr      asIdentifier;
        LetDeclaration      asLet;
        TypeExpr            asType;
        AssignmentExpr      asAssign;
        FunctionDeclaration asFunction;
        BlockExpr           asBlock;
        ReturnStatement     asReturn;
        ErrorExpr           asErr;
        FunctionParameter   asParameter;
        StructDeclaration   asStruct;
        StructField         asStructField;
        BoolLiteralExpr     asBool;
        WhileStatement      asWhile;
        NextStatement       asNext;
        StopStatement       asStop;
        UnaryExpr           asUnary;
        CallExpr            asCallExpr;
        BinaryExpr          asBinary;
        TernaryExpression   asTernary;
    };
};

AstExpr *newIntegerExpr(int value);
AstExpr *newFloatExpr(float value);
AstExpr *newIdentifierExpr(char *name);
AstExpr *newStringExpr(char *name);
AstExpr *newCharExpr(char *name);
AstExpr *newBoolExpr(bool value);

AstExpr *newLetDeclaration(char *name, AstExpr *type, AstExpr *value);
AstExpr *newTypeExpr(char *name, uint8_t ptrDepth);
AstExpr *newAssignExpr(char *name, AstExpr *value, uint8_t ptrDepth);
AstExpr *newFunctionDeclaration(char *name, TypeExpr returnType, BlockExpr body, int paramCount, int paramCapacity, FunctionParameter *parameters, bool isLambda, AstExpr *lambdaExpr);
AstExpr *newBlockExpr(AstExpr **body, int count, int capacity);
AstExpr *newReturnStatement(AstExpr *expr);
AstExpr *newFunctionParameter(char *name, AstExpr *type);
AstExpr *newStructDeclaration(char *name, AstExpr **members, int memberCount, int memberCapacity, bool isInterface);
AstExpr *newStructField(char *name, AstExpr *type);
AstExpr *newWhileStatement(AstExpr *condition, AstExpr *block);
AstExpr *newNextStatement();
AstExpr *newStopStatement();
AstExpr *newUnaryExpr(AstExpr *right, OperatorType operator);
AstExpr *newCallExpr(char *name, int argCount, int argCapacity, AstExpr **arguments);
AstExpr *newBinaryExpr(AstExpr *right, OperatorType operator, AstExpr *left);
AstExpr *newTernaryExpr(AstExpr *condition, AstExpr *falseExpr, AstExpr *trueExpr);

AstExpr *newErrExpr();

#endif