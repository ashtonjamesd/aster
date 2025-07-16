#ifndef map_h
#define map_h

#include "tokenize.h"
#include "parse.h"

char *mapPrimitiveTypeToC(char *type);
OperatorType mapToOperatorType(TokenType type);
char *mapOperatorType(OperatorType type);

#endif