## Pointers

In Aster, pointers work similarly to the way they do in C.

```
let x: i8 = 10

// take the address of x
let xPtr: *i8 = &x

// dereference pointer
let x: i8 = *xPtr
```