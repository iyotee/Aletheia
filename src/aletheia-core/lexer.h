/*
 * ALETHEIA-Core: Simplified Lexer
 */

#ifndef LEXER_H
#define LEXER_H

#include "core.h"

/* Token types (simplified for bootstrap) */
typedef enum {
    TOK_EOF = 0,
    TOK_INT, TOK_CHAR, TOK_VOID,  /* Types */
    TOK_RETURN, TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_SEMI, TOK_COMMA,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQ, TOK_LT, TOK_GT,
    TOK_ASSIGN,  /* = */
    TOK_IDENT, TOK_NUM,
    TOK_STR,    /* String literals */
} TokenType;

/* Token structure */
typedef struct {
    TokenType type;
    char* value;
    int line;
} Token;

/* Lexer state */
typedef struct {
    char* source;
    int pos;
    int line;
} Lexer;

/* Functions */
Lexer* create_lexer(char* source);
void free_lexer(Lexer* lexer);

Token* next_token(Lexer* lexer);
void free_token(Token* token);

/* Token utilities */
const char* token_type_name(TokenType type);

#endif /* LEXER_H */

