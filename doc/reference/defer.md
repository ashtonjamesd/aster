## Defer

Defer is used to execute code after the current scope exits.

```
pub fn main(): i8 {
    let x: i8 = 10
    defer x = x + 1 // executes at the end

    x = x * 2
    // x is 22 here
}
```