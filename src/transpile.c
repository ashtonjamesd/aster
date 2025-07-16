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

static inline void emitSpace(Transpiler *t) {
    fprintf(t->fptr, " ");
}

static inline void emitNewline(Transpiler *t) {
    fprintf(t->fptr, "\n");
}

static inline void emitLeftBrace(Transpiler *t) {
    fprintf(t->fptr, "{");
}

static inline void emitRightBrace(Transpiler *t) {
    fprintf(t->fptr, "}");
}

static inline void emitComma(Transpiler *t) {
    fprintf(t->fptr, ",");
}

static inline void emitSemicolon(Transpiler *t) {
    fprintf(t->fptr, ";");
}

static void emitTypeExpression(Transpiler *t, TypeExpr type) {
    fprintf(t->fptr, "%s", mapPrimitiveTypeToC(type.name));
    for (int i = 0; i < type.ptrDepth; i++) fprintf(t->fptr, "*");
    
    emitSpace(t);
}

static void emitFunctionDeclaration(Transpiler *t, FunctionDeclaration function) {
    emitTypeExpression(t, function.returnType);
    fprintf(t->fptr, "%s", function.name);

    fprintf(t->fptr, "()");
    emitLeftBrace(t);
    emitNewline(t);

    for (int i = 0; i < function.block.count; i++) {
        emitExpr(t, function.block.body[i]);
    }

    emitRightBrace(t);
    emitNewline(t);
}

static void emitReturnStatement(Transpiler *t, ReturnStatement returnStatement) {
    fprintf(t->fptr, "return ");
    emitExpr(t, returnStatement.value);
    emitSemicolon(t);

    emitNewline(t);
}

static void emitLetDeclaration(Transpiler *t, LetDeclaration let) {
    emitTypeExpression(t, let.type);
    fprintf(t->fptr, "%s", let.name);
    fprintf(t->fptr, " = ");
    emitExpr(t, let.value);
    emitSemicolon(t);
    
    emitNewline(t);
}

static void emitStructDeclaration(Transpiler *t, StructDeclaration structDeclaration) {
    fprintf(t->fptr, "struct %s ", structDeclaration.name);
    emitLeftBrace(t);
    emitNewline(t);

    if (structDeclaration.fieldCount == 0) {
        fprintf(t->fptr, "char dummy;");
    }

    for (int i = 0; i < structDeclaration.fieldCount; i++) {
        emitTypeExpression(t, structDeclaration.fields[i].type);
        fprintf(t->fptr, "%s", structDeclaration.fields[i].name);

        emitSemicolon(t);
        emitNewline(t);
    }

    emitNewline(t);
    emitRightBrace(t);
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

static void emitIdentifier(Transpiler *t, IdentifierExpr identifier) {
    fprintf(t->fptr, "%s", identifier.name);
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
            emitIdentifier(t, expr->asIdentifer);
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