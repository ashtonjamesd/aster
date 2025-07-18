#include <stdbool.h>
#include <stdio.h>

signed char main();
_Bool isAlphanumeric(signed char c);
_Bool isPunctuation(signed char c);
_Bool isAscii(signed char c);
signed char toLower(signed char c);
signed char toUpper(signed char c);
_Bool isLetter(signed char c);
_Bool isLower(signed char c);
_Bool isUpper(signed char c);
_Bool isDigit(signed char c);
_Bool isPrintable(signed char c);
_Bool isControl(signed char c);
_Bool isVowel(signed char c);
_Bool isConsonant(signed char c);
_Bool isHex(signed char c);
_Bool isBin(signed char c);
_Bool isWhitespace(signed char c);


signed char main() {
}
struct ascii {
char dummy;
};
_Bool isAlphanumeric(signed char c) {
return isDigit(c)||isLetter(c);
}

_Bool isPunctuation(signed char c) {
return !isAlphanumeric(c)&&!isWhitespace(c);
}

_Bool isAscii(signed char c) {
return c>=0&&c<=127;
}

signed char toLower(signed char c) {
return isUpper(c) ? c+32 : c;;
}

signed char toUpper(signed char c) {
return isLower(c) ? c-32 : c;;
}

_Bool isLetter(signed char c) {
return isLower(c)||isUpper(c);
}

_Bool isLower(signed char c) {
return c>='a'&&c<='z';
}

_Bool isUpper(signed char c) {
return c>='A'&&c<='Z';
}

_Bool isDigit(signed char c) {
return c>='0'&&c<='9';
}

_Bool isPrintable(signed char c) {
return c>=32&&c<=126;
}

_Bool isControl(signed char c) {
return (c>=0&&c<32)||c==127;
}

_Bool isVowel(signed char c) {
switch (c) {
case 'a':
return true;
break;
case 'e':
return true;
break;
case 'i':
return true;
break;
case 'o':
return true;
break;
case 'u':
return true;
break;
default:
return false;}
}

_Bool isConsonant(signed char c) {
return isLetter(c)&&!isVowel(c);
}

_Bool isHex(signed char c) {
return isDigit(c)||(c>='a'&&c<='f')||(c>='A'&&c<='F');
}

_Bool isBin(signed char c) {
return c=='1'||c=='0';
}

_Bool isWhitespace(signed char c) {
return c==' '||c=='\t'||c=='\r'||c=='\n'||c=='\v'||c=='\f';
}

