## Embedding C Source Code

Aster allows you to, with care, embed C source code directly into your program.

This is not recommended in common practice as C is an inherently unsafe language. However, it may fit your use cases.

```
pub fn main: i8 => 0

// starts an embed block
embed {
    static int x = 10;
}
```