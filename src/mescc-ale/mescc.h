/*
 * ALETHEIA MesCC-ALE Phase 1: Minimal C Compiler Header
 *
 * Extremely minimal C compiler that can compile basic functions.
 * Designed to be verifiable by human audit.
 */

#ifndef MESCC_H
#define MESCC_H

// Token types for TinyCC-ALE compatibility
typedef enum {
    TOK_EOF = 0,
    TOK_INT, TOK_CHAR, TOK_LONG,        // Types étendus
    TOK_STRUCT,                          // Structures
    TOK_RETURN,
    TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,         // Pour tableaux
    TOK_SEMI, TOK_COMMA,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQUAL, TOK_EQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_ADDR, TOK_DOT,                   // Pointeurs et structs
    TOK_IDENT, TOK_NUM, TOK_STR,         // Identifiants, nombres, chaînes
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
    int line;
} Token;

// AST node types (Phase 3)
typedef enum {
    AST_FUNC_DEF,
    AST_FUNC_CALL,
    AST_RETURN,
    AST_BINARY_OP,
    AST_ASSIGNMENT,
    AST_IF,
    AST_WHILE,
    AST_BLOCK,
    AST_VAR_DECL,
    AST_PARAM_LIST,
    AST_DEREF,      // *ptr
    AST_ADDR,       // &var
    AST_NUM,
    AST_VAR
} ASTType;

// AST node structure (Phase 3)
typedef struct ASTNode {
    ASTType type;
    union {
        // Function definition
        struct {
            char* name;
            struct ASTNode* params;  // Parameter list
            struct ASTNode* body;
        } func_def;

        // Function call
        struct {
            char* name;
            struct ASTNode** args;   // Arguments
            int arg_count;
        } func_call;

        // Return statement
        struct ASTNode* return_expr;

        // Binary operation
        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;

        // Assignment
        struct {
            char* var_name;
            struct ASTNode* value;
        } assignment;

        // If statement
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;

        // While statement
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_stmt;

        // Block (sequence of statements)
        struct {
            struct ASTNode** statements;
            int statement_count;
        } block;

        // Variable declaration
        struct {
            char* var_name;
            struct ASTNode* initializer;
        } var_decl;

        // Parameter list
        struct {
            char** param_names;
            int param_count;
        } param_list;

        // Dereference (*ptr)
        struct ASTNode* deref_expr;

        // Address-of (&var)
        char* addr_var_name;

        // Number literal
        int num_value;

        // Variable reference
        char* var_name;
    } data;
} ASTNode;

// Symbol table entry
typedef struct Symbol {
    char* name;
    int offset;  // Stack offset from RBP
} Symbol;

// Symbol table
typedef struct {
    Symbol* symbols;
    int count;
    int capacity;
} SymbolTable;

// Function declarations (Phase 2)
Token* tokenize(const char* source);
ASTNode* parse(Token* tokens);
void generate_code(ASTNode* ast, FILE* output, SymbolTable* symtab);
void free_ast(ASTNode* node);
void free_symbol_table(SymbolTable* symtab);

#endif // MESCC_H
