#include <stdio.h>
#include <stdlib.h>

#include "transpile.h"
#include "map.h"
#include "err.h"

static void emitExpr(Transpiler *t, AstExpr *expr);

Transpiler newTranspiler(FILE *fptr, Ast ast) {
    Transpiler transpiler;
    transpiler.ast = ast;
    transpiler.fptr = fptr;

    return transpiler;
}

static inline void emit(Transpiler *t, char *c) {
    fprintf(t->fptr, "%s", c);
}

static inline void emitStar(Transpiler *t) {
    emit(t, "*");
}

static inline void emitSpace(Transpiler *t) {
    emit(t, " ");
}

static inline void emitNewline(Transpiler *t) {
    emit(t, "\n");
}

static inline void emitLeftParen(Transpiler *t) {
    emit(t, "(");
}

static inline void emitRightParen(Transpiler *t) {
    emit(t, ")");
}

static inline void emitLeftBrace(Transpiler *t) {
    emit(t, "{");
}

static inline void emitRightBrace(Transpiler *t) {
    emit(t, "}");
}

static inline void emitComma(Transpiler *t) {
    emit(t, ",");
}

static inline void emitSemicolon(Transpiler *t) {
    emit(t, ";");
}

static void emitTypeExpression(Transpiler *t, TypeExpr type) {
    emit(t, mapPrimitiveTypeToC(type.name));
    for (int i = 0; i < type.ptrDepth; i++) emitStar(t);
    
    emitSpace(t);
}

static void emitFunctionDeclaration(Transpiler *t, FunctionDeclaration function) {
    emitNewline(t);

    emitTypeExpression(t, function.returnType);
    emit(t, function.name);

    emitLeftParen(t);

    for (int i = 0; i < function.paramCount; i++) {
        emitTypeExpression(t, function.parameters[i].type);
        emit(t, function.parameters[i].name);

        if (i != function.paramCount - 1) {
            emitComma(t);
            emitSpace(t);
        }
    }

    emitRightParen(t);
    emitSpace(t);
    emitLeftBrace(t);
    emitNewline(t);

    if (!function.isLambda) {
        for (int i = 0; i < function.block.count; i++) {
            emitExpr(t, function.block.body[i]);
        }
    } else {
        emit(t, "return");
        emitSpace(t);
        emitExpr(t, function.lambdaExpr);
        
        emitSemicolon(t);
        emitNewline(t);
    }

    // emitNewline(t);
    emitRightBrace(t);
    emitNewline(t);
}

static void emitReturnMatch(Transpiler *t, MatchExpr match) {
    emit(t, "switch");
    emitSpace(t);

    emitLeftParen(t);
    emitExpr(t, match.expression);
    emitRightParen(t);
    emitSpace(t);

    emitLeftBrace(t);
    emitNewline(t);

    for (int i = 0; i < match.caseCount; i++) {
        if (match.cases[i].isElseCase) {
            emit(t, "default");
            emit(t, ":");
            emitNewline(t);

            emit(t, "return");
            emitSpace(t);
            emitExpr(t, match.cases[i].expression);
            emitSemicolon(t);
            continue;
        }

        emit(t, "case");
        emitSpace(t);
        emitExpr(t, match.cases[i].pattern);

        emit(t, ":");
        emitNewline(t);

        emit(t, "return");
        emitSpace(t);
        emitExpr(t, match.cases[i].expression);
        emitSemicolon(t);

        emitNewline(t);
        emit(t, "break");
        emitSemicolon(t);

        emitNewline(t);
    }

    emitRightBrace(t);
    emitNewline(t);
}

static void emitReturnStatement(Transpiler *t, ReturnStatement returnStatement) {
    if (returnStatement.value->type == AST_MATCH) {
        emitReturnMatch(t, returnStatement.value->asMatch);
        return;
    }

    emit(t, "return");
    emitSpace(t);

    emitExpr(t, returnStatement.value);
    emitSemicolon(t);

    emitNewline(t);
}

