#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "err.h"
#include "expr.h"
#include "map.h"

static AstExpr *parseStatement(Parser *p);
static AstExpr *parseExpr(Parser *p);
static AstExpr *parseCallExpression(Parser *p);


static AstExpr *error(Parser *p, char *err) {
    compileErrFromParse(p, err);
    return newErrExpr();
}

Parser newParser(char *filePath, Token *tokens, int tokenCount, bool debug) {
    Parser p;

    p.filePath = filePath;

    p.position = 0;
    p.tokenCount = tokenCount;
    p.tokens = tokens;

    p.ast = (Ast){
        .exprCapacity = 1,
        .exprCount = 0,
        .exprs = malloc(sizeof(AstExpr *))
    };

    p.hadErr = false;
    p.debug = debug;

    return p;
}

static void freeExpr(AstExpr *expr) {
    if (!expr) return;

    switch (expr->type) {
        case AST_NEXT: {
            break;
        }
        case AST_STOP: {
            break;
        }
        case AST_INTEGER_LITERAL: {
            break;
        }
        case AST_FLOAT_LITERAL: {
            break;
        }
        case AST_ERR_EXPR: {
            break;
        }
        case AST_BOOL_LITERAL: {
            break;
        }
        case AST_TERNARY: {
            freeExpr(expr->asTernary.condition);
            freeExpr(expr->asTernary.trueExpr);
            freeExpr(expr->asTernary.falseExpr);
            break;
        }
        case AST_CALL_EXPR: {
            for (int i = 0; i < expr->asCallExpr.argCount; i++) {
                freeExpr(expr->asCallExpr.arguments[i]);
            }
            free(expr->asCallExpr.name);
            break;
        }
        case AST_IDENTIFIER: {
            free(expr->asIdentifier.name);
            break;
        }
        case AST_STRING_LITERAL: {
            free(expr->asString.value);
            break;
        }
        case AST_CHAR_LITERAL: {
            free(expr->asChar.value);
            break;
        }
        case AST_LET: {
            free(expr->asLet.name);
            freeExpr(expr->asLet.value);
            break;
        }
        case AST_TYPE_EXPR: {
            free(expr->asType.name);
            break;
        }
        case AST_ASSIGN_EXPR: {
            free(expr->asAssign.name);    
            freeExpr(expr->asAssign.value);
            break;
        }
        case AST_UNARY: {
            freeExpr(expr->asUnary.right);
            break;
        }
        case AST_BINARY: {
            freeExpr(expr->asBinary.left);
            freeExpr(expr->asBinary.right);
            break;
        }
        case AST_FUNCTION_DECLARATION: {
            free(expr->asFunction.name);
            free(expr->asFunction.returnType.name);

            if (!expr->asFunction.isLambda) {
                for (int i = 0; i < expr->asFunction.block.count; i++) {
                    freeExpr(expr->asFunction.block.body[i]);
                }
                free(expr->asFunction.block.body);
            } else {
                freeExpr(expr->asFunction.lambdaExpr);
            }

            for (int i = 0; i < expr->asFunction.paramCount; i++) {
                free(expr->asFunction.parameters[i].name);
                free(expr->asFunction.parameters[i].type.name);
            }
            free(expr->asFunction.parameters);
            break;
        }
        case AST_STRUCT_DECLARATION: {
            free(expr->asStruct.name);
            for (int i = 0; i < expr->asStruct.memberCount; i++) {
                freeExpr(expr->asStruct.members[i]);
            }
            break;
        }
        case AST_RETURN: {
            freeExpr(expr->asReturn.value);
            break;
        }
        case AST_WHILE: {
            freeExpr(expr->asWhile.condition);
            for (int i = 0; i < expr->asWhile.block.count; i++) {
                freeExpr(expr->asWhile.block.body[i]);
            }
            break;
        }
        case AST_IF: {
            freeExpr(expr->asIf.condition);
            for (int i = 0; i < expr->asIf.block.count; i++) {
                freeExpr(expr->asIf.block.body[i]);
            }
            break;
        }
        case AST_FOR: {
            freeExpr(expr->asFor.iterator);
            free(expr->asFor.variable);
            
            for (int i = 0; i < expr->asFor.block.count; i++) {
                freeExpr(expr->asFor.block.body[i]);
            }
            break;
        }
        default: {
            exitWithInternalCompilerError("unknown AST expression type in 'freeExpr'");
        }
    }

    free(expr);
}

