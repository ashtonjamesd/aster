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

    expr->asIdentifier.name = strdup(name);

    return expr;
}

AstExpr *newStringExpr(char *str) {
    AstExpr *expr = newExpr(AST_STRING_LITERAL);

    expr->asString.value = strdup(str);

    return expr;
}

AstExpr *newBoolExpr(bool value) {
    AstExpr *expr = newExpr(AST_BOOL_LITERAL);

    expr->asBool.value = value;

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

AstExpr *newAssignExpr(char *name, AstExpr *value, uint8_t ptrDepth) {
    AstExpr *expr = newExpr(AST_ASSIGN_EXPR);

    expr->asAssign.name = strdup(name);
    expr->asAssign.value = value;
    expr->asAssign.ptrDepth = ptrDepth;

    return expr;
}

AstExpr *newFunctionDeclaration(char *name, TypeExpr returnType, BlockExpr body, int paramCount, int paramCapacity, FunctionParameter *parameters, bool isLambda, AstExpr *lambdaExpr, bool isPublic) {
    AstExpr *expr = newExpr(AST_FUNCTION_DECLARATION);

    expr->asFunction.name = strdup(name);
    expr->asFunction.returnType = returnType;
    expr->asFunction.block = body;
    expr->asFunction.paramCapacity = paramCapacity;
    expr->asFunction.paramCount = paramCount;
    expr->asFunction.parameters = parameters;
    expr->asFunction.isLambda = isLambda;
    expr->asFunction.lambdaExpr = lambdaExpr;
    expr->asFunction.isPublic = isPublic;

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

AstExpr *newStructDeclaration(char *name, AstExpr **members, int memberCount, int memberCapacity, bool isInterface, bool isPublic) {
    AstExpr *expr = newExpr(AST_STRUCT_DECLARATION);
    expr->asStruct.name = strdup(name);

    expr->asStruct.members = members;
    expr->asStruct.memberCapacity = memberCapacity;
    expr->asStruct.memberCount = memberCount;
    expr->asStruct.isInterface = isInterface;
    expr->asStruct.isPublic = isPublic;

    return expr;
}

AstExpr *newStructField(char *name, AstExpr *type) {
    AstExpr *expr = newExpr(AST_STRUCT_FIELD);

    expr->asStructField.name = strdup(name);
    expr->asStructField.type = type->asType;

    return expr;
}

AstExpr *newWhileStatement(AstExpr *condition, AstExpr *block, AstExpr *alteration) {
    AstExpr *expr = newExpr(AST_WHILE);
    
    expr->asWhile.block = block->asBlock;
    expr->asWhile.condition = condition;
    expr->asWhile.alteration = alteration;

    return expr;
}

AstExpr *newNextStatement() {
    AstExpr *expr = newExpr(AST_NEXT);

    return expr;
}

AstExpr *newStopStatement() {
    AstExpr *expr = newExpr(AST_STOP);

    return expr;
}

AstExpr *newUnaryExpr(AstExpr *right, OperatorType operator) {
    AstExpr *expr = newExpr(AST_UNARY);

    expr->asUnary.right = right;
    expr->asUnary.operator = operator;

    return expr;
}

AstExpr *newCallExpr(char *name, int argCount, int argCapacity, AstExpr **arguments) {
    AstExpr *expr = newExpr(AST_CALL_EXPR);

    expr->asCallExpr.name = strdup(name);
    expr->asCallExpr.argCount = argCount;
    expr->asCallExpr.argCapacity = argCapacity;
    expr->asCallExpr.arguments = arguments;

    return expr;
}

AstExpr *newBinaryExpr(AstExpr *right, OperatorType operator, AstExpr *left) {
    AstExpr *expr = newExpr(AST_BINARY);

    expr->asBinary.right = right;
    expr->asBinary.operator = operator;
    expr->asBinary.left = left;

    return expr;
}

AstExpr *newTernaryExpr(AstExpr *condition, AstExpr *falseExpr, AstExpr *trueExpr) {
    AstExpr *expr = newExpr(AST_TERNARY);

    expr->asTernary.condition = condition;
    expr->asTernary.trueExpr = trueExpr;
    expr->asTernary.falseExpr = falseExpr;

    return expr;
}

AstExpr *newForStatement(char *variable, AstExpr *iterator, BlockExpr block) {
    AstExpr *expr = newExpr(AST_FOR);
    
    expr->asFor.block = block;
    expr->asFor.variable = strdup(variable);
    expr->asFor.iterator = iterator;

    return expr;
}

AstExpr *newIfStatement(AstExpr *condition, BlockExpr block) {
    AstExpr *expr = newExpr(AST_IF);
    
    expr->asIf.block = block;
    expr->asIf.condition = condition;

    return expr;
}

AstExpr *newMatchExpr(AstExpr *expression, MatchCaseExpr *cases, int caseCount, int caseCapacity) {
    AstExpr *expr = newExpr(AST_MATCH);

    expr->asMatch.expression = expression;
    expr->asMatch.cases = cases;
    expr->asMatch.caseCapacity = caseCapacity;
    expr->asMatch.caseCount = caseCount;

    return expr;
}

AstExpr *newMatchCaseExpr(AstExpr *pattern, BlockExpr block) {
    AstExpr *expr = newExpr(AST_MATCH_CASE);

    expr->asMatchCase.pattern = pattern;
    expr->asMatchCase.block = block;

    return expr;
}

AstExpr *newEnumDeclaration(char *name, char **values, int valueCount, int valueCapacity, bool isPublic) {
    AstExpr *expr = newExpr(AST_ENUM);

    expr->asEnum.name = strdup(name);
    expr->asEnum.values = values;
    expr->asEnum.valueCount = valueCount;
    expr->asEnum.valueCapacity = valueCapacity;
    expr->asEnum.isPublic = isPublic;

    return expr;
}

AstExpr *newErrExpr() {
    AstExpr *expr = newExpr(AST_ERR_EXPR);

    expr->asErr.dummy = 0;

    return expr;
}