#include "transpile.h"

AsterCTranspiler newTranspiler(Ast ast) {
    AsterCTranspiler transpiler;
    transpiler.ast = ast;

    return transpiler;
}

void emitExpr(AstExpr *expr) {
    if (!expr) return;
}

void transpile(AsterCTranspiler *transpiler) {
    for (int i = 0; i < transpiler->ast.exprCount; i++) {
        emitExpr(transpiler->ast.exprs[i]);
    }
}