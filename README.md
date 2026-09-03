# byteVM

`byteVM` is a compiled programming language implementation written in C11.

The project features a **hybrid architecture with a bytecode compiler and a call-stack based Virtual Machine (VM)**, closely resembling real-world language runtimes like CPython, Lua, and the JVM.

## Key Features

### 1. Functions & Recursion
- First-class function declarations using the `fn` keyword
- Support for multiple parameters and `return` statements
- Full support for **recursion** and nested function calls via Call Frames (`CallFrame`) in the VM

```text
fn factorial(n) {
    if (n < 2) {
        return 1;
    }
    return n * factorial(n - 1);
}

print factorial(5); // Output: 120
```

### 2. Variables & Scoping
- Explicit variable declarations using the `var` keyword (`var x = 10;`, `var y;`)
- Lexical block scoping with standalone `{}` blocks
- Redeclaration protection and variable shadowing support

```text
var x = 10;
{
    var x = 20;
    print x; // Output: 20
}
print x; // Output: 10
```

### 3. Control Flow
- Conditionals: `if` / `else` / `else if` statements
- Loops: `while` loops
- Control statements: `break` and `continue` (with automatic VM stack unwinding for block-local variables)

```text
var i = 0;
while (i < 5) {
    i = i + 1;
    if (i == 3) {
        continue;
    }
    print i;
}
```

### 4. Expression Evaluation & Data Types
- **Data Types**: Integers, Booleans (`true`, `false`), Strings
- **Operators**:
  - Arithmetic: `+`, `-`, `*`, `/`
  - Relational: `>`, `<`, `>=`, `<=`, `==`, `!=`
  - Logical: `&&`, `||`
- Built-in `print` statement

---

## Architecture & Pipeline

```
                 Source Code
                      ↓
          Lexer (characters → tokens)
                      ↓
       Parser (tokens → AST via recursive descent)
                      ↓
        Code Generator (AST → Bytecode Chunk)
                      ↓
   Virtual Machine (Stack & CallFrame Stack Execution)
                      ↓
                 Runtime Output
```

### System Breakdown

- **Lexer (`src/syntax/lexer.c`)** → Converts raw source code into tokens.
- **Parser (`src/syntax/parser.c`)** → Builds an Abstract Syntax Tree (AST) using recursive descent with precedence climbing.
- **AST (`src/syntax/ast.c`)** → Structured representations of AST nodes (functions, blocks, expressions, control flow).
- **Code Generator (`src/compiler/codegen.c`)** → Compiles AST nodes into VM bytecode chunks, managing scope-aware variable lookup.
- **Symbol Table (`src/compiler/symbol_table.c`)** → Manages lexical scopes, tracking local variable stack slots and global variable offsets.
- **Bytecode & Chunk (`src/runtime/chunk.c`)** → Stores instructions, opcodes, and constant pools.
- **Virtual Machine (`src/runtime/vm.c`)** → Executes bytecode instructions using a stack-based model with `CallFrame` isolation (`OP_CALL`, `OP_RETURN`).

---

## EBNF Grammar

```ebnf
program        = { statement } ;
statement      = func_decl | var_decl | assignment | print_stmt | if_stmt | while_stmt | break_stmt | continue_stmt | return_stmt | block ;
func_decl      = "fn" identifier "(" [ identifier { "," identifier } ] ")" block ;
var_decl       = "var" identifier [ "=" logical_or ] ";" ;
assignment     = identifier "=" logical_or ";" ;
print_stmt     = "print" logical_or ";" ;
if_stmt        = "if" "(" logical_or ")" block [ "else" ( if_stmt | block ) ] ;
while_stmt     = "while" "(" logical_or ")" block ;
block          = "{" { statement } "}" ; 
break_stmt     = "break" ";" ;
continue_stmt  = "continue" ";" ;
return_stmt    = "return" [ logical_or ] ";" ;
logical_or     = logical_and { "||" logical_and } ;
logical_and    = comparison { "&&" comparison } ;
comparison     = additive { ("==" | "!=" | "<" | ">" | "<=" | ">=") additive } ;
additive       = term { ("+" | "-") term } ;
term           = factor { ("*" | "/") factor } ;
factor         = identifier "(" [ logical_or { "," logical_or } ] ")" | identifier | number | string | boolean | "(" logical_or ")" ;
boolean        = "true" | "false" ;
```

---

## Building and Running

### Prerequisites
- C11 compatible compiler (`gcc`, `clang`, or MSVC)
- CMake 3.10 or higher

### Build

1. Clone the repository:
   ```bash
   git clone https://github.com/jashhh0908/byteVM.git
   cd byteVM
   ```

2. Build using CMake:
   ```bash
   cmake -B build
   cmake --build build
   ```

### Execute Code

Run any script by passing the source file path to the executable:

**Windows (PowerShell / Command Prompt):**
```powershell
.\build\byteVM.exe tests/recursion_test.txt
```

**Linux / macOS:**
```bash
./build/byteVM tests/recursion_test.txt
```