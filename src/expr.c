#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

AstExpr *newLetDeclaration(char *name, AstExpr *type, AstExpr *value) {
    AstExpr *expr = newExpr(AST_LET);
    expr->asLet.name = strdup(name);
    expr->asLet.value = value;
    expr->asLet.type = type->asType;

    return expr;
}

AstExpr *newTypeExpr(char *name, uint8_t ptrDepth) {
    AstExpr *expr = newExpr(AST_TYPE_EXPR);
    expr->asType.name = strdup(name);
    expr->asType.ptrDepth = ptrDepth;

    return expr;
}

AstExpr *newAssignExpr(char *name, AstExpr *value) {
    AstExpr *expr = newExpr(AST_ASSIGN_EXPR);
    expr->asAssign.name = strdup(name);
    expr->asAssign.value = value;

    return expr;
}

AstExpr *newFunctionDeclaration(char *name, AstExpr *returnType, AstExpr *body) {
    AstExpr *expr = newExpr(AST_FUNCTION_DECLARATION);
    expr->asFunction.name = strdup(name);
    expr->asFunction.returnType = returnType->asType;
    expr->asFunction.block = body->asBlock;

    return expr;
}

AstExpr *newBlockExpr(AstExpr **body, int count, int capacity) {
    AstExpr *expr = newExpr(AST_BLOCK);

    expr->asBlock.body = body;
    expr->asBlock.count = count;
    expr->asBlock.capacity = capacity;

    return expr;
}

AstExpr *newReturnStatement(AstExpr *value) {
    AstExpr *expr = newExpr(AST_RETURN);
    expr->asReturn.value = value;

    return expr;
}

AstExpr *newErrExpr() {
    AstExpr *expr = newExpr(AST_ERR_EXPR);
    expr->asErr.dummy = 0;

    return expr;
}