#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "map.h"
#include "err.h"

#define strEq(a, b) strcmp(a, b) == 0

char *mapPrimitiveTypeToC(char *type) {
    if (strEq(type, "u0")) return "void";
    if (strEq(type, "rawptr")) return "void*";

    else if (strEq(type, "u8")) return "unsigned char";
    else if (strEq(type, "u16")) return "unsigned short";
    else if (strEq(type, "u32")) return "unsigned int";
    else if (strEq(type, "u64")) return "unsigned long";

    else if (strEq(type, "i8")) return "signed char";
    else if (strEq(type, "i16")) return "signed short";
    else if (strEq(type, "i32")) return "signed int";
    else if (strEq(type, "i64")) return "signed long";

    else if (strEq(type, "bool")) return "_Bool";

    else if (strEq(type, "f32")) return "float";
    else if (strEq(type, "f64")) return "double";

    else if (strEq(type, "size")) return "size_t";

    return type;
}

OperatorType mapToOperatorType(TokenType type) {
    switch (type) {
        case TOKEN_AMPERSAND: {
            return OP_ADDRESS_OF;
        }
        case TOKEN_STAR: {
            return OP_DEREF;
        }
        case TOKEN_PLUS: {
            return OP_PLUS;
        }
        case TOKEN_MINUS: {
            return OP_MINUS;
        }
        case TOKEN_LESS_THAN: {
            return OP_LESS_THAN;
        }
        case TOKEN_GREATER_THAN: {
            return OP_GREATER_THAN;
        }
        case TOKEN_GREATER_THAN_EQUALS: {
            return OP_GREATER_THAN_EQUALS;
        }
        case TOKEN_LESS_THAN_EQUALS: {
            return OP_LESS_THAN_EQUALS;
        }
        case TOKEN_NOT_EQUALS: {
            return OP_NOT_EQUALS;
        }
        case TOKEN_DOUBLE_EQUALS: {
            return OP_EQUALS;
        }
        case TOKEN_AND: {
            return OP_AND;
        }
        case TOKEN_OR: {
            return OP_OR;
        }
        case TOKEN_NOT: {
            return OP_NOT;
        }
        case TOKEN_SLASH: {
            return OP_DIVIDE;
        }
        case TOKEN_MOD: {
            return OP_MOD;
        }
        case TOKEN_MODULO: {
            return OP_MOD;
        }
        case TOKEN_SIZEOF: {
            return OP_SIZEOF;
        }
        case TOKEN_PIPE: {
            return OP_BITWISE_OR;
        }
        case TOKEN_TILDE: {
            return OP_BITWISE_NOT;
        }
        case TOKEN_SHIFT_LEFT: {
            return OP_BITWISE_SHIFT_LEFT;
        }
        case TOKEN_SHIFT_RIGHT: {
            return OP_BITWISE_SHIFT_RIGHT;
        }
        case TOKEN_CARET: {
            return OP_BITWISE_XOR;
        }
        case TOKEN_XOR: {
            return OP_BITWISE_XOR;
        }
        case TOKEN_AS: {
            return OP_AS_CAST;
        }
        default: {
            exitWithInternalCompilerError("unable to map to operator type");

            // prevent compiler warning
            exit(1);
        }
    }
}

char *mapOperatorType(OperatorType type) {
    switch (type) {
        case OP_ADDRESS_OF: {
            return "&";
        }
        case OP_DEREF: {
            return "*";
        }
        case OP_PLUS: {
            return "+";
        }
        case OP_MINUS: {
            return "-";
        }
        case OP_LESS_THAN: {
            return "<";
        }
        case OP_GREATER_THAN: {
            return ">";
        }
        case OP_GREATER_THAN_EQUALS: {
            return ">=";
        }
        case OP_LESS_THAN_EQUALS: {
            return "<=";
        }
        case OP_EQUALS: {
            return "==";
        }
        case OP_NOT_EQUALS: {
            return "!=";
        }
        case OP_NOT: {
            return "!";
        }
        case OP_AND: {
            return "&&";
        }
        case OP_OR: {
            return "||";
        }
        case OP_SIZEOF: {
            return "sizeof";
        }
        case OP_BITWISE_AND: {
            return "&";
        }
        case OP_BITWISE_OR: {
            return "|";
        }
        case OP_BITWISE_NOT: {
            return "~";
        }
        case OP_BITWISE_SHIFT_RIGHT: {
            return ">>";
        }
        case OP_BITWISE_SHIFT_LEFT: {
            return "<<";
        }
        case OP_MOD: {
            return "%";
        }
        case OP_BITWISE_XOR: {
            return "^";
        }
        // this operator doesn't have a reasonable symbol to return
        // it is handled specially in the transpiler
        case OP_AS_CAST: {
            return "";
        }
        default: {
            exitWithInternalCompilerError("unable to map to operator type");

            // prevent compiler warning
            exit(1);
        }
    }
}