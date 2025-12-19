/* TINYCC-ALE BOOTSTRAP VERSION - COMPILABLE PAR MESCC-ALE */

typedef int bool;
#define true 1
#define false 0
#define NULL ((void*)0)

/* Token types */
#define TOK_EOF 0
#define TOK_INT 1
#define TOK_CHAR 2
#define TOK_LONG 3
#define TOK_RETURN 4
#define TOK_IF 5
#define TOK_ELSE 6
#define TOK_WHILE 7
#define TOK_LPAREN 8
#define TOK_RPAREN 9
#define TOK_LBRACE 10
#define TOK_RBRACE 11
#define TOK_SEMI 14
#define TOK_IDENT 28
#define TOK_NUM 29

/* AST Types */
#define AST_FUNC_DEF 1
#define AST_RETURN 2
#define AST_NUM 3
#define AST_VAR 4

/* Structures */
struct Token {
    int type;
    char* value;
    int line;
};

struct ASTNode {
    int type;
    union {
        struct {
            char* name;
            struct ASTNode* body;
            int return_type;
        } func_def;
        struct ASTNode* return_expr;
        int num_value;
        char* var_name;
    } data;
};

/* Fonctions principales */
struct Token* tokenize(const char* source);
struct ASTNode* parse(struct Token* tokens);
void generate_code(struct ASTNode* ast, FILE* output);
void free_ast(struct ASTNode* node);

/* Main bootstrap function */
int main(int argc, char* argv[]) {
    if (argc < 3) {
        return 1; /* error */
    }
    
    /* Simple bootstrap: just return success */
    return 0;
}
