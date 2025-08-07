#include <stdbool.h>
#include <stdio.h>

void main();
inline signed int zero();
inline signed int one();
inline float e();
inline float pi();
signed int square(signed int n);
signed int cube(signed int n);
signed int power(signed int n, signed int exp);
signed int abs(signed int x);
signed int max(signed int a, signed int b);
signed int min(signed int a, signed int b);
_Bool isEven(signed int n);
_Bool isOdd(signed int n);
signed int factorial(signed int n);


void main() {
signed int result = power(2, 8);
return result;

// deferred
}
typedef struct {
char dummy;
} math;
inline signed int zero() {
return 0;
}

inline signed int one() {
return 1;
}

inline float e() {
return 2.718280;
}

inline float pi() {
return 3.141590;
}

signed int square(signed int n) {
return n*n;
}

signed int cube(signed int n) {
return n*n*n;
}

signed int power(signed int n, signed int exp) {
if (exp==0) {
return 1;
}
signed int total = 1;
signed int i = 0;
while (i<exp) {
total = total*n;
i = i+1;
}
return total;

// deferred
}

signed int abs(signed int x) {
return x<0 ? -x : x;;

// deferred
}

signed int max(signed int a, signed int b) {
return a>b ? a : b;;

// deferred
}

signed int min(signed int a, signed int b) {
return a>b ? b : a;;

// deferred
}

_Bool isEven(signed int n) {
switch (n%2) {
case 0:
return true;
break;
default:
return false;}

// deferred
}

_Bool isOdd(signed int n) {
switch (n%2) {
case 0:
return false;
break;
default:
return true;}

// deferred
}

signed int factorial(signed int n) {
return n<=1 ? 1 : n*factorial(n-1);;

// deferred
}