void freeParser(Parser *p) {
    for (int i = 0; i < p->ast.exprCount; i++) {
        freeExpr(p->ast.exprs[i]);
    }

    free(p->ast.exprs);
}

static inline void printIndent(int indent) {
    for (int i = 0; i < indent * 2; i++) printf(" ");
}

static void printExpr(AstExpr expr, int indent) {
    printIndent(indent);

    switch (expr.type) {
        case AST_INTEGER_LITERAL: {
            printf("integer literal: %ld\n", expr.asInteger.value);
            break;
        }
        case AST_BOOL_LITERAL: {
            printf("bool literal: %s\n", expr.asBool.value ? "true" : "false");
            break;
        }
        case AST_FLOAT_LITERAL: {
            printf("float literal: %f\n", expr.asFloat.value);
            break;
        }
        case AST_STRING_LITERAL: {
            printf("string literal: \"%s\"\n", expr.asString.value);
            break;
        }
        case AST_CHAR_LITERAL: {
            printf("char literal: '%s'\n", expr.asChar.value);
            break;
        }
        case AST_IDENTIFIER: {
            printf("identifier: %s\n", expr.asIdentifier.name);
            break;
        }
        case AST_ERR_EXPR: {
            printf("error expression\n");
            break;
        }
        case AST_NEXT: {
            printf("next statement\n");
            break;
        }
        case AST_STOP: {
            printf("stop statement\n");
            break;
        }
        case AST_UNARY: {
            printf("unary expression\n");

            printIndent(indent + 2);
            printf("operator: %s\n", mapOperatorType(expr.asUnary.operator));

            printIndent(indent + 2);
            printf("right: \n");
            printExpr(*expr.asUnary.right, indent + 4);
            break;
        }
        case AST_BINARY: {
            printf("binary expression\n");

            printIndent(indent + 2);
            printf("left: \n");
            printExpr(*expr.asBinary.left, indent + 4);

            printIndent(indent + 2);
            printf("operator: %s\n", mapOperatorType(expr.asBinary.operator));

            printIndent(indent + 2);
            printf("right: \n");
            printExpr(*expr.asBinary.right, indent + 4);
            break;
        }
        case AST_LET: {
            printf("let declaration:\n");
            
            printIndent(indent + 2);
            printf("name: %s\n", expr.asLet.name);
            
            printIndent(indent + 2);
            printf("type: %s\n", expr.asLet.type.name );

            printIndent(indent + 2);
            printf("ptr depth: %d\n", expr.asLet.type.ptrDepth);
            
            printIndent(indent + 2);
            printf("value:\n");
            printExpr(*expr.asLet.value, indent + 4);
            break;
        }
        case AST_ASSIGN_EXPR: {
            printf("assignment expression:\n");

            printIndent(indent + 2);
            printf("name: %s\n", expr.asAssign.name);

            printIndent(indent + 2);
            printf("value:\n");
            printExpr(*expr.asAssign.value, indent + 4);
            break;
        }
        case AST_IF: {
            printf("if statement:\n");
            
            printIndent(indent + 2);
            printf("condition:\n");
            printExpr(*expr.asIf.condition, indent + 2);

            printIndent(indent + 2);
            printf("body (%d):\n", expr.asIf.block.count);
            for (int i = 0; i < expr.asIf.block.count; i++) {
                printExpr(*expr.asIf.block.body[i], indent + 2);
            }
            break;
        }
        case AST_FUNCTION_DECLARATION: {
            printf("function declaration:\n");

            printIndent(indent + 2);
            printf("name: %s\n", expr.asFunction.name);
            
            printIndent(indent + 2);
            printf("public: %s\n", expr.asFunction.isPublic ? "true" : "false");

            printIndent(indent + 2);
            printf("type: ");
            for (int i = 0; i < expr.asFunction.returnType.ptrDepth; i++) printf("*");
            printf("%s\n", expr.asFunction.returnType.name);

            printIndent(indent + 2);
            printf("parameters (%d):\n", expr.asFunction.paramCount);
            for (int i = 0; i < expr.asFunction.paramCount; i++) {
                FunctionParameter param = expr.asFunction.parameters[i];

                printIndent(indent + 4);
                printf("name: %s\n", param.name);
                printIndent(indent + 4);
                printf("type: ");
                for (int i = 0; i < param.type.ptrDepth; i++) printf("*");
                printf("%s\n", param.type.name);
            }
            
            if (!expr.asFunction.isLambda) {
                printIndent(indent + 2);
                printf("body (%d):\n", expr.asFunction.block.count);
                for (int i = 0; i < expr.asFunction.block.count; i++) {
                    printExpr(*expr.asFunction.block.body[i], indent + 2);
                }
            } else {
                printIndent(indent + 2);
                printf("lambda: ");
                printExpr(*expr.asFunction.lambdaExpr, indent + 2);
            }

            break;
        }
        case AST_WHILE: {
            printf("while statement:\n");

            printIndent(indent + 2);
            printf("value:\n");
            printExpr(*expr.asWhile.condition, indent + 2);

            printIndent(indent + 2);
            printf("body (%d):\n", expr.asWhile.block.count);
            for (int i = 0; i < expr.asWhile.block.count; i++) {
                printExpr(*expr.asWhile.block.body[i], indent + 2);
            }
            break;
        }
        case AST_FOR: {
            printf("for statement:\n");

            printIndent(indent + 2);
            printf("variable: %s\n", expr.asFor.variable);

            printIndent(indent + 2);
            printf("iterator:\n");
            printExpr(*expr.asFor.iterator, indent + 4);

            printIndent(indent + 2);
            printf("body (%d):\n", expr.asFor.block.count);
            for (int i = 0; i < expr.asFor.block.count; i++) {
                printExpr(*expr.asFor.block.body[i], indent + 4);
            }
            break;
        }
        case AST_RETURN: {
            printf("return statement:\n");

            printIndent(indent + 2);
            printf("value:\n");
            printExpr(*expr.asReturn.value, indent + 2);
            break;
        }
        case AST_STRUCT_DECLARATION: {
            printf("struct declaration:\n");

            printIndent(indent + 2);
            printf("name: %s\n", expr.asStruct.name);

            printIndent(indent + 2);
            printf("interface: %s\n", expr.asStruct.isInterface ? "true" : "false");

            printIndent(indent + 2);
            printf("public: %s\n", expr.asStruct.isPublic ? "true" : "false");

            printIndent(indent + 2);
            printf("members (%d):\n", expr.asStruct.memberCount);
            for (int i = 0; i < expr.asStruct.memberCount; i++) {
                printExpr(*expr.asStruct.members[i], indent + 4);
            }
            break;
        }
        case AST_CALL_EXPR: {
            printf("call expression:\n");

            printIndent(indent + 2);
            printf("name: %s\n", expr.asCallExpr.name);

            printIndent(indent + 2);
            printf("arguments (%d):\n", expr.asCallExpr.argCount);
            for (int i = 0; i < expr.asCallExpr.argCount; i++) {
                printExpr(*expr.asCallExpr.arguments[i], indent + 4);
            }
            break;
        }
        case AST_TERNARY: {
            printf("ternary expression:\n");

            printIndent(indent + 2);
            printf("condition:\n");
            printExpr(*expr.asTernary.condition, indent + 4);

            printIndent(indent + 2);
            printf("true expression:\n");
            printExpr(*expr.asTernary.trueExpr, indent + 4);

            printIndent(indent + 2);
            printf("false expression:\n");
            printExpr(*expr.asTernary.falseExpr, indent + 4);
            break;
        }
        default: {
            exitWithInternalCompilerError("unknown ast expression type in 'printExpr'");
        }
    }
}

