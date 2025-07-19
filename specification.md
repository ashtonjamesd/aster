# Aster

This documentation defines the formal specification of the Aster programming language. It provides a complete and precise description of the language's syntax, semantics, and behaviour, serving as the authoritative reference for compiler development.

Aster is a statically typed, imperative programming language designed for safety, simplicity, and expressiveness. It emphasises readable syntax and safe transpilation, making it suitable for systems programming.


## 1 Lexical Analysis

Source code is interpreted as a sequence of bytes in the ASCII character set. Only 7-bit ASCII characters (U+0000 to U+007F) are guaranteed to be valid in source code. Unicode or multi-byte characters are currently not supported.



### 1.1 Identifiers

Identifiers in Aster have the following grammar:

```
identifier ::= first_char { subsequent_char } ;

first_char ::= letter | '_' ;
subsequent_char ::= letter | digit | '_' ;

letter ::= 'A'..'Z' | 'a'..'z' ;
digit ::= '0'..'9' ;
```

They must start with a letter or underscore and be followed by a sequence of letters, digits, or underscores.


### 1.2 Whitespace

Whitespace is used to separate characters in source code but are otherwise ignored by the lexer.


### 1.3 Comments

Aster supports single line comments only. 

- A line comment begins with `//` and lasts until the end of the line.
- Comments are treated as whitespace and are not part of the token stream.

```
// this is a comment
let x: i8 = 27 // this is also a comment
```


### 1.4 String Literals

String literals are defined with a starting `"` and ending `"` character.


### 1.5 Numeric Literals

Numeric literals are defined as a sequence of digits in any order.

Numeric literals can contain a single `.` to denote a floating point number.

```
1 // this is a numeric literal
299792458 // this is also a numeric literal

42.2 // this is a floating point numeric literal
``` 


### 1.6 Primitive Types

Aster supports for the following primitive types to their C equivalent.

| Type | C Type |
|------|--------|
|i8|signed char|
|u8|unsigned char|
|i16|signed short|
|u16|unsigned short|
|i32|signed int|
|u32|unsigned int|
|i64|signed long|
|u64|unsigned long|
|f32|float|
|f64|double|
|u0|void|
|rawptr|void*|
|bool|_Bool|
|size|size_t|


### 1.7 Operators

Aster supports the following operators.

```
&
*
+
-
<
>
>=
<=
!=
==
and
&&
or
||
not
!
/
mod
%
sizeof
|
~
<<
>>
xor
^
as
```

The precedence for the tokenization of operators depends on the length of the operator token. Longer tokens naturally have a higher precedence than shorter ones.

```
== // parsed as '==', not '=', '='
```


### 1.8 Precedence

| Precedence | Operator |
|------|--------|
|1|Grouping Expression|
|2|Property Accessor|
|3|Dereference, Address-Of, Sizeof|
|4|Logical Not, Plus Unary, Negate, Bitwise Not|
|5|Cast operator (As)|
|6|Multiply, Divide, Modulo|
|7|Plus, Minus|
|8|Bitwise Shifts|
|9|Relational Comparison (>=, >, <= <)|
|10|Relational Equality (== and !=)|
|11|Bitwise And|
|12|Bitwise Xor|
|13|Bitwise Or|
|14|Logical And|
|15|Logical Or|
|16|Ternary Conditional|


### 1.9 Keywords

The following are keywords in Aster and are compiler reserved. They cannot be used as identifiers.

```
and as
else enum
false fn for
if in interface
let
match mod
next not
or
pub
return
sizeof stop struct
then true
while
xor
```


## 2 Syntax

### 2.1 BNF Grammar

The following defines the formal grammar for the Aster programming language.

```
<program> ::= <statements>

<statements> ::= { <statement> }

<statement> ::= <for loop>
			  | <do loop>
			  | <variable declaration>
			  | <type definition>
			  | <assignment>
			  | <if block>
			  | <function declaration>
			  | <return statement>
			  | <use statement>
			  | <tag>
			  | <while alteration loop>
			  | <next statement>
			  | <halt statement>
			  | <defer statement>
			  | <defer statement block>
			  | <embed statement>

<use statement> ::= "use" <identifier>
<tag> ::= "@" <identifier>

<embed statement> ::= "embed" <newline> <any text> <newline> "end"

<defer statement> ::= "defer" <statement> <newline>
<defer statement block> ::= "defer" <newline> <statements> "end"

<variable declaration> ::= "let" <identifier> ":" <type> "=" <expression>

<while alteration loop> ::= "while" <expression> ":" <statements> "{" <statements> "}"

<for loop> ::= "for" <expression> "{" <statements> "}"
<do loop> ::= "do" <expression> "{" <statements> "}"
<until loop> ::= "until" <expression> "{" <statements> "}"
<return statement> ::= "return" <expression>

<next statement> ::= "next"
<stop statement> ::= "stop"

<if block> ::= "if" <expression> "{" <statements> <if tail> "}"
<if tail> ::= { <else if clause> } [ <else clause> ]
<else if clause> ::= "else if" <expression> "{" <statements> "}"
<else clause> ::= "else" <statements>

<function declaration> ::= [ "pub" ] "fn" <identifier> "(" <function parameter list> ")" ":" <type> "{" <statements> "}"

<expression> ::= <literal>
			   | <identifier>
			   | <function call>
			   | <binary expression>
			   | <unary operator>
			   | <ternary expression>
			   | <query expression>
			   | <member access>
			   | <index access>

<query expression> ::= <from clause> <where clauses> <select clause>
<from clause> ::= "from" <identifier> "in" <expression>
<where clauses> ::= { <where clause> }
<where clause> ::= "where" <expression>
<select clause> ::= "select" <expression>

<type definition> "type" <identifier> <statements> "end"

<member access> ::= <expression> "." <identifier>
<index access> ::= <expression> "[" <expression> "]"

<ternary expression> ::= "if" <expression> "then" <expression> "else" <expression>
<assignment> ::= <identifier> "=" <expression>
<function call> ::= <identifier> "(" { <function parameter list> } ")"
<function parameter list> ::= [ <function parameter> { "," <function parameter> } ]
<function parameter> ::= <identifier> ":" <type>

<unary expression> ::= <unary operator> <expression>
<unary operator> ::= "!" | "+" | "-" | "~" | "&" | "*" | "not"

<binary operator> ::= "+"  | "-"  | "*"  | "/"  | "%"
                    | "==" | "!=" | "<" | ">" | "<=" | ">="
                    | "="  | "+=" | "-=" | "*=" | "/=" | "%="
                    | "&"  | "|"  | "^"  | "&=" | "|=" | "^="
                    | "&&" | "||"
                    | "<<" | ">>" | "<<=" | ">>="
                    | "**" | "**=" | "**%"
                    | "//"

<type> ::= <primitive type> | <identifier>
<primitive type> ::= "u0" | "i8" | "u8" | "i16" | "u16" | "i32" | "u32" | "i64" | "u64" | "bool" | "rawptr" | "f32" | "f64" | "size"

<literal> ::= <integer> | <boolean> | <string> | <char> | <float>
<float> ::= <digits> "." <digits>
<integer> ::= 0..9
<boolean> ::= true | false
<string> ::= "\""<any text>"\""
<char> ::= "'"<letter>"'"

<newline> ::= "\n"
<identifier> ::= letter | { letter | digit | "_" }
<digits> ::= { <digit> }
<digit> ::= "0".."9"
<letter> ::= "a".."z" | "A".."Z"
```


## 3 Language Behaviour

