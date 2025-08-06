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