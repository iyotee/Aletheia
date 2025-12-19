/*
 * ALETHEIA MesCC-ALE Phase 2: Code Generator
 *
 * Generates x86-64 assembly code from AST.
 * Output format: NASM assembly
 * Supports: variables, if/else, while, assignments
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

// Symbol table management
void init_symbol_table(SymbolTable* symtab) {
    symtab->symbols = NULL;
    symtab->count = 0;
    symtab->capacity = 0;
}

void free_symbol_table(SymbolTable* symtab) {
    for (int i = 0; i < symtab->count; i++) {
        free(symtab->symbols[i].name);
    }
    free(symtab->symbols);
    symtab->symbols = NULL;
    symtab->count = 0;
    symtab->capacity = 0;
}

int add_symbol(SymbolTable* symtab, const char* name) {
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
                                 symtab->capacity * sizeof(Symbol));
    }

    // Add new symbol
    symtab->symbols[symtab->count].name = my_strdup(name);
    symtab->symbols[symtab->count].offset = -(symtab->count + 1) * 8; // 8 bytes per variable
    return symtab->symbols[symtab->count++].offset;
}

int get_symbol_offset(SymbolTable* symtab, const char* name) {
    for (int i = 0; i < symtab->count; i++) {
        if (strcmp(symtab->symbols[i].name, name) == 0) {
            return symtab->symbols[i].offset;
        }
    }
    return 0; // Not found
}

// Simple register allocation
static int reg_counter = 0;
static const char* get_register() {
    const char* regs[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi"};
    return regs[reg_counter++ % 6];
}

// Generate code for expressions
static void generate_expression(ASTNode* node, FILE* output, SymbolTable* symtab) {
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
            // Push arguments in reverse order (x86-64 calling convention)
            for (int i = node->data.func_call.arg_count - 1; i >= 0; i--) {
                generate_expression(node->data.func_call.args[i], output, symtab);
                fprintf(output, "    push rax  ;; push arg %d\n", i);
            }

            // Call the function
            fprintf(output, "    call %s\n", node->data.func_call.name);

            // Clean up stack (each arg is 8 bytes)
            if (node->data.func_call.arg_count > 0) {
                fprintf(output, "    add rsp, %d  ;; clean up %d args\n",
                       node->data.func_call.arg_count * 8, node->data.func_call.arg_count);
            }
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
static void generate_statement(ASTNode* node, FILE* output, SymbolTable* symtab) {
    switch (node->type) {
        case AST_RETURN:
            generate_expression(node->data.return_expr, output, symtab);
            fprintf(output, "    mov rsp, rbp\n");
            fprintf(output, "    pop rbp\n");
            fprintf(output, "    ret\n");
            break;

        case AST_VAR_DECL:
            {
                int offset = add_symbol(symtab, node->data.var_decl.var_name);
                fprintf(output, "    ;; Declare variable %s at [rbp%+d]\n",
                       node->data.var_decl.var_name, offset);

                if (node->data.var_decl.initializer) {
                    generate_expression(node->data.var_decl.initializer, output, symtab);
                    fprintf(output, "    mov [rbp%+d], rax  ;; initialize %s\n",
                           offset, node->data.var_decl.var_name);
                }
            }
            break;

        case AST_ASSIGNMENT:
            {
                int offset = get_symbol_offset(symtab, node->data.assignment.var_name);
                if (offset != 0) {
                    generate_expression(node->data.assignment.value, output, symtab);
                    fprintf(output, "    mov [rbp%+d], rax  ;; %s =\n",
                           offset, node->data.assignment.var_name);
                } else {
                    fprintf(output, "    ;; Variable %s not found for assignment\n",
                           node->data.assignment.var_name);
                }
            }
            break;

        case AST_IF:
            {
                static int if_count = 0;
                int current_if = if_count++;

                // Generate condition
                generate_expression(node->data.if_stmt.condition, output, symtab);
                fprintf(output, "    test rax, rax\n");
                fprintf(output, "    jz .Lelse_%d\n", current_if);

                // Generate then branch
                generate_statement(node->data.if_stmt.then_branch, output, symtab);

                if (node->data.if_stmt.else_branch) {
                    fprintf(output, "    jmp .Lend_%d\n", current_if);
                    fprintf(output, ".Lelse_%d:\n", current_if);
                    generate_statement(node->data.if_stmt.else_branch, output, symtab);
                } else {
                    fprintf(output, ".Lelse_%d:\n", current_if);
                }

                fprintf(output, ".Lend_%d:\n", current_if);
            }
            break;

        case AST_WHILE:
            {
                static int while_count = 0;
                int current_while = while_count++;

                fprintf(output, ".Lwhile_%d:\n", current_while);

                // Generate condition
                generate_expression(node->data.while_stmt.condition, output, symtab);
                fprintf(output, "    test rax, rax\n");
                fprintf(output, "    jz .Lend_while_%d\n", current_while);

                // Generate body
                generate_statement(node->data.while_stmt.body, output, symtab);
                fprintf(output, "    jmp .Lwhile_%d\n", current_while);

                fprintf(output, ".Lend_while_%d:\n", current_while);
            }
            break;

        case AST_BLOCK:
            for (int i = 0; i < node->data.block.statement_count; i++) {
                generate_statement(node->data.block.statements[i], output, symtab);
            }
            break;

        default:
            fprintf(output, "    ;; Unsupported statement type\n");
    }
}

// Generate code for function definition
static void generate_function(ASTNode* node, FILE* output, SymbolTable* symtab) {
    fprintf(output, ";; Function: %s\n", node->data.func_def.name);
    fprintf(output, "global %s\n", node->data.func_def.name);
    fprintf(output, "%s:\n", node->data.func_def.name);

    // Function prologue
    fprintf(output, "    push rbp\n");
    fprintf(output, "    mov rbp, rsp\n");

    // Initialize symbol table for this function
    init_symbol_table(symtab);

    // Handle parameters (Phase 3) - System V AMD64 calling convention
    if (node->data.func_def.params) {
        ASTNode* params = node->data.func_def.params;
        for (int i = 0; i < params->data.param_list.param_count; i++) {
            char* param_name = params->data.param_list.param_names[i];
            int param_offset;

            if (i < 6) {
                // First 6 parameters in registers: rdi, rsi, rdx, rcx, r8, r9
                const char* reg_names[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                fprintf(output, "    ;; Parameter %s in register %s\n", param_name, reg_names[i]);

                // Move parameter register to a local variable on stack
                param_offset = -(symtab->count + 1) * 8;
                fprintf(output, "    mov [rbp%+d], %s  ;; store param %s\n",
                       param_offset, reg_names[i], param_name);
            } else {
                // Additional parameters on stack at [rbp+16] + (i-6)*8
                param_offset = 16 + ((i - 6) * 8);
                fprintf(output, "    ;; Parameter %s at [rbp+%d]\n", param_name, param_offset);
            }

            add_symbol(symtab, param_name);
        }
    }

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
void generate_code(ASTNode* ast, FILE* output, SymbolTable* symtab) {
    if (!ast) return;

    // Generate NASM header
    fprintf(output, ";; ALETHEIA MesCC-ALE Phase 2 Output\n");
    fprintf(output, ";; Generated assembly code with variables and control flow\n");
    fprintf(output, "\n");
    fprintf(output, "section .text\n");
    fprintf(output, "\n");

    // Generate code based on AST type
    switch (ast->type) {
        case AST_FUNC_DEF:
            generate_function(ast, output, symtab);
            break;

        case AST_BLOCK:
            // Generate code for each function in the program
            for (int i = 0; i < ast->data.block.statement_count; i++) {
                if (ast->data.block.statements[i]->type == AST_FUNC_DEF) {
                    generate_function(ast->data.block.statements[i], output, symtab);
                }
            }
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
