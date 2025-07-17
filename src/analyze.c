#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analyze.h"
#include "err.h"

static void analyzeExpr(Analyzer *analyzer, AstExpr *expr);

static void initScope(Scope *scope) {
    scope->symbols = malloc(sizeof(Symbol) * 1);
    scope->count = 0;
    scope->capacity = 1;
}

static void pushScope(SymbolTable *table) {
    if (table->depth >= table->capacity) {
        table->capacity *= 2;
        table->scopes = realloc(table->scopes, sizeof(Scope) * table->capacity);
    }

    initScope(&table->scopes[table->depth++]);
}

static void popScope(SymbolTable *table) {
    if (table->depth == 0) return;

    Scope *top = &table->scopes[--table->depth];
    free(top->symbols);
}

void freeAnalyzer(Analyzer *analyzer) {
    for (int i = 0; i < analyzer->table.depth; i++) {
        Scope *scope = &analyzer->table.scopes[i];
        free(scope->symbols);
    }

    free(analyzer->table.scopes);
}

Analyzer newAnalyzer(Parser *parser) {
    Analyzer analyzer;

    analyzer.parser = parser;
    analyzer.hadErr = false;

    analyzer.table.scopes = malloc(sizeof(Scope));
    analyzer.table.depth = 0;
    analyzer.table.capacity = 1;

    analyzer.insideLoop = false;

    pushScope(&analyzer.table);

    return analyzer;
}

static void raiseNoEntryPointErr(Analyzer *analyzer) {
    compileErrFromAnalyzer(analyzer, "program requires an entry point 'main' defined\n");
}

static void raiseDuplicateSymbol(Analyzer *analyzer, char *name) {
    compileErrFromAnalyzer(analyzer, "symbol '%s' already defined in this scope\n", name);
}

static void raiseUndefinedSymbol(Analyzer *analyzer, char *name) {
    compileErrFromAnalyzer(analyzer, "symbol '%s' does not exist in this scope\n", name);
}

static void raiseInvalidLoopContextualKeyword(Analyzer *analyzer, char *keyword) {
    compileErrFromAnalyzer(analyzer, "keyword '%s' can only be used inside a loop body\n", keyword);
}

static bool declareSymbol(SymbolTable *table, char *name) {
    if (table->depth == 0) return true;
    
    Scope *current = &table->scopes[table->depth - 1];

    for (int i = 0; i < current->count; i++) {
        if (strcmp(name, current->symbols[i].name) == 0) {
            return false;
        }
    }

    if (current->count >= current->capacity) {
        current->capacity *= 2;
        current->symbols = realloc(current->symbols, sizeof(Symbol) * current->capacity);
    }

    current->symbols[current->count++] = (Symbol){ .name = name };

    return true;
}

static void analyzeFunctionDeclaration(Analyzer *analyzer, FunctionDeclaration function) {
    if (!declareSymbol(&analyzer->table, function.name)) {
        raiseDuplicateSymbol(analyzer, function.name);
    }

    pushScope(&analyzer->table);

    for (int i = 0; i < function.paramCount; i++) {
        if (!declareSymbol(&analyzer->table, function.parameters[i].name)) {
            raiseDuplicateSymbol(analyzer, function.parameters[i].name);
        }
    }

    if (!function.isLambda) {
        for (int i = 0; i < function.block.count; i++) {
            if (function.block.body[i]->type == AST_RETURN) {
                // resolve return type
            }

            analyzeExpr(analyzer, function.block.body[i]);
        }
    }

    popScope(&analyzer->table);
}

static bool symbolExists(SymbolTable *table, char *name) {
    if (table->depth == 0) return false;

    Scope *current = &table->scopes[table->depth - 1];
    for (int i = 0; i < current->count; i++) {
        if (strcmp(current->symbols[i].name, name) == 0) {
            return true;
        }
    }

    return false;
}

static void analyzeAssignExpr(Analyzer *analyzer, AssignmentExpr assign) {
    if (!symbolExists(&analyzer->table, assign.name)) {
        raiseUndefinedSymbol(analyzer, assign.name);
        return;
    }


}

static void analyzeLet(Analyzer *analyzer, LetDeclaration let) {
    if (!declareSymbol(&analyzer->table, let.name)) {
        raiseDuplicateSymbol(analyzer, let.name);
    }

    analyzeExpr(analyzer, let.value);
}

static void analyzeReturnStatement(Analyzer *analyzer, ReturnStatement returnStatement) {
    if (!analyzer) return;
    if (!returnStatement.value) return;
}

static void analyzeStop(Analyzer *analyzer, StopStatement stop) {
    if (!analyzer->insideLoop) {
        raiseInvalidLoopContextualKeyword(analyzer, "stop");
    }

    // prevent compiler warning
    if (stop.dummy == ' ') return;
}

static void analyzeNext(Analyzer *analyzer, NextStatement next) {
    if (!analyzer->insideLoop) {
        raiseInvalidLoopContextualKeyword(analyzer, "next");
    }

    // prevent compiler warning
    if (next.dummy == ' ') return;
}

static void analyzeWhile(Analyzer *analyzer, WhileStatement whileStatement) {
    analyzer->insideLoop = true;

    analyzeExpr(analyzer, whileStatement.condition);

    for (int i = 0; i < whileStatement.block.count; i++) {
        analyzeExpr(analyzer, whileStatement.block.body[i]);
    }

    analyzer->insideLoop = false;
}

static void analyzeCallExpr(Analyzer *analyzer, CallExpr call) {
    if (!symbolExists(&analyzer->table, call.name)) {
        // fix to search outer scopes not just current
        // raiseUndefinedSymbol(analyzer, call.name);
    }
}

static void analyzeExpr(Analyzer *analyzer, AstExpr *expr) {
    switch (expr->type) {
        case AST_LET: {
            analyzeLet(analyzer, expr->asLet);
            break;
        }
        case AST_ASSIGN_EXPR: {
            analyzeAssignExpr(analyzer, expr->asAssign);
            break;
        }
        case AST_FOR: {
            break;
        }
        case AST_FUNCTION_DECLARATION: {
            analyzeFunctionDeclaration(analyzer, expr->asFunction);
            break;
        }
        case AST_RETURN: {
            analyzeReturnStatement(analyzer, expr->asReturn);
            break;
        }
        case AST_WHILE: {
            analyzeWhile(analyzer, expr->asWhile);
            break;
        }
        case AST_NEXT: {
            analyzeNext(analyzer, expr->asNext);
            break;
        }
        case AST_STOP: {
            analyzeStop(analyzer, expr->asStop);
            break;
        }
        case AST_UNARY: {
            break;
        }
        case AST_BINARY: {
            break;
        }
        case AST_TERNARY: {
            break;
        }
        case AST_CALL_EXPR: {
            analyzeCallExpr(analyzer, expr->asCallExpr);
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
        case AST_BOOL_LITERAL: {
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

        analyzeExpr(analyzer, expr);
    }

    if (!hasEntryPoint) {
        raiseNoEntryPointErr(analyzer);
        return;
    }
}