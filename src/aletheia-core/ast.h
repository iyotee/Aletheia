/*
 * ALETHEIA-Core: Simplified AST
 */

#ifndef AST_H
#define AST_H

#include "core.h"

/* AST Node Types (simplified for bootstrap) */
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DEF,
    AST_VAR_DECL,
    AST_RETURN_STMT,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_BLOCK,
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_ASSIGN_EXPR,
    AST_FUNCTION_CALL,
    AST_IDENTIFIER,
    AST_INTEGER_LITERAL,
    AST_STRING_LITERAL,
} ASTNodeType;

/* Type information (simplified) */
typedef enum {
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_PTR,
} TypeKind;

typedef struct {
    TypeKind kind;
    int size;
    struct TypeInfo* base; /* For pointers */
} TypeInfo;

/* AST Node */
typedef struct ASTNode {
    ASTNodeType type;
    TypeInfo* node_type;

    union {
        /* Program */
        struct {
            struct ASTNode** declarations;
            int decl_count;
        } program;

        /* Function definition */
        struct {
            char* name;
            struct ASTNode** params;
            int param_count;
            TypeInfo* return_type;
            struct ASTNode* body;
        } func_def;

        /* Variable declaration */
        struct {
            char* name;
            TypeInfo* var_type;
            struct ASTNode* initializer;
        } var_decl;

        /* Return statement */
        struct ASTNode* return_expr;

        /* If statement */
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;

        /* While statement */
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_stmt;

        /* Block */
        struct {
            struct ASTNode** statements;
            int stmt_count;
        } block;

        /* Binary expression */
        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;

        /* Unary expression */
        struct {
            char op;
            struct ASTNode* operand;
        } unary;

        /* Assignment */
        struct {
            struct ASTNode* target;
            struct ASTNode* value;
        } assign;

        /* Function call */
        struct {
            char* name;
            struct ASTNode** args;
            int arg_count;
        } call;

        /* Literals */
        char* identifier;
        int int_value;
        char* str_value;
    } data;
} ASTNode;

/* Functions */
ASTNode* create_ast_node(ASTNodeType type);
void free_ast_node(ASTNode* node);

TypeInfo* create_type(TypeKind kind);
TypeInfo* create_pointer_type(TypeInfo* base);
void free_type(TypeInfo* type);

#endif /* AST_H */

