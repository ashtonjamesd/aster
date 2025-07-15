#include <stdlib.h>

#include "expr.h"

static AstExpr *newExpr(AstType type) {
    AstExpr *expr = malloc(sizeof(AstExpr));
    expr->type = type;

    return expr;
}

AstExpr *newIntegerExpr(int value) {
    AstExpr *expr = newExpr(AST_INTEGER_LITERAL);
    expr->asInteger.value = value;

    return expr;
}

AstExpr *newFloatExpr(float value) {
    AstExpr *expr = newExpr(AST_FLOAT_LITERAL);
    expr->asFloat.value = value;

    return expr;
}

AstExpr *newIdentifierExpr(char *name) {
    AstExpr *expr = newExpr(AST_IDENTIFIER);
    expr->asIdentifer.name = name;

    return expr;
}

AstExpr *newLetDeclaration(AstExpr *name, AstExpr *value) {
    AstExpr *expr = newExpr(AST_LET);
    expr->asLet.name = name->asIdentifer;
    expr->asLet.value = value;

    return expr;
}

AstExpr *newErrExpr() {
    AstExpr *expr = newExpr(AST_ERR_EXPR);
    expr->asErr.dummy = 0;

    return expr;
}