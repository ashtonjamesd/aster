#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "err.h"
#include "expr.h"

static AstExpr *parseStatement(Parser *p);

Parser newParser(char *filePath, Token *tokens, int tokenCount, bool debug) {
    Parser p;

    p.filePath = filePath;

    p.position = 0;
    p.tokenCount = tokenCount;
    p.tokens = tokens;

    p.ast = (Ast){
        .exprCapacity = 1,
        .exprCount = 0,
        .exprs = malloc(sizeof(AstExpr))
    };

    p.hadErr = false;
    p.debug = debug;

    return p;
}

static void freeExpr(AstExpr *expr) {
    switch (expr->type) {
        case AST_INTEGER_LITERAL:
            break;
        case AST_FLOAT_LITERAL:
            break;
        case AST_ERR_EXPR:
            break;
        case AST_IDENTIFIER:
            free(expr->asIdentifer.name);
            break;
        case AST_LET:
            free(expr->asLet.name);
            freeExpr(expr->asLet.value);
            break;
        case AST_TYPE_EXPR:
            free(expr->asType.name);
            break;
        case AST_ASSIGN_EXPR:
            free(expr->asAssign.name);    
            freeExpr(expr->asAssign.value);
            break;
        case AST_FUNCTION_DECLARATION:
            free(expr->asFunction.name);
            free(expr->asFunction.returnType.name);
            for (int i = 0; i < expr->asFunction.block.count; i++) {
                freeExpr(expr->asFunction.block.body[i]);
            }
            break;
        case AST_RETURN:
            freeExpr(expr->asReturn.value);
            break;
        default: {
            fprintf(stderr, "parse: unknown AST expression type in 'freeExpr': %d\n", expr->type);
            exit(1);
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
            printf("integer literal: %d\n", expr.asInteger.value);
            break;
        }
        case AST_FLOAT_LITERAL: {
            printf("float literal: %f\n", expr.asFloat.value);
            break;
        }
        case AST_IDENTIFIER: {
            printf("identifier: %s\n", expr.asIdentifer.name);
            break;
        }
        case AST_ERR_EXPR: {
            printf("error expression\n");
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
        case AST_FUNCTION_DECLARATION: {
            printf("function declaration:\n");

            printIndent(indent + 2);
            printf("name: %s\n", expr.asFunction.name);

            printIndent(indent + 2);
            printf("type: %s\n", expr.asFunction.returnType.name);
            
            printIndent(indent + 2);
            printf("body (%d):\n", expr.asFunction.block.count);
            for (int i = 0; i < expr.asFunction.block.count; i++) {
                printExpr(*expr.asFunction.block.body[i], indent + 2);
            }

            break;
        }
        case AST_RETURN: {
            printf("return statement:\n");

            printIndent(indent + 2);
            printf("value:\n");
            printIndent(indent + 2);
            printExpr(*expr.asReturn.value, indent);
            break;
        }
        default: {
            fprintf(stderr, "parse: unknown ast expression type in 'printExpr': %d\n", expr.type);
            exit(1);
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
            return newIntegerExpr(atoi(token.lexeme));
        }
        case TOKEN_FLOAT: {
            return newFloatExpr(atof(token.lexeme));
        }
        case TOKEN_IDENTIFIER: {
            return newIdentifierExpr(strdup(token.lexeme));
        }
        default: {
            compileErrFromParse(p, "expected expression");
            return newErrExpr();
        }
    }
}

static AstExpr *parseExpr(Parser *p) {
    return parsePrimary(p);
}


static AstExpr *error(Parser *p, char *err) {
    compileErrFromParse(p, err);
    return newErrExpr();
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

    AstExpr *type = newTypeExpr("", 0);
    if (match(p, TOKEN_COLON)) {
        advance(p);

        AstExpr *typeExpr = parseType(p);
        if (typeExpr->type != AST_TYPE_EXPR) {
            return error(p, "expected type after ':'");
        }
        
        free(type);
        type = typeExpr;
    }

    if (!expect(p, TOKEN_SINGLE_EQUALS)) {
        return error(p, "expected '='");
    }

    AstExpr *value = parseExpr(p);
    if (isErr(value)) {
        return error(p, "expected expression");
    }

    return newLetDeclaration(name.lexeme, type, value);
}

static AstExpr *parseAssignment(Parser *p) {
    Token name = currentToken(p);
    advance(p);

    if (!expect(p, TOKEN_SINGLE_EQUALS)) {
        return error(p, "expected '='");
    }

    AstExpr *value = parseExpr(p);
    if (isErr(value)) return error(p, "expected expression");

    return newAssignExpr(name.lexeme, value);
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
    advance(p);

    Token name = currentToken(p);
    if (!expect(p, TOKEN_IDENTIFIER)) {
        return error(p, "expected identifier after 'fn'");
    }

    if (!expect(p, TOKEN_LEFT_PAREN)) {
        return error(p, "expected '('");
    }

    if (!expect(p, TOKEN_RIGHT_PAREN)) {
        return error(p, "expected ')'");
    }

    if (!expect(p, TOKEN_COLON)) {
        return error(p, "expected ':' and then a type specifier");
    }

    AstExpr *returnType = parseType(p);
    if (returnType->type != AST_TYPE_EXPR) return returnType;

    if (!expect(p, TOKEN_LEFT_BRACE)) {
        return error(p, "expected '{'");
    }

    AstExpr *body = parseBlock(p);
    if (isErr(body)) return body;

    if (!expect(p, TOKEN_RIGHT_BRACE)) {
        return error(p, "expected '}'");
    }

    return newFunctionDeclaration(name.lexeme, returnType, body);
}

static AstExpr *parseReturn(Parser *p) {
    advance(p);

    AstExpr *value = parseExpr(p);
    if (isErr(value)) return value;

    return newReturnStatement(value);
}

static AstExpr *parseStatement(Parser *p) {
    if (match(p, TOKEN_LET)) {
        return parseLet(p);
    } else if (match(p, TOKEN_IDENTIFIER)) {
        return parseAssignment(p);
    } else if (match(p, TOKEN_FN)) {
        return parseFunction(p);
    } else if (match(p, TOKEN_RETURN)) {
        return parseReturn(p);
    }

    return parseExpr(p);
}

void addExpr(AstExpr *expr, Parser *p) {
    if (p->ast.exprCount >= p->ast.exprCapacity) {
        p->ast.exprCapacity *= 2;
        p->ast.exprs = realloc(p->ast.exprs, sizeof(AstExpr) * p->ast.exprCapacity);
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