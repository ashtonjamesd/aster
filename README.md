## Aster

Aster is a general-purpose programming language designed to be safe, simple, and expressive.

The primary purpose of Aster is to build upon the established concepts found in C, with much safer alternatives, while improving code readability through intuitive syntax.

<br/>

```
// this is the entry point to your application
pub fn main(argc: i32, argv: **i8): i32 {
    // ..

    return 0
}
  
// recursively computes the factorial of an integer 'n'
pub fn factorial(n: i32): i32 {
    return if n <= 1 then 1 else n * factorial(n - 1)
}

// returns the absolute value of an integer 'x'
pub fn abs(x: i32): i32 => if x < 0 then -x else x
```
