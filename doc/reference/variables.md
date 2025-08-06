## Variables

A variable is a reference to a value. Variables must be declared with an initialiser. The type of the variable must be specified.

```
let x: i8 = 0
```



```
// boolean variable defined with an expression
let a: bool = (1 == 1 and 2 == 2)

// u8 pointer variable
let s: *u8 = "Hello, World!"

// declaring 8-bit integer and 8-bit integer pointer
let x: i8 = 1
let xPtr: *i8 = &x;
```