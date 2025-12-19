/*
 * ALETHEIA TinyCC-ALE: Code Generator
 *
 * Extended code generator supporting more C features.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinycc.h"

// Simple string duplication
static char* tiny_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

// Symbol table management
void init_symbol_table(TinySymbolTable* symtab) {
    symtab->symbols = NULL;
    symtab->count = 0;
    symtab->capacity = 0;
}

void free_symbol_table(TinySymbolTable* symtab) {
    for (int i = 0; i < symtab->count; i++) {
        free(symtab->symbols[i].name);
        tiny_free_type(symtab->symbols[i].type);
    }
    free(symtab->symbols);
    symtab->symbols = NULL;
    symtab->count = 0;
    symtab->capacity = 0;
}

int add_symbol(TinySymbolTable* symtab, const char* name, TinyType* type) {
    // Check if symbol already exists
    for (int i = 0; i < symtab->count; i++) {
        if (strcmp(symtab->symbols[i].name, name) == 0) {
            return symtab->symbols[i].offset;
        }
    }

    // Expand array if needed
    if (symtab->count >= symtab->capacity) {
        symtab->capacity = symtab->capacity == 0 ? 8 : symtab->capacity * 2;
        symtab->symbols = realloc(symtab->symbols,
                                 symtab->capacity * sizeof(TinySymbol));
    }

    // Add new symbol
    symtab->symbols[symtab->count].name = tiny_strdup(name);
    symtab->symbols[symtab->count].type = type; // Copy type?
    symtab->symbols[symtab->count].offset = -(symtab->count + 1) * 8; // 8 bytes per variable
    return symtab->symbols[symtab->count++].offset;
}

int get_symbol_offset(TinySymbolTable* symtab, const char* name) {
    for (int i = 0; i < symtab->count; i++) {
        if (strcmp(symtab->symbols[i].name, name) == 0) {
            return symtab->symbols[i].offset;
        }
    }
    return 0; // Not found
}

// Generate code for expressions
static void generate_expression(TinyASTNode* node, FILE* output, TinySymbolTable* symtab) {
    switch (node->type) {
        case AST_NUM:
            fprintf(output, "    mov rax, %d\n", node->data.num_value);
            break;

        case AST_VAR:
            {
                int offset = get_symbol_offset(symtab, node->data.var_name);
                if (offset != 0) {
                    fprintf(output, "    mov rax, [rbp%+d]  ;; load %s\n",
                           offset, node->data.var_name);
                } else {
                    fprintf(output, "    ;; Variable %s not found\n", node->data.var_name);
                    fprintf(output, "    mov rax, 0\n");
                }
            }
            break;

        case AST_DEREF:
            generate_expression(node->data.deref_expr, output, symtab);
            fprintf(output, "    mov rax, [rax]  ;; dereference\n");
            break;

        case AST_ADDR:
            {
                int offset = get_symbol_offset(symtab, node->data.addr_var_name);
                if (offset != 0) {
                    fprintf(output, "    lea rax, [rbp%+d]  ;; address of %s\n",
                           offset, node->data.addr_var_name);
                } else {
                    fprintf(output, "    ;; Variable %s not found for address\n",
                           node->data.addr_var_name);
                    fprintf(output, "    mov rax, 0\n");
                }
            }
            break;

        case AST_FUNC_CALL:
            // Simple function calls without arguments
            fprintf(output, "    call %s\n", node->data.func_call.name);
            break;

        case AST_BINARY_OP:
            // Generate right operand first (x86 convention)
            generate_expression(node->data.binary.right, output, symtab);
            fprintf(output, "    push rax\n");

            // Generate left operand
            generate_expression(node->data.binary.left, output, symtab);

            // Perform operation
            fprintf(output, "    pop rbx\n");
            switch (node->data.binary.op) {
                case '+':
                    fprintf(output, "    add rax, rbx\n");
                    break;
                case '-':
                    fprintf(output, "    sub rax, rbx\n");
                    break;
                case '*':
                    fprintf(output, "    imul rax, rbx\n");
                    break;
                case '/':
                    fprintf(output, "    cqo\n");
                    fprintf(output, "    idiv rbx\n");
                    break;
                case '<':
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setl al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case '>':
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setg al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case 'L': // <=
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setle al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case 'G': // >=
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setge al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case 'E': // ==
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    sete al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                default:
                    fprintf(output, "    ;; Unsupported operator: %c\n", node->data.binary.op);
            }
            break;

        default:
            fprintf(output, "    ;; Unsupported expression type\n");
    }
}

// Generate code for statements
static void generate_statement(TinyASTNode* node, FILE* output, TinySymbolTable* symtab) {
    switch (node->type) {
        case AST_RETURN:
            generate_expression(node->data.return_expr, output, symtab);
            fprintf(output, "    mov rsp, rbp\n");
            fprintf(output, "    pop rbp\n");
            fprintf(output, "    ret\n");
            break;

        case AST_VAR_DECL:
            {
                int offset = add_symbol(symtab, node->data.var_decl.var_name,
                                       node->data.var_decl.var_type);
                fprintf(output, "    ;; Declare variable %s at [rbp%+d]\n",
                       node->data.var_decl.var_name, offset);

                if (node->data.var_decl.initializer) {
                    generate_expression(node->data.var_decl.initializer, output, symtab);
                    fprintf(output, "    mov [rbp%+d], rax  ;; initialize %s\n",
                           offset, node->data.var_decl.var_name);
                }
            }
            break;

        default:
            fprintf(output, "    ;; Unsupported statement type\n");
    }
}

// Generate code for function definition
static void generate_function(TinyASTNode* node, FILE* output, TinySymbolTable* symtab) {
    fprintf(output, ";; Function: %s\n", node->data.func_def.name);
    fprintf(output, "global %s\n", node->data.func_def.name);
    fprintf(output, "%s:\n", node->data.func_def.name);

    // Function prologue
    fprintf(output, "    push rbp\n");
    fprintf(output, "    mov rbp, rsp\n");

    // Initialize symbol table for this function
    init_symbol_table(symtab);

    // Generate function body
    generate_statement(node->data.func_def.body, output, symtab);

    // Function epilogue (only if not already done by return)
    fprintf(output, "    mov rsp, rbp\n");
    fprintf(output, "    pop rbp\n");
    fprintf(output, "    ret\n");
    fprintf(output, "\n");

    // Clean up symbol table
    free_symbol_table(symtab);
}

// Main code generation function
void tiny_generate_code(TinyASTNode* ast, FILE* output, TinySymbolTable* symtab) {
    if (!ast) return;

    // Generate NASM header
    fprintf(output, ";; ALETHEIA TinyCC-ALE Output\n");
    fprintf(output, ";; Extended C compiler with types\n");
    fprintf(output, "\n");
    fprintf(output, "section .text\n");
    fprintf(output, "\n");

    // Generate code based on AST type
    switch (ast->type) {
        case AST_FUNC_DEF:
            generate_function(ast, output, symtab);
            break;

        default:
            fprintf(output, ";; Unsupported AST root type\n");
    }

    // Add program entry point if this is a main function
    if (ast->type == AST_FUNC_DEF &&
        strcmp(ast->data.func_def.name, "main") == 0) {
        fprintf(output, ";; Program entry point\n");
        fprintf(output, "global _start\n");
        fprintf(output, "_start:\n");
        fprintf(output, "    call main\n");
        fprintf(output, "    mov rdi, rax\n");
        fprintf(output, "    mov rax, 60  ; sys_exit\n");
        fprintf(output, "    syscall\n");
    }
}

