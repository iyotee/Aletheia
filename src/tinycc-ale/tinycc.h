/*
 * ALETHEIA TinyCC-ALE: Tiny C Compiler for ALETHEIA Bootstrap
 *
 * A simplified C compiler that can compile ALETHEIA-Core.
 * Built on top of MesCC-ALE capabilities.
 *
 * Supports: int, char, long, pointers, basic structs, arrays
 * Can be compiled by MesCC-ALE and compile itself.
 */

#ifndef TINYCC_H
#define TINYCC_H

// Extended token types for TinyCC-ALE
typedef enum {
    TOK_EOF = 0,
    TOK_INT, TOK_CHAR, TOK_LONG,      // Types étendus
    TOK_RETURN,
    TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,       // Pour arrays
    TOK_SEMI, TOK_COMMA,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQUAL, TOK_EQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_ADDR, TOK_DOT,                // Pour pointeurs et structs
    TOK_IDENT, TOK_NUM, TOK_STR,      // String literals ajoutés
} TinyTokenType;

// Token structure for TinyCC-ALE
typedef struct {
    TinyTokenType type;
    char* value;
    int line;
} TinyToken;

// Extended AST node types
typedef enum {
    AST_FUNC_DEF,
    AST_FUNC_CALL,
    AST_RETURN,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
    AST_IF, AST_WHILE, AST_BLOCK,
    AST_VAR_DECL,
    AST_PARAM_LIST,
    AST_DEREF, AST_ADDR,
    AST_ARRAY_ACCESS,                 // Pour arrays
    AST_MEMBER_ACCESS,                // Pour structs
    AST_NUM, AST_VAR, AST_STR,
    AST_CAST,                         // Casts de types
} TinyASTType;

// Type system for TinyCC-ALE
typedef enum {
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_LONG,
    TYPE_PTR,                        // Pointeurs génériques
    TYPE_ARRAY,                      // Arrays
    TYPE_STRUCT,                     // Structs (simplifiées)
} TinyTypeKind;

// Type information
typedef struct TinyType {
    TinyTypeKind kind;
    int size;                        // Size in bytes
    struct TinyType* base;           // For pointers/arrays
    char* struct_name;               // For structs
} TinyType;

// Extended AST nodes
typedef struct TinyASTNode {
    TinyASTType type;
    TinyType* ast_type;              // Type information
    union {
        // Function definition
        struct {
            char* name;
            struct TinyASTNode* params;
            struct TinyASTNode* body;
            TinyType* return_type;
        } func_def;

        // Function call
        struct {
            char* name;
            struct TinyASTNode** args;
            int arg_count;
        } func_call;

        // Return statement
        struct TinyASTNode* return_expr;

        // Binary operation
        struct {
            char op;
            struct TinyASTNode* left;
            struct TinyASTNode* right;
        } binary;

        // Assignment
        struct {
            char* var_name;
            struct TinyASTNode* value;
        } assignment;

        // If statement
        struct {
            struct TinyASTNode* condition;
            struct TinyASTNode* then_branch;
            struct TinyASTNode* else_branch;
        } if_stmt;

        // While statement
        struct {
            struct TinyASTNode* condition;
            struct TinyASTNode* body;
        } while_stmt;

        // Block
        struct {
            struct TinyASTNode** statements;
            int statement_count;
        } block;

        // Variable declaration
        struct {
            char* var_name;
            TinyType* var_type;
            struct TinyASTNode* initializer;
        } var_decl;

        // Parameter list
        struct {
            char** param_names;
            TinyType** param_types;
            int param_count;
        } param_list;

        // Dereference (*ptr)
        struct TinyASTNode* deref_expr;

        // Address-of (&var)
        char* addr_var_name;

        // Array access
        struct {
            struct TinyASTNode* array;
            struct TinyASTNode* index;
        } array_access;

        // Cast
        struct {
            TinyType* cast_type;
            struct TinyASTNode* expr;
        } cast;

        // Literals
        int num_value;
        char* str_value;
        char* var_name;
    } data;
} TinyASTNode;

// Symbol table entry with types
typedef struct TinySymbol {
    char* name;
    TinyType* type;
    int offset;  // Stack offset
} TinySymbol;

// Symbol table
typedef struct {
    TinySymbol* symbols;
    int count;
    int capacity;
} TinySymbolTable;

// Function declarations
TinyToken* tiny_tokenize(const char* source);
TinyASTNode* tiny_parse(TinyToken* tokens);
void tiny_generate_code(TinyASTNode* ast, FILE* output, TinySymbolTable* symtab);
void tiny_free_ast(TinyASTNode* node);
void tiny_free_symbol_table(TinySymbolTable* symtab);

// Type system functions
TinyType* tiny_make_type(TinyTypeKind kind);
TinyType* tiny_make_ptr_type(TinyType* base);
TinyType* tiny_make_array_type(TinyType* base, int size);
void tiny_free_type(TinyType* type);

#endif // TINYCC_H
