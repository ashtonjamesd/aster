#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "err.h"
#include "expr.h"

Parser newParser(char *filePath, Token *tokens, int tokenCount, bool debug) {
    Parser parser;

    parser.filePath = filePath;

    parser.position = 0;
    parser.tokenCount = tokenCount;
    parser.tokens = tokens;

    parser.ast = (Ast){
        .exprCapacity = 1,
        .exprCount = 0,
        .exprs = malloc(sizeof(AstExpr))
    };

    parser.hadErr = false;
    parser.debug = debug;

    return parser;
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
            free(expr->asLet.name.name);
            freeExpr(expr->asLet.value);
            break;
        default: {
            fprintf(stderr, "parse: unknown AST expression type in 'freeExpr': %d\n", expr->type);
            exit(1);
        }
    }

    free(expr);
}

void freeParser(Parser *parser) {
    for (int i = 0; i < parser->ast.exprCount; i++) {
        freeExpr(parser->ast.exprs[i]);
    }

    free(parser->ast.exprs);
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
            printIndent(indent * 2);
            printf("name: %s\n", expr.asLet.name.name);
            printIndent(indent * 2);
            printf("value:\n");
            printExpr(*expr.asLet.value, indent * 4);
            break;
        }
        default: {
            fprintf(stderr, "parse: unknown ast expression type in 'printExpr': %d\n", expr.type);
            exit(1);
        }
    }
}

void printAst(Parser *parser) {
    printf("\nAST:\n");
    for (int i = 0; i < parser->ast.exprCount; i++) {
        printExpr(*parser->ast.exprs[i], 1);
    }
}

static inline Token currentToken(Parser *parser) {
    return parser->tokens[parser->position];
}

static inline bool match(Parser *parser, TokenType type) {
    return currentToken(parser).type == type;
}

static inline bool isEnd(Parser *parser) {
    if (parser->position >= parser->tokenCount) return true;
    if (currentToken(parser).type == TOKEN_EOF) return true;

    return false;
}

static inline void advance(Parser *parser) {
    parser->position++;
}

static inline bool isErr(AstExpr *expr) {
    return expr->type == AST_ERR_EXPR;
}

static AstExpr *parsePrimary(Parser *parser) {
    Token token = currentToken(parser);
    advance(parser);

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
            compileErrFromParse(parser, "expected expression");
            return newErrExpr();
        }
    }
}

static AstExpr *parseExpr(Parser *parser) {
    return parsePrimary(parser);
}

static AstExpr *parseLet(Parser *parser) {
    advance(parser);

    AstExpr *name = parsePrimary(parser);
    if (name->type != AST_IDENTIFIER) return newErrExpr();

    advance(parser); // =

    AstExpr *value = parseExpr(parser);
    if (isErr(value)) return value;

    return newLetDeclaration(name, value);
}

static AstExpr *parseStatement(Parser *parser) {
    if (match(parser, TOKEN_LET)) return parseLet(parser);

    return parseExpr(parser);
}

void addExpr(AstExpr *expr, Parser *parser) {
    if (parser->ast.exprCount >= parser->ast.exprCapacity) {
        parser->ast.exprCapacity *= 2;
        parser->ast.exprs = realloc(parser->ast.exprs, sizeof(AstExpr) * parser->ast.exprCapacity);
    }

    parser->ast.exprs[parser->ast.exprCount++] = expr;
}

void parse(Parser *parser) {
    while (!isEnd(parser)) {
        AstExpr *expr = parseStatement(parser);
        addExpr(expr, parser);

        if (expr->type == AST_ERR_EXPR) break;
    }

    if (parser->debug) printAst(parser);
}