## Defer

Defer is used to execute code after the current function scope exits. 

```
pub fn main(): i8 {
    let x: i8 = 10
    defer x = x + 1 // executes at the end

    x = x * 2
    // x is 21 here
}
```