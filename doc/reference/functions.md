## Functions

The syntax for writing functions is as follows:

```
// does nothing and returns nothing
fn foo(): u0 {}
```

Function parameters begin with the identifier, followed by the type.

```
pub fn max(a: i32, b: i32): i32 {
    return if a > b then a else b
}
```

The return type of the function must be specified.

Functions starting with `pub` are public and can be used in different modules


Arrow functions are a shorthand way to define concise, single-expression functions, similar to lambda expressions or expression-bodied functions in other languages.

Here is an example for the standard library ascii module. This example uses a ternary and an arrow function.

```
pub fn toLower(c: i8): i8 => if isUpper(c) then c + 32 else c
```

Functions with no parameters can omit the parentheses, for instance:

```
pub fn zero(): i8 => 0

// becomes:

pub fn zero: i8 => 0
```