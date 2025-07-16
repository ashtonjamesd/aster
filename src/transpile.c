#include <stdio.h>

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

    for (int i = 0; i < function.block.count; i++) {
        emitExpr(t, function.block.body[i]);
    }

    // emitNewline(t);
    emitRightBrace(t);
    emitNewline(t);
}

static void emitReturnStatement(Transpiler *t, ReturnStatement returnStatement) {
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

    emitRightBrace(t);
    emitNewline(t);
}


static void emitLetDeclaration(Transpiler *t, LetDeclaration let) {
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

    if (structDeclaration.fieldCount == 0) {
        emit(t, "char dummy;");
    }

    for (int i = 0; i < structDeclaration.fieldCount; i++) {
        emitTypeExpression(t, structDeclaration.fields[i].type);
        emit(t, structDeclaration.fields[i].name);

        emitSemicolon(t);
        emitNewline(t);
    }

    emitNewline(t);
    emitRightBrace(t);
    emitSemicolon(t);

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
    fprintf(t->fptr, "%d", integer.value);
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
    emit(t, mapOperatorType(unary.operator));
    emitExpr(t, unary.right);
}

static void emitBinary(Transpiler *t, BinaryExpr binary) {
    emitExpr(t, binary.left);
    emit(t, mapOperatorType(binary.operator));
    emitExpr(t, binary.right);
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

    // TODO: call expression as a statement won't need ';'
    emitSemicolon(t);
}

static void emitExpr(Transpiler *t, AstExpr *expr) {
    switch (expr->type) {
        case AST_FUNCTION_DECLARATION: {
            emitFunctionDeclaration(t, expr->asFunction);
            break;
        }
        case AST_STRUCT_DECLARATION: {
            emitStructDeclaration(t, expr->asStruct);
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
        case AST_ASSIGN_EXPR: {
            emitAssignExpression(t, expr->asAssign);
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