static void emitWhileStatement(Transpiler *t, WhileStatement whileStatement) {
    emit(t, "while");
    emitSpace(t);

    emitLeftParen(t);
    emitExpr(t, whileStatement.condition);
    emitRightParen(t);

    emitSpace(t);

    emitLeftBrace(t);
    emitNewline(t);

    for (int i = 0; i < whileStatement.block.count; i++) {
        emitExpr(t, whileStatement.block.body[i]);
    }

    emitExpr(t, whileStatement.alteration);

    emitRightBrace(t);
    emitNewline(t);
}

static void emitForStatement(Transpiler *t, ForStatement forStatement) {
    if (!t) return;
    if (!forStatement.iterator) return;
}

// block cases (x => { ... }) are not allowed for assignment, must be: x => <expr>
static void emitLetMatchAssignment(Transpiler *t, LetDeclaration let, MatchExpr match) {
    emitTypeExpression(t, let.type);
    emit(t, let.name);
    emitSemicolon(t);
    emitNewline(t);

    emit(t, "switch");
    emitSpace(t);

    emitLeftParen(t);
    emitExpr(t, match.expression);
    emitRightParen(t);
    emitSpace(t);

    emitLeftBrace(t);
    emitNewline(t);

    for (int i = 0; i < match.caseCount; i++) {
        emit(t, "case");
        emitSpace(t);
        emitExpr(t, match.cases[i].pattern);

        emit(t, ":");
        emitNewline(t);

        emit(t, let.name);
        emit(t, "=");
        emitExpr(t, match.cases[i].expression);
        emitSemicolon(t);

        emitNewline(t);
        emit(t, "break");
        emitSemicolon(t);

        emitNewline(t);
    }

    emitRightBrace(t);
    emitNewline(t);
}

static void emitLetDeclaration(Transpiler *t, LetDeclaration let) {
    if (let.value->type == AST_MATCH) {
        emitLetMatchAssignment(t, let, let.value->asMatch);
        return;
    }

    emitTypeExpression(t, let.type);
    emit(t, let.name);

    emitSpace(t);
    emit(t, "=");
    emitSpace(t);

    emitExpr(t, let.value);
    emitSemicolon(t);
    
    emitNewline(t);
}

static void emitStructDeclaration(Transpiler *t, StructDeclaration structDeclaration) {
    emit(t, "struct");
    emitSpace(t);
    emit(t, structDeclaration.name);
    emitSpace(t);

    emitLeftBrace(t);
    emitNewline(t);

    int fieldCount = 0;
    for (int i = 0; i < structDeclaration.memberCount; i++) {
        if (structDeclaration.members[i]->type == AST_STRUCT_FIELD) fieldCount++;
    }
    if (fieldCount == 0) {
        emit(t, "char dummy;");
    }

    int *fnIndexs = malloc(sizeof(int));
    int fnIndexCapacity = 1;
    int fnIndexCount = 0;

    for (int i = 0; i < structDeclaration.memberCount; i++) {
        if (structDeclaration.members[i]->type == AST_FUNCTION_DECLARATION) {            
            if (fnIndexCount >= fnIndexCapacity) {
                fnIndexCapacity *= 2;
                fnIndexs = realloc(fnIndexs, sizeof(int) * fnIndexCapacity);
            }

            fnIndexs[fnIndexCount++] = i;
            continue;
        }

        emitExpr(t, structDeclaration.members[i]);

        // emitSemicolon(t);
        emitNewline(t);
    }

    emitNewline(t);
    emitRightBrace(t);
    emitSemicolon(t);

    for (int i = 0; i < fnIndexCount; i++) {
        emitExpr(t, structDeclaration.members[fnIndexs[i]]);
    }

    free(fnIndexs);

    emitNewline(t);
}

static void emitIfStatement(Transpiler *t, IfStatement ifStatement) {
    emit(t, "if");
    emitSpace(t);
    emitLeftParen(t);
    emitExpr(t, ifStatement.condition);
    emitRightParen(t);
    emitSpace(t);

    emitLeftBrace(t);
        emitNewline(t);

    for (int i = 0; i < ifStatement.block.count; i++) {
        emitExpr(t, ifStatement.block.body[i]);
    }

    emitRightBrace(t);
    emitNewline(t);
}

