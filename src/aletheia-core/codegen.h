/*
 * ALETHEIA-Core: Code Generator
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "core.h"

/* Symbol table entry */
typedef struct {
    char* name;
    TypeInfo* type;
    int offset;
} Symbol;

/* Symbol table */
typedef struct {
    Symbol* symbols;
    int count;
    int capacity;
} SymbolTable;

/* Code generator */
typedef struct {
    FILE* output;
    SymbolTable* symtab;
    int label_count;
} CodeGen;

/* Functions */
CodeGen* create_codegen(FILE* output);
void free_codegen(CodeGen* gen);

void generate_code(ASTNode* ast, CodeGen* gen);
void generate_function(ASTNode* func, CodeGen* gen);
void generate_statement(ASTNode* stmt, CodeGen* gen);
void generate_expression(ASTNode* expr, CodeGen* gen);

/* Symbol table functions */
SymbolTable* create_symbol_table();
void free_symbol_table(SymbolTable* symtab);
int add_symbol(SymbolTable* symtab, char* name, TypeInfo* type);
int find_symbol(SymbolTable* symtab, char* name);

#endif /* CODEGEN_H */

