## Structs

Structs work almost exactly as they do in C.

```
// defines a struct with 5 fields
struct Colour {
    pub r: u8
    pub g: u8
    pub b: u8
    pub a: u8

    x: u8 // private field
}
```

Fields can be either public or private. This is signified with the 'pub' keyword.

Structs can also contain functions. These functions are accessible on struct instances.

```
struct Colour {
    // ..

    pub fn new(r: u8, g: u8, b: u8, a: u8): Colour {
        let colour: Colour = {
            r: r,
            g: g,
            b: b,
            a: a
        }

        return colour
    }
}
```

// The following has not been implemented in the compiler yet.

A struct interface is a non-instantiable struct that other structs can implement.

```
pub struct interface Printable {
    pub fn print(): void
}
```

Use the 'with' keyword to implement an interface struct.

```
pub struct String with Printable {
    pub fn print(): void {
        // ..
    }
}
```