static void emitAssignExpression(Transpiler *t, AssignmentExpr assign) {
    for (int i = 0; i < assign.ptrDepth; i++) emitStar(t);
    emit(t, assign.name);
    emitSpace(t);
    emit(t, "=");
    emitSpace(t);

    emitExpr(t, assign.value);
    emitSemicolon(t);
    emitNewline(t);
}

static void emitIntegerLiteral(Transpiler *t, IntegerLiteralExpr integer) {
    fprintf(t->fptr, "%ld", integer.value);
}

static void emitFloatLiteral(Transpiler *t, FloatLiteralExpr floatLiteral) {
    fprintf(t->fptr, "%f", floatLiteral.value);
}

static void emitStringLiteral(Transpiler *t, StringLiteralExpr string) {
    fprintf(t->fptr, "\"%s\"", string.value);
}

static void emitCharLiteral(Transpiler *t, CharLiteralExpr charLiteral) {
    fprintf(t->fptr, "\'%s\'", charLiteral.value);
}

static void emitBoolLiteral(Transpiler *t, BoolLiteralExpr boolLiteral) {
    fprintf(t->fptr, "%s", boolLiteral.value ? "true" : "false");
}

static void emitIdentifier(Transpiler *t, IdentifierExpr identifier) {
    emit(t, identifier.name);
}

static void emitTernary(Transpiler *t, TernaryExpression ternary) {
    emitExpr(t, ternary.condition);
    emit(t, " ? ");
    emitExpr(t, ternary.trueExpr);
    emit(t, " : ");
    emitExpr(t, ternary.falseExpr);
    emitSemicolon(t);
}

static void emitNext(Transpiler *t, NextStatement next) {
    emit(t, "continue;");
    emitNewline(t);

    // prevent compiler warning
    if (next.dummy == ' ') return;
}

static void emitStop(Transpiler *t, StopStatement stop) {
    emit(t, "break;");
    emitNewline(t);

    // prevent compiler warning
    if (stop.dummy == ' ') return;
}

static void emitUnary(Transpiler *t, UnaryExpr unary) {
    if (unary.operator == OP_SIZEOF) {
        emit(t, mapOperatorType(unary.operator));
        emitLeftParen(t);
        emitExpr(t, unary.right);
        emitRightParen(t);
    } else {
        emit(t, mapOperatorType(unary.operator));
        emitExpr(t, unary.right);
    }
}

static void emitBinary(Transpiler *t, BinaryExpr binary) {
    if (binary.operator == OP_AS_CAST) {
        emitLeftParen(t);
        emit(t, mapPrimitiveTypeToC(binary.right->asIdentifier.name));
        emitRightParen(t);
        emitExpr(t, binary.left);
    } else {
        emitExpr(t, binary.left);
        emit(t, mapOperatorType(binary.operator));
        emitExpr(t, binary.right);
    }
}

static void emitCallExpr(Transpiler *t, CallExpr call) {
    emit(t, call.name);

    emitLeftParen(t);
    for (int i = 0; i < call.argCount; i++) {
        emitExpr(t, call.arguments[i]);
        
        if (i != call.argCount - 1) {
            emitComma(t);
            emitSpace(t);
        }
    }
    emitRightParen(t);
}

static void emitMatchExpression(Transpiler *t, MatchExpr match) {
    emit(t, "switch");
    emitSpace(t);

    emitLeftParen(t);
    emitExpr(t, match.expression);
    emitRightParen(t);
    emitSpace(t);

    emitLeftBrace(t);
    emitNewline(t);

    for (int i = 0; i < match.caseCount; i++) {
        if (match.cases[i].isElseCase) {
            emit(t, "default");
            emit(t, ":");
            emitNewline(t); 

            if (match.cases[i].expression->type == AST_BLOCK) {
                for (int j = 0; j < match.cases[i].expression->asBlock.count; j++) {
                    emitExpr(t, match.cases[i].expression->asBlock.body[j]);
                }
            } else {
                emitExpr(t, match.cases[i].expression);
            }

            emit(t, "break");
            emitSemicolon(t);

            emitNewline(t);
            continue;
        }

        emit(t, "case");
        emitSpace(t);
        emitExpr(t, match.cases[i].pattern);

        emit(t, ":");
        emitNewline(t);

        if (match.cases[i].expression->type == AST_BLOCK) {
            for (int j = 0; j < match.cases[i].expression->asBlock.count; j++) {
                emitExpr(t, match.cases[i].expression->asBlock.body[j]);
            }
        } else {
            emitExpr(t, match.cases[i].expression);
        }

        emit(t, "break");
        emitSemicolon(t);

        emitNewline(t);
    }

    emitRightBrace(t);
    emitNewline(t);
}

