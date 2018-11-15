# Notes

### Usage
The parser can be plugged into for diagnosing by reimplementing the cin redirect from hw2.

Look at `./cases/*.correct` in the cases from hw2 to get a gist of how the lexer spits out tokens. The token return type is the token type in tokens.h

```
SYNOPSIS: ./prog3 [filename]
Run with no args. Enter tokens as the return type of the token from lex.cpp
^d to return or SIGQUIT + to exit.
Index:
    Primaries:
        PLUS
        MINUS
        STAR
        SLASH
    Quantities:
        LPAREN
        RPAREN
    Int:
        ICONST([1-.*])
    "String":
        SCONST(.*)
    IDENT:
        IDENT(x)
    Bool:
        TRUE
        FALSE
    print:
        PRINT
    if:
        IF
    =:
        ASSIGN
    ;:
        SC
```

Notes:
I've been using EBNF for years and didn't even realize it
<https://unix.stackexchange.com/a/17847/54028>
