#include <stdbool.h>
#include <stdio.h>

void main() {
return zero()+one()+abs(-5);
}
struct math {
char dummy;
};
unsigned char zero() {
return 0;
}

unsigned char one() {
return 1;
}

unsigned char square(unsigned char a, unsigned char b) {
return a*b;
}

unsigned char abs(unsigned char x) {
return x<0 ? -x : x;;
}

