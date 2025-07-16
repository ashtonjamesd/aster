#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyze.h"
#include "err.h"

Analyzer newAnalyzer(Parser *parser) {
    Analyzer analyzer;

    analyzer.parser = parser;
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
        case AST_STRUCT_DECLARATION: {
            break;
        }
        case AST_FLOAT_LITERAL: {
            break;
        }
        case AST_IDENTIFIER: {
            break;
        }
        case AST_INTEGER_LITERAL: {
            break;
        }
        case AST_STRING_LITERAL: {
            break;
        }
        case AST_CHAR_LITERAL: {
            break;
        }
        case AST_ERR_EXPR: {
            exitWithInternalCompilerError("found error expression in analyzer");
            break;
        }
        default: {
            exitWithInternalCompilerError("unknown expression type in 'analyzeExpr'");
        }
    }
}

void raiseNoEntryPointErr(Analyzer *analyzer) {
    compileErrFromAnalyzer(analyzer, "program requires an entry point 'main' defined\n");
}

void analyze(Analyzer *analyzer) {
    if (analyzer->parser->ast.exprCount == 0) {
        raiseNoEntryPointErr(analyzer);
        return;
    }

    bool hasEntryPoint = false;
    for (int i = 0; i < analyzer->parser->ast.exprCount; i++) {
        AstExpr *expr = analyzer->parser->ast.exprs[i];

        if (!hasEntryPoint && expr->type == AST_FUNCTION_DECLARATION) {
            if (strcmp(expr->asFunction.name, "main") == 0) {
                hasEntryPoint = true;
            }
        }

        analyzeExpr(expr);
    }

    if (!hasEntryPoint) {
        raiseNoEntryPointErr(analyzer);
        return;
    }
}