/*
 * ALETHEIA-Core: Code Generator Implementation
 */

#include "codegen.h"
#include <string.h>  // For memcpy

/* Create code generator */
CodeGen* create_codegen(FILE* output) {
    CodeGen* gen = core_malloc(sizeof(CodeGen));
    gen->output = output;
    gen->symtab = create_symbol_table();
    gen->label_count = 0;
    return gen;
}

/* Free code generator */
void free_codegen(CodeGen* gen) {
    free_symbol_table(gen->symtab);
    core_free(gen);
}

/* Create symbol table */
SymbolTable* create_symbol_table() {
    SymbolTable* symtab = core_malloc(sizeof(SymbolTable));
    symtab->symbols = 0;
    symtab->count = 0;
    symtab->capacity = 0;
    return symtab;
}

/* Free symbol table */
void free_symbol_table(SymbolTable* symtab) {
    for (int i = 0; i < symtab->count; i++) {
        core_free(symtab->symbols[i].name);
        free_type(symtab->symbols[i].type);
    }
    core_free(symtab->symbols);
    core_free(symtab);
}

/* Add symbol */
int add_symbol(SymbolTable* symtab, char* name, TypeInfo* type) {
    /* Check if already exists */
    for (int i = 0; i < symtab->count; i++) {
        char* existing = symtab->symbols[i].name;
        bool match = true;
        char* n = name;
        char* e = existing;
        while (*n && *e) {
            if (*n != *e) {
                match = false;
                break;
            }
            n++;
            e++;
        }
        if (match && *n == '\0' && *e == '\0') {
            return symtab->symbols[i].offset;
        }
    }

    /* Expand if needed */
    if (symtab->count >= symtab->capacity) {
        int old_capacity = symtab->capacity;
        symtab->capacity = symtab->capacity == 0 ? 8 : symtab->capacity * 2;
        Symbol* old_symbols = symtab->symbols;
        symtab->symbols = core_malloc(symtab->capacity * sizeof(Symbol));

        /* Copy existing symbols to new allocation */
        if (old_symbols != NULL && old_capacity > 0) {
            memcpy(symtab->symbols, old_symbols, old_capacity * sizeof(Symbol));
        }
    }

    /* Add new symbol */
    symtab->symbols[symtab->count].name = core_strdup(name);
    symtab->symbols[symtab->count].type = type;
    symtab->symbols[symtab->count].offset = -(symtab->count + 1) * 8;
    return symtab->symbols[symtab->count++].offset;
}

/* Find symbol */
int find_symbol(SymbolTable* symtab, char* name) {
    for (int i = 0; i < symtab->count; i++) {
        char* existing = symtab->symbols[i].name;
        bool match = true;
        char* n = name;
        char* e = existing;
        while (*n && *e) {
            if (*n != *e) {
                match = false;
                break;
            }
            n++;
            e++;
        }
        if (match && *n == '\0' && *e == '\0') {
            return symtab->symbols[i].offset;
        }
    }
    return 0; /* Not found */
}

/* Generate unique label */
void generate_label(CodeGen* gen, char* prefix) {
    fprintf(gen->output, ".L%s_%d:\n", prefix, gen->label_count++);
}

