## Control Flow

Aster supports a wide range of expressive and common flow constructs.

<br/>

### While

The while construct loops as long as the given condition evaluates to true on each iteration.

```
let i: i8 = 0
while true {
    // ..
}
```

Optionally, not dissimilar to the alteration block in a traditional for loop, you can provide an expression to execute on each iteration.

In this case, `i` will be incremented by one on each iteration. This will not execute on the first iteration.

```
while true : i = i + 1 {
    // ..
}
```

Statements like `stop` and `next` are used to control the behaviour of a loop.

```
while true {
    if false {
        next // skips this iteration
    }

    if true {
        stop // exits the loop
    }
}
```

An indefinite loop can be constructed with:

```
while true {
    // ..
}
```

### If Else

If-else blocks work the exact same as in C.

```
let x: bool = true
if x {
    // executes if true
} else {
    // executes if false
}

// Example with else-if
if x {

} else if not x {

} else {

}
```

### For

For loops can be used to loop through an list of items.

```
for x in someArray {
    // ..
}
```