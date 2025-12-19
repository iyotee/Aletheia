/*
 * ALETHEIA TinyCC-ALE: Parser
 *
 * Extended parser supporting more C features.
 * Builds on MesCC-ALE capabilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinycc.h"

// Simple string duplication function
static char* my_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

static char* tiny_strdup(const char* s) {
    return my_strdup(s);
}

// Global token stream
static TinyToken* global_tokens;
static int global_token_pos;

// Helper functions
static TinyToken* current_token() {
    return &global_tokens[global_token_pos];
}

static void advance() {
    global_token_pos++;
}

static int expect(TinyTokenType type) {
    if (current_token()->type == type) {
        advance();
        return 1;
    }
    return 0;
}

// Forward declarations
static TinyASTNode* parse_expression();
static TinyASTNode* parse_term();
static TinyASTNode* parse_factor();
static TinyASTNode* parse_statement();
static TinyType* parse_type_specifier();

// Create AST node
static TinyASTNode* create_node(TinyASTType type) {
    TinyASTNode* node = malloc(sizeof(TinyASTNode));
    node->type = type;
    node->ast_type = NULL;
    return node;
}

// Create type
TinyType* tiny_make_type(TinyTypeKind kind) {
    TinyType* type = malloc(sizeof(TinyType));
    type->kind = kind;
    type->size = 4; // Default to int size
    type->base = NULL;
    type->struct_name = NULL;

    switch (kind) {
        case TYPE_CHAR: type->size = 1; break;
        case TYPE_INT: type->size = 4; break;
        case TYPE_LONG: type->size = 8; break;
        case TYPE_PTR: type->size = 8; break; // 64-bit pointers
        default: break;
    }

    return type;
}

TinyType* tiny_make_ptr_type(TinyType* base) {
    TinyType* type = tiny_make_type(TYPE_PTR);
    type->base = base;
    return type;
}

TinyType* tiny_make_array_type(TinyType* base, int size) {
    TinyType* type = tiny_make_type(TYPE_ARRAY);
    type->base = base;
    type->size = base->size * size;
    return type;
}

void tiny_free_type(TinyType* type) {
    if (!type) return;
    if (type->base) tiny_free_type(type->base);
    if (type->struct_name) free(type->struct_name);
    free(type);
}

// Parse type specifier
static TinyType* parse_type_specifier() {
    if (current_token()->type == TOK_INT) {
        advance();
        return tiny_make_type(TYPE_INT);
    } else if (current_token()->type == TOK_CHAR) {
        advance();
        return tiny_make_type(TYPE_CHAR);
    } else if (current_token()->type == TOK_LONG) {
        advance();
        return tiny_make_type(TYPE_LONG);
    }

    return NULL;
}

// Parse type (with pointers)
static TinyType* parse_type() {
    TinyType* base_type = parse_type_specifier();
    if (!base_type) return NULL;

    // Handle pointers
    while (current_token()->type == TOK_STAR) {
        advance();
        base_type = tiny_make_ptr_type(base_type);
    }

    return base_type;
}

// parse_type is defined above

// Parse expression (extended)
static TinyASTNode* parse_expression() {
    return parse_term();
}

// parse_comparison removed for now - will be added back later

static TinyASTNode* parse_term() {
    TinyASTNode* left = parse_factor();

    while (current_token()->type == TOK_PLUS ||
           current_token()->type == TOK_MINUS) {
        char op = current_token()->value[0];
        advance();

        TinyASTNode* right = parse_factor();

        TinyASTNode* binary = create_node(AST_BINARY_OP);
        binary->data.binary.op = op;
        binary->data.binary.left = left;
        binary->data.binary.right = right;

        left = binary;
    }

    return left;
}

static TinyASTNode* parse_factor() {
    if (current_token()->type == TOK_NUM) {
        TinyASTNode* node = create_node(AST_NUM);
        node->data.num_value = atoi(current_token()->value);
        node->ast_type = tiny_make_type(TYPE_INT);
        advance();
        return node;
    }

    if (current_token()->type == TOK_STR) {
        TinyASTNode* node = create_node(AST_STR);
        node->data.str_value = tiny_strdup(current_token()->value);
        // String literals are char arrays
        node->ast_type = tiny_make_array_type(tiny_make_type(TYPE_CHAR),
                                             strlen(current_token()->value) + 1);
        advance();
        return node;
    }

    if (current_token()->type == TOK_IDENT) {
        TinyToken* ident_token = current_token();
        advance();

        // Check if it's a function call
        if (current_token()->type == TOK_LPAREN) {
            // For now, keep it simple - same as MesCC-ALE
            TinyASTNode* call = create_node(AST_FUNC_CALL);
            call->data.func_call.name = tiny_strdup(ident_token->value);
            call->data.func_call.args = NULL;
            call->data.func_call.arg_count = 0;

            expect(TOK_LPAREN);
            expect(TOK_RPAREN);

            return call;
        } else {
            // Variable reference
            TinyASTNode* node = create_node(AST_VAR);
            node->data.var_name = tiny_strdup(ident_token->value);
            return node;
        }
    }

    // Dereference (*ptr)
    if (current_token()->type == TOK_STAR) {
        advance();
        TinyASTNode* expr = parse_factor();
        if (!expr) return NULL;

        TinyASTNode* deref = create_node(AST_DEREF);
        deref->data.deref_expr = expr;
        return deref;
    }

    // Address-of (&var)
    if (current_token()->type == TOK_ADDR) {
        advance();
        if (current_token()->type != TOK_IDENT) {
            fprintf(stderr, "Expected identifier after '&' at line %d\n",
                    current_token()->line);
            return NULL;
        }

        TinyASTNode* addr = create_node(AST_ADDR);
        addr->data.addr_var_name = tiny_strdup(current_token()->value);
        advance();
        return addr;
    }

    if (expect(TOK_LPAREN)) {
        TinyASTNode* expr = parse_expression();
        expect(TOK_RPAREN);
        return expr;
    }

    fprintf(stderr, "Expected expression at line %d\n", current_token()->line);
    return NULL;
}

// Parse statement (extended)
static TinyASTNode* parse_statement() {
    TinyTokenType type = current_token()->type;

    // Variable declaration (extended)
    if (type == TOK_INT || type == TOK_CHAR || type == TOK_LONG) {
        TinyType* var_type = parse_type();
        if (!var_type) return NULL;

        if (current_token()->type != TOK_IDENT) {
            fprintf(stderr, "Expected variable name at line %d\n", current_token()->line);
            tiny_free_type(var_type);
            return NULL;
        }

        char* var_name = tiny_strdup(current_token()->value);
        advance();

        TinyASTNode* initializer = NULL;
        if (current_token()->type == TOK_EQUAL) {
            advance();
            initializer = parse_expression();
        }

        if (!expect(TOK_SEMI)) {
            fprintf(stderr, "Expected ';' after variable declaration at line %d\n",
                    current_token()->line);
            tiny_free_type(var_type);
            free(var_name);
            tiny_free_ast(initializer);
            return NULL;
        }

        TinyASTNode* var_decl = create_node(AST_VAR_DECL);
        var_decl->data.var_decl.var_name = var_name;
        var_decl->data.var_decl.var_type = var_type;
        var_decl->data.var_decl.initializer = initializer;
        return var_decl;
    }

    // Handle other statement types (similar to MesCC-ALE)
    if (type == TOK_IF) {
        // For now, delegate to MesCC-ALE style parsing
        // This is a placeholder - we'll extend this
        fprintf(stderr, "IF statements not yet implemented in TinyCC-ALE\n");
        return NULL;
    }

    if (type == TOK_RETURN) {
        if (!expect(TOK_RETURN)) return NULL;

        TinyASTNode* expr = parse_expression();

        if (!expect(TOK_SEMI)) {
            tiny_free_ast(expr);
            return NULL;
        }

        TinyASTNode* ret = create_node(AST_RETURN);
        ret->data.return_expr = expr;
        return ret;
    }

    fprintf(stderr, "Unsupported statement type at line %d\n", current_token()->line);
    return NULL;
}

// Parse function definition (extended)
static TinyASTNode* parse_function_definition() {
    // Parse return type
    TinyType* return_type = parse_type_specifier();
    if (!return_type) return NULL;

    if (current_token()->type != TOK_IDENT) {
        fprintf(stderr, "Expected function name at line %d\n", current_token()->line);
        tiny_free_type(return_type);
        return NULL;
    }

    char* func_name = tiny_strdup(current_token()->value);
    advance();

    // Parse parameters (simplified for now)
    if (!expect(TOK_LPAREN) || !expect(TOK_RPAREN)) {
        tiny_free_type(return_type);
        free(func_name);
        return NULL;
    }

    // Parse body
    if (!expect(TOK_LBRACE)) {
        tiny_free_type(return_type);
        free(func_name);
        return NULL;
    }

    // For now, parse a simple body (just return statement)
    TinyASTNode* body = parse_statement();

    if (!expect(TOK_RBRACE)) {
        tiny_free_type(return_type);
        free(func_name);
        tiny_free_ast(body);
        return NULL;
    }

    TinyASTNode* func_def = create_node(AST_FUNC_DEF);
    func_def->data.func_def.name = func_name;
    func_def->data.func_def.params = NULL; // No params for now
    func_def->data.func_def.body = body;
    func_def->data.func_def.return_type = return_type;

    return func_def;
}

// End of parser functions

// Free AST (extended)
void tiny_free_ast(TinyASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_FUNC_DEF:
            free(node->data.func_def.name);
            tiny_free_ast(node->data.func_def.params);
            tiny_free_ast(node->data.func_def.body);
            tiny_free_type(node->data.func_def.return_type);
            break;
        case AST_FUNC_CALL:
            free(node->data.func_call.name);
            for (int i = 0; i < node->data.func_call.arg_count; i++) {
                tiny_free_ast(node->data.func_call.args[i]);
            }
            free(node->data.func_call.args);
            break;
        case AST_RETURN:
            tiny_free_ast(node->data.return_expr);
            break;
        case AST_BINARY_OP:
            tiny_free_ast(node->data.binary.left);
            tiny_free_ast(node->data.binary.right);
            break;
        case AST_ASSIGNMENT:
            free(node->data.assignment.var_name);
            tiny_free_ast(node->data.assignment.value);
            break;
        case AST_VAR_DECL:
            free(node->data.var_decl.var_name);
            tiny_free_type(node->data.var_decl.var_type);
            tiny_free_ast(node->data.var_decl.initializer);
            break;
        case AST_DEREF:
            tiny_free_ast(node->data.deref_expr);
            break;
        case AST_ADDR:
            free(node->data.addr_var_name);
            break;
        case AST_STR:
            free(node->data.str_value);
            break;
        case AST_VAR:
            free(node->data.var_name);
            break;
        case AST_NUM:
        case AST_CAST:
            // Nothing special to free
            break;
    }

    if (node->ast_type) tiny_free_type(node->ast_type);
    free(node);
}

// Free symbol table
void tiny_free_symbol_table(TinySymbolTable* symtab) {
    for (int i = 0; i < symtab->count; i++) {
        free(symtab->symbols[i].name);
        tiny_free_type(symtab->symbols[i].type);
    }
    free(symtab->symbols);
    symtab->symbols = NULL;
    symtab->count = 0;
    symtab->capacity = 0;
}

// Main parse function
TinyASTNode* tiny_parse(TinyToken* tokens) {
    global_tokens = tokens;
    global_token_pos = 0;

    return parse_function_definition();
}
