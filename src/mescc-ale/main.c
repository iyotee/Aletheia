/*
 * ALETHEIA MesCC-ALE Phase 1: Main Compiler
 *
 * Usage: mescc-ale < input.c > output.asm
 */

#include <stdio.h>
#include <stdlib.h>
#include "mescc.h"

int main() {
    // Read source code from stdin
    char* source = NULL;
    size_t source_size = 0;
    size_t buffer_size = 1024;
    source = malloc(buffer_size);
    if (source == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    size_t total_read = 0;
    size_t bytes_read;

    while ((bytes_read = fread(source + total_read, 1,
                               buffer_size - total_read - 1, stdin)) > 0) {
        total_read += bytes_read;

        // Expand buffer if needed
        if (total_read >= buffer_size - 1) {
            buffer_size *= 2;
            char* new_source = realloc(source, buffer_size);
            if (new_source == NULL) {
                fprintf(stderr, "Memory reallocation failed\n");
                free(source);
                return 1;
            }
            source = new_source;
        }
    }

    if (total_read == 0) {
        fprintf(stderr, "No input provided\n");
        free(source);
        return 1;
    }

    source[total_read] = '\0';

    // Tokenize
    Token* tokens = tokenize(source);
    if (!tokens) {
        fprintf(stderr, "Tokenization failed\n");
        free(source);
        return 1;
    }

    // Parse
    ASTNode* ast = parse(tokens);
    if (!ast) {
        fprintf(stderr, "Parsing failed\n");
        free(source);
        // Note: tokens are freed by parser on error
        return 1;
    }

    // Generate code
    SymbolTable symtab;
    generate_code(ast, stdout, &symtab);

    // Cleanup
    free_ast(ast);
    free(source);

    return 0;
}