void printAst(Parser *p) {
    printf("\nAST:\n");
    for (int i = 0; i < p->ast.exprCount; i++) {
        printExpr(*p->ast.exprs[i], 1);
    }
}

static inline Token currentToken(Parser *p) {
    return p->tokens[p->position];
}

static inline bool match(Parser *p, TokenType type) {
    return currentToken(p).type == type;
}

static inline void advance(Parser *p) {
    p->position++;
}

static inline void recede(Parser *p) {
    p->position--;
}

static inline bool isErr(AstExpr *expr) {
    return expr->type == AST_ERR_EXPR;
}

static bool expect(Parser *p, TokenType type) {
    if (match(p, type)) {
        advance(p);
        return true;
    }

    return false;
}

static inline bool isEnd(Parser *p) {
    if (p->position >= p->tokenCount) return true;
    if (currentToken(p).type == TOKEN_EOF) return true;

    return false;
}

static AstExpr *parsePrimary(Parser *p) {
    Token token = currentToken(p);
    advance(p);

    switch (token.type) {
        case TOKEN_INTEGER: {
            return newIntegerExpr(atol(token.lexeme));
        }
        case TOKEN_FLOAT: {
            return newFloatExpr(atof(token.lexeme));
        }
        case TOKEN_IDENTIFIER: {
            if (match(p, TOKEN_LEFT_PAREN)) {
                recede(p);

                return parseCallExpression(p);
            }

            return newIdentifierExpr(token.lexeme);
        }
        case TOKEN_CHAR: {
            return newCharExpr(token.lexeme);
        }
        case TOKEN_STRING: {
            return newStringExpr(token.lexeme);
        }
        case TOKEN_TRUE: {
            return newBoolExpr(token.lexeme[0] == 't');
        }
        case TOKEN_FALSE: {
            return newBoolExpr(token.lexeme[0] == 'f');
        }
        default: {
            compileErrFromParse(p, "expected expression");
            return newErrExpr();
        }
    }
}

