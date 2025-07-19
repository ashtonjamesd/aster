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
    compileErrFromAnalyzer(analyzer, 
        "program requires an entry point 'main' defined\n"
    );
}

static void raiseDuplicateSymbol(Analyzer *analyzer, char *name) {
    compileErrFromAnalyzer(analyzer, 
        "symbol '%s' already defined in this scope\n", name
    );
}

static void raiseUndefinedSymbol(Analyzer *analyzer, char *name) {
    compileErrFromAnalyzer(analyzer, 
        "symbol '%s' does not exist in this scope\n", name
    );
}

static void raiseInvalidLoopContextualKeyword(Analyzer *analyzer, char *keyword) {
    compileErrFromAnalyzer(analyzer, 
        "keyword '%s' can only be used inside a loop body\n", keyword
    );
}

static void raiseIntOverflow(Analyzer *analyzer, uint64_t value, char *name, char *type) {
    compileErrFromAnalyzer(analyzer, 
        "compile constant value %llu overflows type %s on symbol '%s'", value, type, name
    );
}

static void raiseIntUnderflow(Analyzer *analyzer, uint64_t value, char *name, char *type) {
    compileErrFromAnalyzer(analyzer, 
        "compile constant value %llu underflows type %s on symbol '%s'", value, type, name
    );
}

static void raiseConstantCannotBeReassigned(Analyzer *analyzer, char *name) {
    compileErrFromAnalyzer(analyzer, 
        "constant symbol '%s' cannot be reassigned", name
    ); 
}

static bool declareSymbol(SymbolTable *table, char *name, AstExpr *declaration) {
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

    current->symbols[current->count++] = (Symbol){ .name = name, .declaration = declaration };

    return true;
}

static ConstEvalResult newConstant(uint64_t value) {
    return (ConstEvalResult){ .isConstant = true, .value = value };
}

static ConstEvalResult newNonConstant() {
    return (ConstEvalResult){ .isConstant = false };
}

static ConstEvalResult evaluateConstExpr(AstExpr *expr) {
    switch (expr->type) {
        case AST_INTEGER_LITERAL: {
            return newConstant(expr->asInteger.value);
        }
        case AST_BINARY: {
            ConstEvalResult leftValue = evaluateConstExpr(expr->asBinary.left);
            ConstEvalResult rightValue = evaluateConstExpr(expr->asBinary.right);
            
            if (!leftValue.isConstant || !rightValue.isConstant) {
                return newNonConstant();
            }

            switch (expr->asBinary.operator) {
                case OP_PLUS: {
                    return newConstant(leftValue.value + rightValue.value);
                }
                case OP_MINUS: {
                    return newConstant(leftValue.value - rightValue.value);
                }
                case OP_DEREF: {
                    return newConstant(leftValue.value * rightValue.value);
                }
                case OP_DIVIDE: {
                    return newConstant(leftValue.value / rightValue.value);
                }
                default: {
                    return newNonConstant();
                }
            }
        }
        case AST_UNARY: {
            ConstEvalResult rightValue = evaluateConstExpr(expr->asUnary.right);

            switch (expr->asUnary.operator) {
                case OP_MINUS: {
                    return newConstant(-rightValue.value);
                }
                default: {
                    return newNonConstant();
                }
            }
        }
        default: {
            return newNonConstant();
        }
    }
}

static void checkBitOverflows(Analyzer *analyzer, TypeExpr type, uint64_t value, char *name) {
    if (strcmp(type.name, "i8") == 0) {
        if (value > INT8_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < INT8_MIN) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    } else if (strcmp(type.name, "u8") == 0) {
        if (value > UINT8_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < 0) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    } else if (strcmp(type.name, "i16") == 0) {
        if (value > INT16_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < INT16_MIN) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    } else if (strcmp(type.name, "u16") == 0) {
        if (value > UINT16_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < 0) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    } else if (strcmp(type.name, "i32") == 0) {
        if (value > INT32_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < INT32_MIN) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    } else if (strcmp(type.name, "u32") == 0) {
        if (value > UINT32_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < 0) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    } else if (strcmp(type.name, "i64") == 0) {
        if (value > INT64_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < INT64_MIN) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    } else if (strcmp(type.name, "u64") == 0) {
        if (value > UINT64_MAX) {
            raiseIntOverflow(analyzer, value, name, type.name);
        } else if (value < 0) {
            raiseIntUnderflow(analyzer, value, name, type.name);
        }
    }
}

static void analyzeFunctionDeclaration(Analyzer *analyzer, AstExpr *functionExpr) {
    FunctionDeclaration function = functionExpr->asFunction;

    if (!declareSymbol(&analyzer->table, function.name, functionExpr)) {
        raiseDuplicateSymbol(analyzer, function.name);
    }

    pushScope(&analyzer->table);

    for (int i = 0; i < function.paramCount; i++) {
                                                            // TODO!: fix this at some point
        if (!declareSymbol(&analyzer->table, function.parameters[i].name, NULL)) {
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

static Symbol *retrieveSymbol(SymbolTable *table, char *name) {
    if (table->depth == 0) return NULL;

    Scope *current = &table->scopes[table->depth - 1];
    for (int i = 0; i < current->count; i++) {
        if (strcmp(current->symbols[i].name, name) == 0) {
            return &current->symbols[i];
        }
    }

    return NULL;
}

static void analyzeAssignExpr(Analyzer *analyzer, AssignmentExpr assign) {
    Symbol *symbol = retrieveSymbol(&analyzer->table, assign.name);

    if (!symbol) {
        raiseUndefinedSymbol(analyzer, assign.name);
        return;
    }

    if (symbol->declaration->type == AST_LET) {
        if (symbol->declaration->asLet.isConstant) {
            raiseConstantCannotBeReassigned(analyzer, symbol->declaration->asLet.name);
        }
    }
}

static void analyzeLet(Analyzer *analyzer, AstExpr *letExpr) {
    LetDeclaration let = letExpr->asLet;
    
    if (!declareSymbol(&analyzer->table, let.name, letExpr)) {
        raiseDuplicateSymbol(analyzer, let.name);
    }

    analyzeExpr(analyzer, let.value);

    ConstEvalResult result = evaluateConstExpr(let.value);
    checkBitOverflows(analyzer, let.type, result.value, let.name);
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
    if (!retrieveSymbol(&analyzer->table, call.name)) {
        // fix to search outer scopes not just current
        // raiseUndefinedSymbol(analyzer, call.name);
    }
}

static void analyzeExpr(Analyzer *analyzer, AstExpr *expr) {
    switch (expr->type) {
        case AST_LET: {
            analyzeLet(analyzer, expr);
            break;
        }
        case AST_ASSIGN_EXPR: {
            analyzeAssignExpr(analyzer, expr->asAssign);
            break;
        }
        case AST_FOR: {
            break;
        }
        case AST_PROPERTY_ACCESS: {
            break;
        }
        case AST_MATCH: {
            break;
        }
        case AST_ENUM: {
            break;
        }
        case AST_STRUCT_INITIALIZER: {
            break;
        }
        case AST_FUNCTION_DECLARATION: {
            analyzeFunctionDeclaration(analyzer, expr);
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