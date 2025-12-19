# ALETHEIA-Core: Bootstrap Compiler

ALETHEIA-Core is a simplified, bootstrap-compatible version of the ALETHEIA C compiler that can be compiled by TinyCC-ALE.

## Purpose

ALETHEIA-Core serves as the bridge between TinyCC-ALE and the full ALETHEIA compiler. It implements the core compilation pipeline while being compatible with TinyCC-ALE's limitations.

## Key Differences from Full ALETHEIA

- **Simplified AST**: Reduced node types for bootstrap compatibility
- **Limited optimizations**: Basic optimizations only
- **No advanced diagnostics**: Basic error reporting
- **Compatible types**: Only types supported by TinyCC-ALE
- **Minimal dependencies**: No external libraries

## Architecture

```
ALETHEIA-Core/
├── core.h          # Core compiler interface
├── lexer.c/.h      # Simplified lexer
├── parser.c/.h     # Recursive descent parser
├── ast.c/.h        # Simplified AST
├── codegen.c/.h    # x86-64 code generation
├── symbol.c/.h     # Symbol table
└── main.c          # Entry point
```

## Bootstrap Chain

```
Stage0-ALE → MesCC-ALE → TinyCC-ALE → ALETHEIA-Core → ALETHEIA-Full
```

## Building

```bash
# Compile with TinyCC-ALE
./tinycc-ale src/aletheia-core/*.c -o aletheia-core

# Test compilation
./aletheia-core test.c -o test
```