static AstExpr *parsePointerOp(Parser *p) {
    while (match(p, TOKEN_STAR) || match(p, TOKEN_AMPERSAND) || match(p, TOKEN_SIZEOF)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseExpr(p);
        if (isErr(right)) return right;

        return newUnaryExpr(right, mapToOperatorType(operator));
    }

    return parsePrimary(p);
}

static AstExpr *parseUnary(Parser *p) {
    while (match(p, TOKEN_MINUS) || match(p, TOKEN_PLUS) || 
        match(p, TOKEN_NOT) || match(p, TOKEN_TILDE)
    ) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parsePointerOp(p);
        if (isErr(right)) return right;

        return newUnaryExpr(right, mapToOperatorType(operator));
    }

    return parsePointerOp(p);
}

static AstExpr *parseAs(Parser *p) {
    AstExpr *left = parseUnary(p);

    while (match(p, TOKEN_AS)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr* right = parseUnary(p);
        if (isErr(right)) return right;

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseFactor(Parser *p) {
    AstExpr *left = parseAs(p);

    while (match(p, TOKEN_STAR) || match(p, TOKEN_SLASH) || match(p, TOKEN_MODULO) || match(p, TOKEN_MOD)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr* right = parseAs(p);
        if (isErr(right)) return right;

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseTerm(Parser *p) {
    AstExpr *left = parseFactor(p);

    while (match(p, TOKEN_PLUS) || match(p, TOKEN_MINUS)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr* right = parseFactor(p);
        if (isErr(right)) return right;

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseBitwiseShifts(Parser *p) {
    AstExpr* left = parseTerm(p);

    while (match(p, TOKEN_SHIFT_LEFT) || match(p, TOKEN_SHIFT_RIGHT)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseTerm(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseComparative(Parser *p) {
    AstExpr *left = parseBitwiseShifts(p);

    while (match(p, TOKEN_LESS_THAN) || match(p, TOKEN_GREATER_THAN) || 
        match(p, TOKEN_LESS_THAN_EQUALS) || match(p, TOKEN_GREATER_THAN_EQUALS)
    ) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseBitwiseShifts(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseRelationalEquality(Parser *p) {
    AstExpr* left = parseComparative(p);

    while (match(p, TOKEN_DOUBLE_EQUALS) || match(p, TOKEN_NOT_EQUALS)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseComparative(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseBitwiseAnd(Parser *p) {
    AstExpr* left = parseRelationalEquality(p);

    while (match(p, TOKEN_AMPERSAND)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseRelationalEquality(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseBitwiseXor(Parser *p) {
    AstExpr* left = parseBitwiseAnd(p);

    while (match(p, TOKEN_CARET) || match(p, TOKEN_XOR)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseBitwiseAnd(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseBitwiseOr(Parser *p) {
    AstExpr* left = parseBitwiseXor(p);

    while (match(p, TOKEN_PIPE)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseBitwiseXor(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseLogicalAnd(Parser *p) {
    AstExpr* left = parseBitwiseOr(p);

    while (match(p, TOKEN_AND)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseBitwiseOr(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseLogicalOr(Parser *p) {
    AstExpr* left = parseLogicalAnd(p);

    while (match(p, TOKEN_OR)) {
        TokenType operator = currentToken(p).type;
        advance(p);

        AstExpr *right = parseLogicalAnd(p);

        left = newBinaryExpr(right, mapToOperatorType(operator), left);
    }

    return left;
}

static AstExpr *parseTernary(Parser *p) {
    if (match(p, TOKEN_IF)) {
        advance(p);

        AstExpr *condition = parseExpr(p);
        if (isErr(condition)) return condition;

        if (!expect(p, TOKEN_THEN)) {
            return error(p, "expected 'then' after ternary condition");
        }

        AstExpr *trueExpr = parseExpr(p);
        if (isErr(trueExpr)) return trueExpr;
        
        if (!expect(p, TOKEN_ELSE)) {
            return error(p, "expected 'else' then ternary false expression");
        }

        AstExpr *falseExpr = parseExpr(p);
        if (isErr(falseExpr)) return falseExpr;

        return newTernaryExpr(condition, falseExpr, trueExpr);
    }

    return parseLogicalOr(p);
}

static AstExpr *parseExpr(Parser *p) {
    return parseTernary(p);
}

static AstExpr *parseType(Parser *p) {
    uint8_t ptrDepth = 0;
    while (match(p, TOKEN_STAR)) {
        advance(p);
        ptrDepth++;

        if (ptrDepth == 9) {
            return error(p, "pointer indirection higher than 8 is not allowed");
        }
    }

    Token name = currentToken(p);
    if (!expect(p, TOKEN_IDENTIFIER)) {
        return error(p, "expected type specifier");
    }

    return newTypeExpr(name.lexeme, ptrDepth);
}

static AstExpr *parseLet(Parser *p) {
    advance(p);

    Token name = currentToken(p);
    if (!expect(p, TOKEN_IDENTIFIER)) {
        return error(p, "expected identifier after 'let'");
    }

    if (!expect(p, TOKEN_COLON)) {
        return error(p, "expected ':' and then a type specifier");
    }

    AstExpr *typeExpr = parseType(p);
    if (typeExpr->type != AST_TYPE_EXPR) {
        return error(p, "expected type after ':'");
    }

    if (!expect(p, TOKEN_SINGLE_EQUALS)) {
        return error(p, "expected '='");
    }

    AstExpr *value = parseExpr(p);
    if (isErr(value)) {
        return error(p, "expected expression");
    }

    return newLetDeclaration(name.lexeme, typeExpr, value);
}

static AstExpr *parseAssignment(Parser *p) {
    int ptrDepth = 0;
    while (match(p, TOKEN_STAR)) {
        ptrDepth++;
        advance(p);
    }

    Token name = currentToken(p);
    advance(p);

    if (!expect(p, TOKEN_SINGLE_EQUALS)) {
        return error(p, "expected '=' after identifier");
    }

    AstExpr *value = parseExpr(p);
    if (isErr(value)) return value;

    return newAssignExpr(name.lexeme, value, ptrDepth);
}

static AstExpr *parseBlock(Parser *p) {
    int count = 0;
    int capacity = 1;

    AstExpr **body = malloc(sizeof(AstExpr *));
    while (!match(p, TOKEN_RIGHT_BRACE)) {
        AstExpr *expr = parseStatement(p);
        if (isErr(expr)) return expr;
        
        if (count >= capacity) {
            capacity *= 2;
            body = realloc(body, sizeof(AstExpr *) * capacity);
        }

        body[count++] = expr;
    }

    return newBlockExpr(body, count, capacity);
}

static AstExpr *parseFunction(Parser *p) {
    bool isPublic = false;

    if (match(p, TOKEN_PUB)) {
        isPublic = true;
        advance(p);
    }

    advance(p);

    Token name = currentToken(p);
    if (!expect(p, TOKEN_IDENTIFIER)) {
        return error(p, "expected identifier after 'fn'");
    }

    if (!expect(p, TOKEN_LEFT_PAREN)) {
        return error(p, "expected '('");
    }

    FunctionParameter *parameters = malloc(sizeof(FunctionParameter *));
    int paramCount = 0;
    int paramCapacity = 1;  

    if (!match(p, TOKEN_RIGHT_PAREN)) {
        recede(p);
        
        do {
            advance(p);

            Token name = currentToken(p);
            if (!expect(p, TOKEN_IDENTIFIER)) {
                return error(p, "expected identifier");
            }

            if (!expect(p, TOKEN_COLON)) {
                return error(p, "expected ':' and then a type declaration");
            }

            AstExpr *type = parseType(p);
            if (isErr(type)) return type;

            AstExpr *parameter = newFunctionParameter(name.lexeme, type);

            if (paramCount >= paramCapacity) {
                paramCapacity *= 2;
                parameters = realloc(parameters, sizeof(FunctionParameter) * paramCapacity);
            }
            parameters[paramCount++] = parameter->asParameter;

        } while (match(p, TOKEN_COMMA));
    }

    if (!expect(p, TOKEN_RIGHT_PAREN)) {
        return error(p, "expected ')'");
    }

    if (!expect(p, TOKEN_COLON)) {
        return error(p, "function return types must be specified, expected ':' and then a type specifier after ')'");
    }

    AstExpr *returnType = parseType(p);
    if (returnType->type != AST_TYPE_EXPR) return returnType;

    AstExpr *lambdaExpr = NULL;
    bool isLambda = false;

    if (match(p, TOKEN_LAMBDA)) {
        advance(p);

        lambdaExpr = parseExpr(p);
        if (isErr(lambdaExpr)) return lambdaExpr;

        isLambda = true;
    }

    if (!isLambda && !expect(p, TOKEN_LEFT_BRACE)) {
        return error(p, "expected '{'");
    }

    AstExpr *body = NULL;
    if (!isLambda) {
        body = parseBlock(p);
        if (isErr(body)) return body;

        if (!expect(p, TOKEN_RIGHT_BRACE)) {
            return error(p, "expected '}'");
        }
    }
    
    BlockExpr block = body ? body->asBlock : (BlockExpr){ .body = NULL, .count = 0 };

    AstExpr* functionDeclaration = newFunctionDeclaration(
        name.lexeme, returnType->asType, block, paramCount, paramCapacity, parameters, isLambda, lambdaExpr, isPublic
    );

    free(returnType);
    if (body) free(body);

    return functionDeclaration;
}

static AstExpr *parseReturn(Parser *p) {
    advance(p);

    AstExpr *value = parseExpr(p);
    if (isErr(value)) return value;

    return newReturnStatement(value);
}

static AstExpr *parseStruct(Parser *p) {
    bool isPublic = false;
    bool isInterface = false;

    if (match(p, TOKEN_PUB)) {
        advance(p);
        isPublic = true;
    }

    if (match(p, TOKEN_INTERFACE)) {
        advance(p);
        isInterface = true;
    }
    advance(p);

    Token name = currentToken(p);
    if (!expect(p, TOKEN_IDENTIFIER)) {
        return error(p, "expected identifier after 'struct'");
    }

    if (!expect(p, TOKEN_LEFT_BRACE)) {
        return error(p, "expected '{'");
    }

    AstExpr **members = malloc(sizeof(AstExpr *));
    int memberCount = 0;
    int memberCapacity = 1;

    if (!match(p, TOKEN_RIGHT_BRACE)) {
        do {
            // handles leading commas
            // if (match(p, TOKEN_COMMA)) {
            //     advance(p);
            //     if (match(p, TOKEN_RIGHT_BRACE)) {
            //         break;
            //     } else {
            //         recede(p);
            //     }
            // }

            AstExpr *member = parseStatement(p);
            if (isErr(member)) return member;

            // Token fieldName = currentToken(p);
            // if (!expect(p, TOKEN_IDENTIFIER)) {
            //     return error(p, "expected identifier");
            // }

            // if (!expect(p, TOKEN_COLON)) {
            //     return error(p, "expected ':'");
            // }

            // AstExpr *type = parseType(p);
            // if (isErr(type)) return type;

            // AstExpr *field = newStructField(fieldName.lexeme, type);

            if (memberCount >= memberCapacity) {
                memberCapacity *= 2;
                members = realloc(members, sizeof(AstExpr *) * memberCapacity);
            }
            
            members[memberCount++] = member;

        } while (!match(p, TOKEN_RIGHT_BRACE));
    }

    if (!expect(p, TOKEN_RIGHT_BRACE)) {
        return error(p, "expected '}'");
    }

    return newStructDeclaration(name.lexeme, members, memberCount, memberCapacity, isInterface, isPublic);
}

static AstExpr *parseInterface(Parser *p) {
    advance(p);

    if (match(p, TOKEN_STRUCT)) {
        recede(p);
        
        return parseStruct(p);
    }

    return error(p, "expected 'struct' declaration after 'interface'");
}

static AstExpr *parseNext(Parser *p) {
    advance(p);

    return newNextStatement();
}


static AstExpr *parseStop(Parser *p) {
    advance(p);

    return newStopStatement();
}

static AstExpr *parseWhile(Parser *p) {
    advance(p);

    AstExpr *condition = parseExpr(p);
    if (isErr(condition)) return condition;

    if (!expect(p, TOKEN_LEFT_BRACE)) {
        return error(p, "expected '{'");
    }

    AstExpr *block = parseBlock(p);
    if (isErr(block)) return block;

    if (!expect(p, TOKEN_RIGHT_BRACE)) {
        return error(p, "expected '}'");
    }

    return newWhileStatement(condition, block);
}

static AstExpr *parseCallExpression(Parser *p) {
    Token name = currentToken(p);
    advance(p);

    if (!expect(p, TOKEN_LEFT_PAREN)) {
        return error(p, "expected '(' in call expression");
    }

    AstExpr **arguments = malloc(sizeof(AstExpr *));
    int count = 0;
    int capacity = 1;

    if (!match(p, TOKEN_RIGHT_PAREN)) {
        recede(p);

        do {
            advance(p);

            AstExpr *val = parseExpr(p);
            if (isErr(val)) return val;

            if (count >= capacity) {
                capacity *= 2;
                arguments = realloc(arguments, sizeof(AstExpr *) * capacity);
            }
            arguments[count++] = val;

        } while (match(p, TOKEN_COMMA));
    }
    
    if (!expect(p, TOKEN_RIGHT_PAREN)) {
        return error(p, "expected '(' in call expression");
    }

    return newCallExpr(name.lexeme, count, capacity, arguments);
}

static AstExpr *parseIdentifier(Parser *p) {
    advance(p);

    if (match(p, TOKEN_SINGLE_EQUALS)) {
        recede(p);

        return parseAssignment(p);
    } else if (match(p, TOKEN_LEFT_PAREN)) {
        recede(p);

        return parseCallExpression(p);
    }
    
    return parseExpr(p);
}

static AstExpr *parsePub(Parser *p) {
    advance(p);

    if (match(p, TOKEN_FN)) {
        recede(p);

        return parseFunction(p);
    } else if (match(p, TOKEN_STRUCT) || match(p, TOKEN_INTERFACE)) {
        recede(p);

        return parseStruct(p);
    }

    return parseExpr(p);
}

static AstExpr *parseFor(Parser *p) {
    advance(p);
    
    Token name = currentToken(p);
    if (!expect(p, TOKEN_IDENTIFIER)) {
        return error(p, "expected identifier after 'for'");
    }

    if (!expect(p, TOKEN_IN)) {
        return error(p, "expected 'in' after for loop condition");
    }

    AstExpr *iterator = parseExpr(p);
    if (isErr(iterator)) return iterator;

    if (!expect(p, TOKEN_LEFT_BRACE)) {
        return error(p, "expected '{'");
    }

    AstExpr *block = parseBlock(p);
    if (isErr(block)) return block;

    if (!expect(p, TOKEN_RIGHT_BRACE)) {
        return error(p, "expected '}'");
    }

    return newForStatement(name.lexeme, iterator, block->asBlock);
}

static AstExpr *parseIf(Parser *p) {
    advance(p);

    AstExpr *condition = parseExpr(p);
    if (isErr(condition)) return condition;

    if (!expect(p, TOKEN_LEFT_BRACE)) {
        return error(p, "expected '{'");
    }

    AstExpr *block = parseBlock(p);
    if (isErr(block)) return block;
    
    if (!expect(p, TOKEN_RIGHT_BRACE)) {
        return error(p, "expected '}'");
    }

    return newIfStatement(condition, block->asBlock);
}


static AstExpr *parseStatement(Parser *p) {
    Token token = currentToken(p);

    switch (token.type) {
        case TOKEN_LET: {
            return parseLet(p);
        }
        case TOKEN_IF: {
            return parseIf(p);
        }
        case TOKEN_PUB: {
            return parsePub(p);
        }
        case TOKEN_FOR: {
            return parseFor(p);
        }
        case TOKEN_STAR: {
            return parseAssignment(p);
        }
        case TOKEN_IDENTIFIER: {
            return parseIdentifier(p);
        }
        case TOKEN_FN: {
            return parseFunction(p);
        }
        case TOKEN_RETURN: {
            return parseReturn(p);
        }
        case TOKEN_STRUCT: {
            return parseStruct(p);
        }
        case TOKEN_INTERFACE: {
            return parseInterface(p);
        }
        case TOKEN_WHILE: {
            return parseWhile(p);
        }
        case TOKEN_STOP: {
            return parseStop(p);
        }
        case TOKEN_NEXT: {
            return parseNext(p);
        }
        default: {
            return parseExpr(p);
        }
    }
}

void addExpr(AstExpr *expr, Parser *p) {
    if (p->ast.exprCount >= p->ast.exprCapacity) {
        p->ast.exprCapacity *= 2;
        p->ast.exprs = realloc(p->ast.exprs, sizeof(AstExpr *) * p->ast.exprCapacity);
    }

    p->ast.exprs[p->ast.exprCount++] = expr;
}

void parse(Parser *p) {
    while (!isEnd(p)) {
        AstExpr *expr = parseStatement(p);
        addExpr(expr, p);

        if (expr->type == AST_ERR_EXPR) break;
    }

    if (p->debug) printAst(p);
}