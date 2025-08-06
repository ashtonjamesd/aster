## Match Statement

Match statements act the same as the C switch statement but have a more pleasing syntax and some extensions.

Pattern matching is currently not supported.

Declare an empty match statement.

```
match 0 {

}
```

Match can be used as an alternative to if statements with constant conditions.

```
let b: bool = true

match b {
    true => {
        // executes if true
    }, // requires comma after each match block
    false => {
        // executes if false
    }
}
```

Match can also be used as an expression.

```
let isEven: bool = match n mod 2 {
    0 => true,
    1 => false
}
```

Likewise, it can be returned as an expression.

```
return match c {
    'a' => 97,
    // ..
}
```

You can use the `else` match block to provide a default case.

```
let grade: char = 'B'

match grade {
    'A' => {
        // excellent
    },
    'B' => {
        // good
    },
    'C' => {
        // average
    },
    else => {
        // fallback for any other grade
    }
}
```

Match can also be used to contain statement blocks, rather than just expression values:

```
match 1 {
    1 => {
        match 2 + 2 {
            4 => {
                // can put more statements here


                return 1
            },
            else => {
                // ..
                
                return 0
            }
        }
    },
    else => {
        return 0
    }
}
```