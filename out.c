#include <stdbool.h>
#include <stdio.h>

signed char main();
Colour new(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
Colour default();
Colour red();
Colour green();
Colour blue();


signed char main() {
Colour colour = new(0, 0, 0, 0);
}
typedef struct {
unsigned char r;
unsigned char g;
unsigned char b;
unsigned char a;

} Colour;
Colour new(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
Colour colour = {
.r = r,
.g = g,
.b = b,
.a = a,
};
return colour;
}

Colour default() {
Colour colour = {
.r = 0,
.g = 0,
.b = 0,
.a = 0,
};
return colour;
}

Colour red() {
Colour colour = {
.r = 255,
.g = 0,
.b = 0,
.a = 255,
};
return colour;
}

Colour green() {
Colour colour = {
.r = 0,
.g = 255,
.b = 0,
.a = 255,
};
return colour;
}

Colour blue() {
Colour colour = {
.r = 0,
.g = 0,
.b = 255,
.a = 255,
};
return colour;
}

