

#ifndef TINYCC_H
#define TINYCC_H


typedef enum {
    TOK_EOF = 0,
    TOK_INT, TOK_CHAR, TOK_LONG,      
    TOK_RETURN,
    TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,       
    TOK_SEMI, TOK_COMMA,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQUAL, TOK_EQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_ADDR, TOK_DOT,                
    TOK_IDENT, TOK_NUM, TOK_STR,      
} TinyTokenType;


typedef struct {
    TinyTokenType type;
    char* value;
    int line;
} TinyToken;


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
    AST_ARRAY_ACCESS,                 
    AST_MEMBER_ACCESS,                
    AST_NUM, AST_VAR, AST_STR,
    AST_CAST,                         
} TinyASTType;


typedef enum {
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_LONG,
    TYPE_PTR,                        
    TYPE_ARRAY,                      
    TYPE_STRUCT,                     
} TinyTypeKind;


typedef struct TinyType {
    TinyTypeKind kind;
    int size;                        
    struct TinyType* base;           
    char* struct_name;               
} TinyType;


typedef struct TinyASTNode {
    TinyASTType type;
    TinyType* ast_type;              
    union {
        
        struct {
            char* name;
            struct TinyASTNode* params;
            struct TinyASTNode* body;
            TinyType* return_type;
        } func_def;

        
        struct {
            char* name;
            struct TinyASTNode** args;
            int arg_count;
        } func_call;

        
        struct TinyASTNode* return_expr;

        
        struct {
            char op;
            struct TinyASTNode* left;
            struct TinyASTNode* right;
        } binary;

        
        struct {
            char* var_name;
            struct TinyASTNode* value;
        } assignment;

        
        struct {
            struct TinyASTNode* condition;
            struct TinyASTNode* then_branch;
            struct TinyASTNode* else_branch;
        } if_stmt;

        
        struct {
            struct TinyASTNode* condition;
            struct TinyASTNode* body;
        } while_stmt;

        
        struct {
            struct TinyASTNode** statements;
            int statement_count;
        } block;

        
        struct {
            char* var_name;
            TinyType* var_type;
            struct TinyASTNode* initializer;
        } var_decl;

        
        struct {
            char** param_names;
            TinyType** param_types;
            int param_count;
        } param_list;

        
        struct TinyASTNode* deref_expr;

        
        char* addr_var_name;

        
        struct {
            struct TinyASTNode* array;
            struct TinyASTNode* index;
        } array_access;

        
        struct {
            TinyType* cast_type;
            struct TinyASTNode* expr;
        } cast;

        
        int num_value;
        char* str_value;
        char* var_name;
    } data;
} TinyASTNode;


typedef struct TinySymbol {
    char* name;
    TinyType* type;
    int offset;  
} TinySymbol;


typedef struct {
    TinySymbol* symbols;
    int count;
    int capacity;
} TinySymbolTable;


TinyToken* tiny_tokenize(const char* source);
TinyASTNode* tiny_parse(TinyToken* tokens);
void tiny_generate_code(TinyASTNode* ast, FILE* output, TinySymbolTable* symtab);
void tiny_free_ast(TinyASTNode* node);
void tiny_free_symbol_table(TinySymbolTable* symtab);


TinyType* tiny_make_type(TinyTypeKind kind);
TinyType* tiny_make_ptr_type(TinyType* base);
TinyType* tiny_make_array_type(TinyType* base, int size);
void tiny_free_type(TinyType* type);

#endif 
