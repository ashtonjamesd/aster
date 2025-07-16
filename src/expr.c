#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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

    expr->asIdentifer.name = strdup(name);

    return expr;
}

AstExpr *newStringExpr(char *str) {
    AstExpr *expr = newExpr(AST_STRING_LITERAL);

    expr->asString.value = strdup(str);

    return expr;
}

AstExpr *newCharExpr(char *chr) {
    AstExpr *expr = newExpr(AST_CHAR_LITERAL);

    expr->asChar.value = strdup(chr);

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

AstExpr *newFunctionDeclaration(char *name, AstExpr *returnType, AstExpr *body, int paramCount, int paramCapacity, FunctionParameter *parameters) {
    AstExpr *expr = newExpr(AST_FUNCTION_DECLARATION);

    expr->asFunction.name = strdup(name);
    expr->asFunction.returnType = returnType->asType;
    expr->asFunction.block = body->asBlock;
    expr->asFunction.paramCapacity = paramCapacity;
    expr->asFunction.paramCount = paramCount;
    expr->asFunction.parameters = parameters;

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

AstExpr *newFunctionParameter(char *name, AstExpr *type) {
    AstExpr *expr = newExpr(AST_FUNCTION_PARAMETER);

    expr->asParameter.name = strdup(name);
    expr->asParameter.type = type->asType;

    return expr;
}

AstExpr *newStructDeclaration(char *name, StructField *fields, int fieldCount, int fieldCapacity, bool isInterface) {
    AstExpr *expr = newExpr(AST_STRUCT_DECLARATION);
    expr->asStruct.name = strdup(name);

    expr->asStruct.fields = fields;
    expr->asStruct.fieldCapacity = fieldCapacity;
    expr->asStruct.fieldCount = fieldCount;
    expr->asStruct.isInterface = isInterface;

    return expr;
}

AstExpr *newStructField(char *name, AstExpr *type) {
    AstExpr *expr = newExpr(AST_STRUCT_FIELD);

    expr->asStructField.name = strdup(name);
    expr->asStructField.type = type->asType;

    return expr;
}

AstExpr *newErrExpr() {
    AstExpr *expr = newExpr(AST_ERR_EXPR);

    expr->asErr.dummy = 0;

    return expr;
}