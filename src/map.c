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

    exitWithInternalCompilerError("unable to map aster type to c type");
    
    // prevents compiler warning (-Wreturn-type)
    exit(1);
}