static void emitEnumDeclaration(Transpiler *t, EnumDeclaration enumDeclaration) {
    emit(t, "enum");
    emitSpace(t);

    emit(t, enumDeclaration.name);
    emitSpace(t);

    emitLeftBrace(t);
    emitNewline(t);
    
    for (int i = 0; i < enumDeclaration.valueCount; i++) {
        emit(t, enumDeclaration.values[i]);
        emitComma(t);
        emitNewline(t);
    }

    emitRightBrace(t);
    emitSemicolon(t);
}

static void emitExpr(Transpiler *t, AstExpr *expr) {
    switch (expr->type) {
        case AST_FUNCTION_DECLARATION: {
            emitFunctionDeclaration(t, expr->asFunction);
            break;
        }
        case AST_ENUM: {
            emitEnumDeclaration(t, expr->asEnum);
            break;
        }
        case AST_STRUCT_DECLARATION: {
            emitStructDeclaration(t, expr->asStruct);
            break;
        }
        case AST_MATCH: {
            emitMatchExpression(t, expr->asMatch);
            break;
        }
        case AST_LET: {
            emitLetDeclaration(t, expr->asLet);
            break;
        }
        case AST_RETURN: {
            emitReturnStatement(t, expr->asReturn);
            break;
        }
        case AST_WHILE: {
            emitWhileStatement(t, expr->asWhile);
            break;
        }
        case AST_FOR: {
            emitForStatement(t, expr->asFor);
            break;
        }
        case AST_ASSIGN_EXPR: {
            emitAssignExpression(t, expr->asAssign);
            break;
        }
        case AST_IF: {
            emitIfStatement(t, expr->asIf);
            break;
        }
        case AST_NEXT: {
            emitNext(t, expr->asNext);
            break;
        }
        case AST_STOP: {
            emitStop(t, expr->asStop);
            break;
        }
        case AST_UNARY: {
            emitUnary(t, expr->asUnary);
            break;
        }
        case AST_BINARY: {
            emitBinary(t, expr->asBinary);
            break;
        }
        case AST_CALL_EXPR: {
            emitCallExpr(t, expr->asCallExpr);
            break;
        }
        case AST_INTEGER_LITERAL: {
            emitIntegerLiteral(t, expr->asInteger);
            break;
        }
        case AST_FLOAT_LITERAL: {
            emitFloatLiteral(t, expr->asFloat);
            break;
        }
        case AST_STRING_LITERAL: {
            emitStringLiteral(t, expr->asString);
            break;
        }
        case AST_CHAR_LITERAL: {
            emitCharLiteral(t, expr->asChar);
            break;
        }
        case AST_IDENTIFIER: {
            emitIdentifier(t, expr->asIdentifier);
            break;
        }
        case AST_BOOL_LITERAL: {
            emitBoolLiteral(t, expr->asBool);
            break;
        }
        case AST_TERNARY: {
            emitTernary(t, expr->asTernary);
            break;
        }
        default: {
            exitWithInternalCompilerError("unknown expression type in 'emitExpr'");
        }
    }
}

void transpile(Transpiler *t) {
    fprintf(t->fptr, "#include <stdbool.h>\n");
    fprintf(t->fptr, "#include <stdio.h>\n");

    for (int i = 0; i < t->ast.exprCount; i++) {
        emitExpr(t, t->ast.exprs[i]);
    }
}