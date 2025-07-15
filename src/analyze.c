#include <stdio.h>
#include <stdlib.h>

#include "analyze.h"

Analyzer newAnalyzer(Parser *parser) {
    Analyzer analyzer;

    analyzer.ast = parser->ast;
    analyzer.hadErr = false;

    return analyzer;
}

void analyzeFunctionDeclaration(FunctionDeclaration function) {
    if (function.name) return;
}

void analyzeAssignExpr(AssignmentExpr assign) {
    if (assign.value) return;
}

void analyzeLet(LetDeclaration let) {
    if (let.value) return;
}

void analyzeExpr(AstExpr *expr) {
    switch (expr->type) {
        case AST_LET: {
            analyzeLet(expr->asLet);
            break;
        }
        case AST_ASSIGN_EXPR: {
            analyzeAssignExpr(expr->asAssign);
            break;
        }
        case AST_FUNCTION_DECLARATION: {
            analyzeFunctionDeclaration(expr->asFunction);
            break;
        }
        case AST_FLOAT_LITERAL: {}
        case AST_IDENTIFIER: {}
        case AST_INTEGER_LITERAL: {}
        case AST_ERR_EXPR: {
            fprintf(stderr, "critical: found error expression in analyzer\n");
            exit(1);
        }
        default: {
            fprintf(stderr, "analyzer: unknown expression type in 'analyzeExpr': %d\n", expr->type);
            exit(1);
        }
    }
}

void analyze(Analyzer *analyzer) {
    for (int i = 0; i < analyzer->ast.exprCount; i++) {
        analyzeExpr(analyzer->ast.exprs[i]);
    }
}