#include <stdbool.h>
#include <stdio.h>

void main() {
signed int result = power(2, 8);
}
struct math {
char dummy;
};
signed int zero() {
return 0;
}

signed int one() {
return 1;
}

signed int square(signed int n) {
return n*n;
}

signed int cube(signed int n) {
return n*n*n;
}

float pi() {
return 3.141590;
}

float e() {
return 2.718280;
}

signed int power(signed int n, signed int exp) {
if (exp==zero()) {
return 1;
}
signed int total = 1;
signed int i = 0;
while (i<exp) {
total = total*n;
i = i+1;
}
return total;
}

signed int abs(signed int x) {
return x<0 ? -x : x;;
}

signed int max(signed int a, signed int b) {
return a>b ? a : b;;
}

signed int min(signed int a, signed int b) {
return a>b ? b : a;;
}

_Bool isEven(signed char n) {
return n%2==0;
}

_Bool isOdd(signed char n) {
return n%2!=0;
}

signed int factorial(signed int n) {
return n<=1 ? 1 : n*factorial(n-1);;
}

