#include <stdbool.h>
#include <stdio.h>

signed char main();
unsigned char u8max();
unsigned char u8min();
signed char i8max();
unsigned char i8max();
unsigned short u16max();
unsigned short u16min();
signed short i16max();
signed short i16min();


signed char main() {
return main();
}
typedef struct {
char dummy;
} limits;
unsigned char u8max() {
return 255;
}

unsigned char u8min() {
return 0;
}

signed char i8max() {
return 127;
}

unsigned char i8max() {
return -127-1;
}

unsigned short u16max() {
return 65535;
}

unsigned short u16min() {
return 0;
}

signed short i16max() {
return 32767;
}

signed short i16min() {
return -32767-1;
}