/* Generate expression */
void generate_expression(ASTNode* expr, CodeGen* gen) {
    switch (expr->type) {
        case AST_INTEGER_LITERAL:
            fprintf(gen->output, "    mov rax, %d\n", expr->data.int_value);
            break;

        case AST_IDENTIFIER: {
            int offset = find_symbol(gen->symtab, expr->data.identifier);
            if (offset != 0) {
                fprintf(gen->output, "    mov rax, [rbp%+d]  ;; load %s\n",
                       offset, expr->data.identifier);
            } else {
                fprintf(gen->output, "    mov rax, 0  ;; undefined variable %s\n",
                       expr->data.identifier);
            }
            break;
        }

        case AST_UNARY_EXPR:
            if (expr->data.unary.op == '*') {
                generate_expression(expr->data.unary.operand, gen);
                fprintf(gen->output, "    mov rax, [rax]  ;; dereference\n");
            }
            break;

        case AST_BINARY_EXPR: {
            /* Right operand first */
            generate_expression(expr->data.binary.right, gen);
            fprintf(gen->output, "    push rax\n");

            /* Left operand */
            generate_expression(expr->data.binary.left, gen);

            fprintf(gen->output, "    pop rbx\n");

            /* Operation */
            switch (expr->data.binary.op) {
                case '+':
                    fprintf(gen->output, "    add rax, rbx\n");
                    break;
                case '-':
                    fprintf(gen->output, "    sub rax, rbx\n");
                    break;
                case '*':
                    fprintf(gen->output, "    imul rax, rbx\n");
                    break;
                case '/':
                    fprintf(gen->output, "    cqo\n");
                    fprintf(gen->output, "    idiv rbx\n");
                    break;
                case '<':
                    fprintf(gen->output, "    cmp rax, rbx\n");
                    fprintf(gen->output, "    setl al\n");
                    fprintf(gen->output, "    movzx rax, al\n");
                    break;
                case '>':
                    fprintf(gen->output, "    cmp rax, rbx\n");
                    fprintf(gen->output, "    setg al\n");
                    fprintf(gen->output, "    movzx rax, al\n");
                    break;
                case '=': /* Note: simplified equality */
                    fprintf(gen->output, "    cmp rax, rbx\n");
                    fprintf(gen->output, "    sete al\n");
                    fprintf(gen->output, "    movzx rax, al\n");
                    break;
                default:
                    fprintf(gen->output, "    ;; unknown op %c\n", expr->data.binary.op);
            }
            break;
        }

        case AST_FUNCTION_CALL:
            /* For now, simple calls without arguments */
            fprintf(gen->output, "    call %s\n", expr->data.call.name);
            break;

        default:
            fprintf(gen->output, "    ;; unknown expression type\n");
    }
}

/* Generate statement */
void generate_statement(ASTNode* stmt, CodeGen* gen) {
    switch (stmt->type) {
        case AST_VAR_DECL: {
            int offset = add_symbol(gen->symtab, stmt->data.var_decl.name,
                                   stmt->data.var_decl.var_type);
            fprintf(gen->output, "    ;; var %s at [rbp%+d]\n",
                   stmt->data.var_decl.name, offset);

            if (stmt->data.var_decl.initializer) {
                generate_expression(stmt->data.var_decl.initializer, gen);
                fprintf(gen->output, "    mov [rbp%+d], rax\n", offset);
            }
            break;
        }

        case AST_RETURN_STMT:
            if (stmt->data.return_expr) {
                generate_expression(stmt->data.return_expr, gen);
            }
            fprintf(gen->output, "    mov rsp, rbp\n");
            fprintf(gen->output, "    pop rbp\n");
            fprintf(gen->output, "    ret\n");
            break;

        case AST_IF_STMT: {
            int label_id = gen->label_count++;

            generate_expression(stmt->data.if_stmt.condition, gen);
            fprintf(gen->output, "    test rax, rax\n");
            fprintf(gen->output, "    jz .Lelse_%d\n", label_id);

            generate_statement(stmt->data.if_stmt.then_branch, gen);

            if (stmt->data.if_stmt.else_branch) {
                fprintf(gen->output, "    jmp .Lend_%d\n", label_id);
                fprintf(gen->output, ".Lelse_%d:\n", label_id);
                generate_statement(stmt->data.if_stmt.else_branch, gen);
            } else {
                fprintf(gen->output, ".Lelse_%d:\n", label_id);
            }

            fprintf(gen->output, ".Lend_%d:\n", label_id);
            break;
        }

        case AST_WHILE_STMT: {
            int label_id = gen->label_count++;
            fprintf(gen->output, ".Lwhile_%d:\n", label_id);

            generate_expression(stmt->data.while_stmt.condition, gen);
            fprintf(gen->output, "    test rax, rax\n");
            fprintf(gen->output, "    jz .Lend_while_%d\n", label_id);

            generate_statement(stmt->data.while_stmt.body, gen);
            fprintf(gen->output, "    jmp .Lwhile_%d\n", label_id);
            fprintf(gen->output, ".Lend_while_%d:\n", label_id);
            break;
        }

        case AST_BLOCK:
            for (int i = 0; i < stmt->data.block.stmt_count; i++) {
                generate_statement(stmt->data.block.statements[i], gen);
            }
            break;

        case AST_ASSIGN_EXPR: {
            /* Generate value first */
            generate_expression(stmt->data.assign.value, gen);
            fprintf(gen->output, "    push rax\n");

            /* Assume simple identifier for now */
            if (stmt->data.assign.target->type == AST_IDENTIFIER) {
                int offset = find_symbol(gen->symtab, stmt->data.assign.target->data.identifier);
                fprintf(gen->output, "    pop rax\n");
                fprintf(gen->output, "    mov [rbp%+d], rax\n", offset);
            } else {
                fprintf(gen->output, "    pop rax  ;; complex assignment not implemented\n");
            }
            break;
        }

        default:
            /* Expression statement */
            generate_expression(stmt, gen);
            break;
    }
}

