/*
 * ALETHEIA MesCC-ALE Phase 1: Lexer
 *
 * Extremely simple lexer for basic C syntax.
 * Supports: int, return, identifiers, numbers, operators, punctuation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mescc.h"

// Simple string duplication functions (since strdup is not in C99)
static char* my_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

static char* my_strndup(const char* s, size_t n) {
    size_t len = 0;
    while (len < n && s[len]) len++;
    char* dup = malloc(len + 1);
    if (dup) {
        memcpy(dup, s, len);
        dup[len] = '\0';
    }
    return dup;
}

// Keywords mapping (Phase 2)
typedef struct {
    const char* word;
    TokenType type;
} Keyword;

static Keyword keywords[] = {
    {"int", TOK_INT},
    {"char", TOK_CHAR},
    {"long", TOK_LONG},
    {"struct", TOK_STRUCT},
    {"return", TOK_RETURN},
    {"if", TOK_IF},
    {"else", TOK_ELSE},
    {"while", TOK_WHILE},
    {NULL, TOK_EOF}
};

// Create a new token
static Token* create_token(TokenType type, const char* value, int line) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->value = value ? my_strdup(value) : NULL;
    token->line = line;
    return token;
}

// Free a token
static void free_token(Token* token) {
    if (token->value) free(token->value);
    free(token);
}

// Check if character is valid for identifier
static int is_ident_char(char c) {
    return isalnum(c) || c == '_';
}

// Tokenize source code
Token* tokenize(const char* source) {
    Token* tokens = NULL;
    int token_count = 0;
    int capacity = 0;
    int pos = 0;
    int line = 1;

    while (source[pos] != '\0') {
        char c = source[pos];

        // Skip whitespace and comments
        if (isspace(c)) {
            if (c == '\n') line++;
            pos++;
            continue;
        } else if (c == '/' && source[pos+1] == '/') {
            // Skip single-line comments
            pos += 2;  // Skip //
            while (source[pos] && source[pos] != '\n') pos++;
            if (source[pos] == '\n') {
                line++;
                pos++;  // Skip the newline
            }
            continue;
        } else if (c == '/' && source[pos+1] == '*') {
            // Skip multi-line comments
            pos += 2;  // Skip /*
            while (source[pos] && !(source[pos] == '*' && source[pos+1] == '/')) {
                if (source[pos] == '\n') line++;
                pos++;
            }
            if (source[pos] == '*' && source[pos+1] == '/') {
                pos += 2;  // Skip */
            }
            continue;
        }

        // Single character tokens and compound operators
        TokenType type = TOK_EOF;
        int consume_extra = 0;

        switch (c) {
            case '(': type = TOK_LPAREN; break;
            case ')': type = TOK_RPAREN; break;
            case '{': type = TOK_LBRACE; break;
            case '}': type = TOK_RBRACE; break;
            case '[': type = TOK_LBRACKET; break;
            case ']': type = TOK_RBRACKET; break;
            case ';': type = TOK_SEMI; break;
            case ',': type = TOK_COMMA; break;
            case '+': type = TOK_PLUS; break;
            case '-': type = TOK_MINUS; break;
            case '*': type = TOK_STAR; break;
            case '/': type = TOK_SLASH; break;
            case '&': type = TOK_ADDR; break;
            case '<':
                if (source[pos + 1] == '=') {
                    type = TOK_LE;
                    consume_extra = 1;
                } else {
                    type = TOK_LT;
                }
                break;
            case '>':
                if (source[pos + 1] == '=') {
                    type = TOK_GE;
                    consume_extra = 1;
                } else {
                    type = TOK_GT;
                }
                break;
            case '=':
                if (source[pos + 1] == '=') {
                    type = TOK_EQ;
                    consume_extra = 1;
                } else {
                    type = TOK_EQUAL;
                }
                break;
        }

        if (type != TOK_EOF) {
            // Expand token array if needed
            if (token_count >= capacity) {
                capacity = capacity == 0 ? 16 : capacity * 2;
                tokens = realloc(tokens, capacity * sizeof(Token));
            }

            // Create token value (handle compound operators)
            char* token_value;
            if (consume_extra) {
                char value[3] = {c, source[pos + 1], '\0'};
                token_value = my_strdup(value);
            } else {
                char value[2] = {c, '\0'};
                token_value = my_strdup(value);
            }

            tokens[token_count++] = *create_token(type, token_value, line);
            free(token_value);

            pos += 1 + consume_extra; // consume character(s)
            continue;
        }

        if (type != TOK_EOF) {
            // Expand token array if needed
            if (token_count >= capacity) {
                capacity = capacity == 0 ? 16 : capacity * 2;
                tokens = realloc(tokens, capacity * sizeof(Token));
            }

            char value[2] = {c, '\0'};
            tokens[token_count++] = *create_token(type, value, line);
            pos++;
            continue;
        }

        // Numbers
        if (isdigit(c)) {
            int start = pos;
            while (isdigit(source[pos])) pos++;
            char* value = my_strndup(&source[start], pos - start);

            if (token_count >= capacity) {
                capacity = capacity == 0 ? 16 : capacity * 2;
                tokens = realloc(tokens, capacity * sizeof(Token));
            }
            tokens[token_count++] = *create_token(TOK_NUM, value, line);
            free(value);
            continue;
        }

        // Identifiers and keywords
        if (isalpha(c) || c == '_') {
            int start = pos;
            while (is_ident_char(source[pos])) pos++;
            char* value = my_strndup(&source[start], pos - start);

            // Check if it's a keyword
            TokenType token_type = TOK_IDENT;
            for (int i = 0; keywords[i].word != NULL; i++) {
                if (strcmp(value, keywords[i].word) == 0) {
                    token_type = keywords[i].type;
                    break;
                }
            }

            if (token_count >= capacity) {
                capacity = capacity == 0 ? 16 : capacity * 2;
                tokens = realloc(tokens, capacity * sizeof(Token));
            }
            tokens[token_count++] = *create_token(token_type, value, line);
            free(value);
            continue;
        }

        // Unknown character
        fprintf(stderr, "Unexpected character '%c' at line %d\n", c, line);
        // Free allocated tokens
        for (int i = 0; i < token_count; i++) {
            free_token(&tokens[i]);
        }
        free(tokens);
        return NULL;
    }

    // Add EOF token
    if (token_count >= capacity) {
        capacity = capacity == 0 ? 16 : capacity * 2;
        tokens = realloc(tokens, capacity * sizeof(Token));
    }
    tokens[token_count++] = *create_token(TOK_EOF, NULL, line);

    // Add sentinel NULL token
    if (token_count >= capacity) {
        capacity++;
        tokens = realloc(tokens, capacity * sizeof(Token));
    }
    tokens[token_count].type = TOK_EOF;
    tokens[token_count].value = NULL;

    return tokens;
}

// Helper function to get token type name (for debugging)
const char* token_type_name(TokenType type) {
    switch (type) {
        case TOK_EOF: return "EOF";
        case TOK_INT: return "int";
        case TOK_CHAR: return "char";
        case TOK_LONG: return "long";
        case TOK_STRUCT: return "struct";
        case TOK_RETURN: return "return";
        case TOK_IF: return "if";
        case TOK_ELSE: return "else";
        case TOK_WHILE: return "while";
        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_LBRACKET: return "[";
        case TOK_RBRACKET: return "]";
        case TOK_SEMI: return ";";
        case TOK_COMMA: return ",";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_STAR: return "*";
        case TOK_SLASH: return "/";
        case TOK_EQUAL: return "=";
        case TOK_LT: return "<";
        case TOK_GT: return ">";
        case TOK_LE: return "<=";
        case TOK_GE: return ">=";
        case TOK_EQ: return "==";
        case TOK_ADDR: return "&";
        case TOK_IDENT: return "identifier";
        case TOK_NUM: return "number";
        default: return "unknown";
    }
}
