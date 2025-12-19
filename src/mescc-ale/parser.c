/*
 * ALETHEIA MesCC-ALE Phase 1: Parser
 *
 * Extremely simple recursive descent parser for basic C functions.
 * Supports: int func() { return expr; }
 * Where expr is: number | identifier | expr + expr | expr - expr | (expr)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mescc.h"

// Simple string duplication function
static char* my_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

// Forward declaration for token_type_name from lexer.c
const char* token_type_name(TokenType type);

// Forward declarations for parser functions
static ASTNode* parse_function_definition();
static int parse_struct_declaration();

// Global token stream and current position
static Token* tokens;
static int token_pos;

// Peek at next token without advancing
static Token* peek_token() {
    if (token_pos + 1 < /* some max */ 10000) { // TODO: proper bounds check
        return &tokens[token_pos + 1];
    }
    return NULL;
}

// Create AST node
static ASTNode* create_node(ASTType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

// Free AST (Phase 3)
void free_ast(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_FUNC_DEF:
            free(node->data.func_def.name);
            free_ast(node->data.func_def.params);
            free_ast(node->data.func_def.body);
            break;
        case AST_FUNC_CALL:
            free(node->data.func_call.name);
            for (int i = 0; i < node->data.func_call.arg_count; i++) {
                free_ast(node->data.func_call.args[i]);
            }
            free(node->data.func_call.args);
            break;
        case AST_RETURN:
            free_ast(node->data.return_expr);
            break;
        case AST_BINARY_OP:
            free_ast(node->data.binary.left);
            free_ast(node->data.binary.right);
            break;
        case AST_ASSIGNMENT:
            free(node->data.assignment.var_name);
            free_ast(node->data.assignment.value);
            break;
        case AST_IF:
            free_ast(node->data.if_stmt.condition);
            free_ast(node->data.if_stmt.then_branch);
            if (node->data.if_stmt.else_branch)
                free_ast(node->data.if_stmt.else_branch);
            break;
        case AST_WHILE:
            free_ast(node->data.while_stmt.condition);
            free_ast(node->data.while_stmt.body);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->data.block.statement_count; i++) {
                free_ast(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
        case AST_VAR_DECL:
            free(node->data.var_decl.var_name);
            if (node->data.var_decl.initializer)
                free_ast(node->data.var_decl.initializer);
            break;
        case AST_PARAM_LIST:
            for (int i = 0; i < node->data.param_list.param_count; i++) {
                free(node->data.param_list.param_names[i]);
            }
            free(node->data.param_list.param_names);
            break;
        case AST_DEREF:
            free_ast(node->data.deref_expr);
            break;
        case AST_ADDR:
            free(node->data.addr_var_name);
            break;
        case AST_VAR:
            free(node->data.var_name);
            break;
        case AST_NUM:
            // Nothing to free
            break;
    }
    free(node);
}

// Forward declarations (Phase 3)
static ASTNode* parse_expression();
static ASTNode* parse_comparison();
static ASTNode* parse_term();
static ASTNode* parse_factor();
static ASTNode* parse_statement();
static ASTNode* parse_if_statement();
static ASTNode* parse_while_statement();
static ASTNode* parse_variable_declaration();
static ASTNode* parse_function_call();
static ASTNode* parse_parameter_list();
static ASTNode* parse_function_call_with_name(const char* func_name);

// Get current token
static Token* current_token() {
    return &tokens[token_pos];
}

// Advance to next token
static void advance() {
    token_pos++;
}

// Parse struct declaration (simplified for TinyCC-ALE compatibility)
static int parse_struct_declaration() {
    fprintf(stderr, "Parsing struct declaration (simplified)\n");

    // For now, just skip until semicolon - very basic
    while (current_token()->type != TOK_SEMI && current_token()->type != TOK_EOF) {
        advance();
    }
    if (current_token()->type == TOK_SEMI) {
        advance();
    }

    fprintf(stderr, "Struct declaration skipped\n");
    return 1; // Success
}

// Expect a specific token type
static int expect(TokenType type) {
    if (current_token()->type == type) {
        advance();
        return 1;
    }
    return 0;
}

// Parse expression (recursive descent) - Phase 2
static ASTNode* parse_expression() {
    return parse_comparison();
}

static ASTNode* parse_comparison() {
    ASTNode* left = parse_term();

    TokenType op_type = current_token()->type;
    if (op_type == TOK_LT || op_type == TOK_GT ||
        op_type == TOK_LE || op_type == TOK_GE ||
        op_type == TOK_EQ) {

        char op;
        switch (op_type) {
            case TOK_LT: op = '<'; break;
            case TOK_GT: op = '>'; break;
            case TOK_LE: op = 'L'; break; // 'L' for <=
            case TOK_GE: op = 'G'; break; // 'G' for >=
            case TOK_EQ: op = 'E'; break; // 'E' for ==
            default: op = '?'; break;
        }
        advance();

        ASTNode* right = parse_term();

        ASTNode* binary = create_node(AST_BINARY_OP);
        binary->data.binary.op = op;
        binary->data.binary.left = left;
        binary->data.binary.right = right;

        return binary;
    }

    return left;
}

static ASTNode* parse_term() {
    ASTNode* left = parse_factor();

    while (current_token()->type == TOK_PLUS ||
           current_token()->type == TOK_MINUS ||
           current_token()->type == TOK_STAR ||
           current_token()->type == TOK_SLASH) {
        char op = current_token()->value[0];
        advance();

        ASTNode* right = parse_factor();

        ASTNode* binary = create_node(AST_BINARY_OP);
        binary->data.binary.op = op;
        binary->data.binary.left = left;
        binary->data.binary.right = right;

        left = binary;
    }

    return left;
}

static ASTNode* parse_factor() {
    if (current_token()->type == TOK_NUM) {
        ASTNode* node = create_node(AST_NUM);
        node->data.num_value = atoi(current_token()->value);
        advance();
        return node;
    }

    if (current_token()->type == TOK_IDENT) {
        Token* ident_token = current_token();
        advance();

        // Check if it's a function call
        if (current_token()->type == TOK_LPAREN) {
            return parse_function_call_with_name(ident_token->value);
        } else {
            // Variable reference
            ASTNode* node = create_node(AST_VAR);
            node->data.var_name = my_strdup(ident_token->value);
            return node;
        }
    }

    // Dereference (*ptr)
    if (current_token()->type == TOK_STAR) {
        advance();
        ASTNode* expr = parse_factor();
        if (!expr) return NULL;

        ASTNode* deref = create_node(AST_DEREF);
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

        ASTNode* addr = create_node(AST_ADDR);
        addr->data.addr_var_name = my_strdup(current_token()->value);
        advance();
        return addr;
    }

    if (expect(TOK_LPAREN)) {
        ASTNode* expr = parse_expression();
        expect(TOK_RPAREN);
        return expr;
    }

    fprintf(stderr, "Expected number, identifier, '*', '&', or '(' at line %d\n",
            current_token()->line);
    return NULL;
}

// Parse function call with known function name
static ASTNode* parse_function_call_with_name(const char* func_name) {
    expect(TOK_LPAREN); // consume (

    // Parse arguments
    ASTNode** args = NULL;
    int arg_count = 0;
    int capacity = 0;

    if (current_token()->type != TOK_RPAREN) {
        // Parse first argument
        ASTNode* arg = parse_expression();
        if (!arg) return NULL;

        if (arg_count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            args = realloc(args, capacity * sizeof(ASTNode*));
        }
        args[arg_count++] = arg;

        // Parse additional arguments
        while (current_token()->type == TOK_COMMA) {
            advance(); // consume ,
            arg = parse_expression();
            if (!arg) {
                for (int i = 0; i < arg_count; i++) free_ast(args[i]);
                free(args);
                return NULL;
            }

            if (arg_count >= capacity) {
                capacity *= 2;
                args = realloc(args, capacity * sizeof(ASTNode*));
            }
            args[arg_count++] = arg;
        }
    }

    if (!expect(TOK_RPAREN)) {
        for (int i = 0; i < arg_count; i++) free_ast(args[i]);
        free(args);
        return NULL;
    }

    ASTNode* call = create_node(AST_FUNC_CALL);
    call->data.func_call.name = my_strdup(func_name);
    call->data.func_call.args = args;
    call->data.func_call.arg_count = arg_count;
    return call;
}

// Parse return statement
static ASTNode* parse_return() {
    fprintf(stderr, "Parsing return statement\n");
    if (!expect(TOK_RETURN)) {
        fprintf(stderr, "Expected 'return' at line %d\n", current_token()->line);
        return NULL;
    }

    fprintf(stderr, "Parsing return expression\n");
    ASTNode* expr = parse_expression();

    fprintf(stderr, "Parsed return expression, now expecting ';'\n");
    if (!expect(TOK_SEMI)) {
        fprintf(stderr, "Expected ';' after return statement at line %d, found %s\n",
                current_token()->line, token_type_name(current_token()->type));
        free_ast(expr);
        return NULL;
    }

    ASTNode* return_node = create_node(AST_RETURN);
    return_node->data.return_expr = expr;
    return return_node;
}

// Parse statement
static ASTNode* parse_statement() {
    TokenType type = current_token()->type;

    // Debug: print current token
    fprintf(stderr, "Parsing statement, current token: %s at line %d\n",
            token_type_name(type), current_token()->line);

    // Try different statement types
    if (type == TOK_INT || type == TOK_CHAR || type == TOK_LONG) {
        // Variable or function declaration
        fprintf(stderr, "Parsing declaration (type %d)\n", type);
        // Look ahead to see if it's a function or variable
        Token* next = peek_token();
        if (next && next->type == TOK_LPAREN) {
            // Function declaration
            return parse_function_definition();
        } else {
            // Variable declaration
            return parse_variable_declaration();
        }
    } else if (type == TOK_IF) {
        fprintf(stderr, "Parsing if statement\n");
        return parse_if_statement();
    } else if (type == TOK_WHILE) {
        fprintf(stderr, "Parsing while statement\n");
        return parse_while_statement();
    } else if (type == TOK_RETURN) {
        fprintf(stderr, "Parsing return statement\n");
        return parse_return();
    } else if (type == TOK_IDENT) {
        fprintf(stderr, "Parsing identifier, checking for assignment\n");
        // Assignment or expression statement
        Token* ident_token = current_token();
        advance();

        if (current_token()->type == TOK_EQUAL) {
            // Assignment
            advance(); // consume =

            ASTNode* value = parse_expression();

            if (!expect(TOK_SEMI)) {
                fprintf(stderr, "Expected ';' after assignment at line %d\n",
                        current_token()->line);
                free_ast(value);
                return NULL;
            }

            ASTNode* assignment = create_node(AST_ASSIGNMENT);
            assignment->data.assignment.var_name = my_strdup(ident_token->value);
            assignment->data.assignment.value = value;
            return assignment;
        } else {
            // Not implemented yet - just expression
            fprintf(stderr, "Expression statements not implemented at line %d\n",
                    current_token()->line);
            return NULL;
        }
    } else if (type == TOK_LBRACE) {
        // Block statement
        fprintf(stderr, "Parsing block statement\n");
        advance(); // consume {

        ASTNode** statements = NULL;
        int statement_count = 0;
        int capacity = 0;

        while (current_token()->type != TOK_RBRACE && current_token()->type != TOK_EOF) {
            ASTNode* stmt = parse_statement();
            if (!stmt) {
                // Free allocated statements
                for (int i = 0; i < statement_count; i++) {
                    free_ast(statements[i]);
                }
                free(statements);
                return NULL;
            }

            // Expand array if needed
            if (statement_count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                statements = realloc(statements, capacity * sizeof(ASTNode*));
            }

            statements[statement_count++] = stmt;
        }

        if (!expect(TOK_RBRACE)) {
            fprintf(stderr, "Expected '}' at line %d\n", current_token()->line);
            // Free allocated statements
            for (int i = 0; i < statement_count; i++) {
                free_ast(statements[i]);
            }
            free(statements);
            return NULL;
        }

        ASTNode* block = create_node(AST_BLOCK);
        block->data.block.statements = statements;
        block->data.block.statement_count = statement_count;
        return block;
    }

    fprintf(stderr, "Expected statement, found %s at line %d\n",
            token_type_name(type), current_token()->line);
    return NULL;
}

// Parse variable declaration (Phase 3 - with pointers)
static ASTNode* parse_variable_declaration() {
    // Accept int, char, or long
    TokenType expected_type = current_token()->type;
    if (expected_type != TOK_INT && expected_type != TOK_CHAR && expected_type != TOK_LONG) {
        fprintf(stderr, "Expected 'int', 'char', or 'long' at line %d, got %s\n",
                current_token()->line, token_type_name(expected_type));
        return NULL;
    }
    advance(); // consume the type token

    // Check for pointer declarator
    int is_pointer = 0;
    if (current_token()->type == TOK_STAR) {
        is_pointer = 1;
        advance(); // consume *
    }

    if (current_token()->type != TOK_IDENT) {
        fprintf(stderr, "Expected variable name at line %d\n", current_token()->line);
        return NULL;
    }

    char* var_name = my_strdup(current_token()->value);
    advance();

    // Check for array dimensions (simplified - just skip for now)
    while (current_token()->type == TOK_LBRACKET) {
        advance(); // consume [
        // Skip array size expression (simplified)
        while (current_token()->type != TOK_RBRACKET && current_token()->type != TOK_EOF) {
            advance();
        }
        if (current_token()->type == TOK_RBRACKET) {
            advance(); // consume ]
        }
    }

    ASTNode* initializer = NULL;

    if (current_token()->type == TOK_EQUAL) {
        advance(); // consume =
        initializer = parse_expression();
    }

    if (!expect(TOK_SEMI)) {
        fprintf(stderr, "Expected ';' after variable declaration at line %d\n",
                current_token()->line);
        free(var_name);
        free_ast(initializer);
        return NULL;
    }

    ASTNode* var_decl = create_node(AST_VAR_DECL);
    var_decl->data.var_decl.var_name = var_name;
    var_decl->data.var_decl.initializer = initializer;
    // Note: is_pointer flag could be stored in AST if needed for type checking
    return var_decl;
}

// Parse if statement
static ASTNode* parse_if_statement() {
    if (!expect(TOK_IF)) {
        fprintf(stderr, "Expected 'if' at line %d\n", current_token()->line);
        return NULL;
    }

    if (!expect(TOK_LPAREN)) {
        fprintf(stderr, "Expected '(' after if at line %d\n", current_token()->line);
        return NULL;
    }

    ASTNode* condition = parse_expression();

    if (!expect(TOK_RPAREN)) {
        fprintf(stderr, "Expected ')' after if condition at line %d\n",
                current_token()->line);
        free_ast(condition);
        return NULL;
    }

    ASTNode* then_branch = parse_statement();
    ASTNode* else_branch = NULL;

    if (current_token()->type == TOK_ELSE) {
        advance(); // consume else
        else_branch = parse_statement();
    }

    ASTNode* if_stmt = create_node(AST_IF);
    if_stmt->data.if_stmt.condition = condition;
    if_stmt->data.if_stmt.then_branch = then_branch;
    if_stmt->data.if_stmt.else_branch = else_branch;
    return if_stmt;
}

// Parse while statement
static ASTNode* parse_while_statement() {
    if (!expect(TOK_WHILE)) {
        fprintf(stderr, "Expected 'while' at line %d\n", current_token()->line);
        return NULL;
    }

    if (!expect(TOK_LPAREN)) {
        fprintf(stderr, "Expected '(' after while at line %d\n", current_token()->line);
        return NULL;
    }

    ASTNode* condition = parse_expression();

    if (!expect(TOK_RPAREN)) {
        fprintf(stderr, "Expected ')' after while condition at line %d\n",
                current_token()->line);
        free_ast(condition);
        return NULL;
    }

    ASTNode* body = parse_statement();

    ASTNode* while_stmt = create_node(AST_WHILE);
    while_stmt->data.while_stmt.condition = condition;
    while_stmt->data.while_stmt.body = body;
    return while_stmt;
}

// Parse function body
static ASTNode* parse_function_body() {
    if (!expect(TOK_LBRACE)) {
        fprintf(stderr, "Expected '{' at line %d\n", current_token()->line);
        return NULL;
    }

    // For Phase 2: multiple statements
    ASTNode** statements = NULL;
    int statement_count = 0;
    int capacity = 0;

    while (current_token()->type != TOK_RBRACE && current_token()->type != TOK_EOF) {
        ASTNode* stmt = parse_statement();
        if (!stmt) {
            // Free allocated statements
            for (int i = 0; i < statement_count; i++) {
                free_ast(statements[i]);
            }
            free(statements);
            return NULL;
        }

        // Expand array if needed
        if (statement_count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            statements = realloc(statements, capacity * sizeof(ASTNode*));
        }

        statements[statement_count++] = stmt;
    }

    if (!expect(TOK_RBRACE)) {
        fprintf(stderr, "Expected '}' at line %d\n", current_token()->line);
        // Free allocated statements
        for (int i = 0; i < statement_count; i++) {
            free_ast(statements[i]);
        }
        free(statements);
        return NULL;
    }

    ASTNode* block = create_node(AST_BLOCK);
    block->data.block.statements = statements;
    block->data.block.statement_count = statement_count;
    return block;
}

// Parse parameter list
static ASTNode* parse_parameter_list() {
    char** param_names = NULL;
    int param_count = 0;
    int capacity = 0;

    while (current_token()->type != TOK_RPAREN) {
        // Parse parameter: int/char/long[*] param_name
        TokenType param_type = current_token()->type;
        if (param_type != TOK_INT && param_type != TOK_CHAR && param_type != TOK_LONG) {
            fprintf(stderr, "Expected parameter type (int/char/long) at line %d, got %s\n",
                    current_token()->line, token_type_name(param_type));
            goto error;
        }
        advance(); // consume parameter type

        // Check for pointer declarator
        int is_pointer_param = 0;
        if (current_token()->type == TOK_STAR) {
            is_pointer_param = 1;
            advance(); // consume *
        }

        if (current_token()->type != TOK_IDENT) {
            fprintf(stderr, "Expected parameter name at line %d\n",
                    current_token()->line);
            goto error;
        }

        // Add parameter name
        if (param_count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            param_names = realloc(param_names, capacity * sizeof(char*));
        }
        param_names[param_count++] = my_strdup(current_token()->value);
        advance();

        // Check for comma (more parameters)
        if (current_token()->type == TOK_COMMA) {
            advance(); // consume comma
        } else if (current_token()->type != TOK_RPAREN) {
            fprintf(stderr, "Expected ',' or ')' in parameter list at line %d\n",
                    current_token()->line);
            goto error;
        }
    }

    ASTNode* param_list = create_node(AST_PARAM_LIST);
    param_list->data.param_list.param_names = param_names;
    param_list->data.param_list.param_count = param_count;
    return param_list;

error:
    for (int i = 0; i < param_count; i++) free(param_names[i]);
    free(param_names);
    return NULL;
}

// Parse function definition
static ASTNode* parse_function_definition() {
    // Accept int, char, or long return types
    TokenType return_type = current_token()->type;
    if (return_type != TOK_INT && return_type != TOK_CHAR && return_type != TOK_LONG) {
        fprintf(stderr, "Expected return type (int/char/long) at line %d, got %s\n",
                current_token()->line, token_type_name(return_type));
        return NULL;
    }
    advance(); // consume return type

    if (current_token()->type != TOK_IDENT) {
        fprintf(stderr, "Expected function name at line %d\n", current_token()->line);
        return NULL;
    }

    char* func_name = my_strdup(current_token()->value);
    advance();

    // Parse parameters
    if (!expect(TOK_LPAREN)) {
        free(func_name);
        return NULL;
    }

    ASTNode* params = parse_parameter_list();
    if (!params) {
        free(func_name);
        return NULL;
    }

    if (!expect(TOK_RPAREN)) {
        free_ast(params);
        free(func_name);
        return NULL;
    }

    ASTNode* body = parse_function_body();
    if (!body) {
        free_ast(params);
        free(func_name);
        return NULL;
    }

    ASTNode* func_def = create_node(AST_FUNC_DEF);
    func_def->data.func_def.name = func_name;
    func_def->data.func_def.params = params;
    func_def->data.func_def.body = body;

    return func_def;
}

// Main parse function (Phase 3)
ASTNode* parse(Token* token_stream) {
    tokens = token_stream;
    token_pos = 0;

    // For Phase 3: collect all function definitions in a block
    ASTNode** functions = NULL;
    int func_count = 0;
    int capacity = 0;

    while (current_token()->type != TOK_EOF) {
        TokenType current_type = current_token()->type;
        fprintf(stderr, "Global token: %s\n", token_type_name(current_type));
        if (current_type == TOK_INT || current_type == TOK_CHAR || current_type == TOK_LONG) {
            fprintf(stderr, "Parsing function\n");
            ASTNode* func_def = parse_function_definition();
            if (!func_def) return NULL;
        } else if (current_type == TOK_STRUCT) {
            fprintf(stderr, "Parsing struct declaration\n");
            // Parse struct declaration: struct Name { ... };
            if (!parse_struct_declaration()) return NULL;
            // Struct declarations don't add to functions array
        } else {
            fprintf(stderr, "Expected function definition at line %d\n",
                    current_token()->line);
            // Free allocated functions
            for (int i = 0; i < func_count; i++) {
                free_ast(functions[i]);
            }
            free(functions);
            return NULL;
        }
    }

    // Create a block containing all functions
    ASTNode* program = create_node(AST_BLOCK);
    program->data.block.statements = functions;
    program->data.block.statement_count = func_count;
    return program;
}