/* Generate function */
void generate_function(ASTNode* func, CodeGen* gen) {
    fprintf(gen->output, ";; Function: %s\n", func->data.func_def.name);
    fprintf(gen->output, "global %s\n", func->data.func_def.name);
    fprintf(gen->output, "%s:\n", func->data.func_def.name);

    /* Prologue */
    fprintf(gen->output, "    push rbp\n");
    fprintf(gen->output, "    mov rbp, rsp\n");

    /* Generate body */
    generate_statement(func->data.func_def.body, gen);

    /* Epilogue (in case no return) */
    fprintf(gen->output, "    mov rsp, rbp\n");
    fprintf(gen->output, "    pop rbp\n");
    fprintf(gen->output, "    ret\n");
    fprintf(gen->output, "\n");
}

/* Generate code */
void generate_code(ASTNode* ast, CodeGen* gen) {
    /* NASM header */
    fprintf(gen->output, ";; ALETHEIA-Core Output\n");
    fprintf(gen->output, ";; Bootstrap C compiler\n");
    fprintf(gen->output, "\n");
    fprintf(gen->output, "section .text\n");
    fprintf(gen->output, "\n");

    /* Generate program */
    if (ast->type == AST_PROGRAM) {
        for (int i = 0; i < ast->data.program.decl_count; i++) {
            ASTNode* decl = ast->data.program.declarations[i];
            if (decl->type == AST_FUNCTION_DEF) {
                generate_function(decl, gen);
            }
        }
    }

    /* Entry point for main */
    bool has_main = false;
    if (ast->type == AST_PROGRAM) {
        for (int i = 0; i < ast->data.program.decl_count; i++) {
            ASTNode* decl = ast->data.program.declarations[i];
            if (decl->type == AST_FUNCTION_DEF) {
                char* name = decl->data.func_def.name;
                bool is_main = true;
                char* m = "main";
                char* n = name;
                while (*m && *n) {
                    if (*m != *n) {
                        is_main = false;
                        break;
                    }
                    m++;
                    n++;
                }
                if (is_main && *m == '\0' && *n == '\0') {
                    has_main = true;
                    break;
                }
            }
        }
    }

    if (has_main) {
        fprintf(gen->output, ";; Program entry point\n");
        fprintf(gen->output, "global _start\n");
        fprintf(gen->output, "_start:\n");
        fprintf(gen->output, "    call main\n");
        fprintf(gen->output, "    mov rdi, rax\n");
        fprintf(gen->output, "    mov rax, 60  ; sys_exit\n");
        fprintf(gen->output, "    syscall\n");
    }
}

