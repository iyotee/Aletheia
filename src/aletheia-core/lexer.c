/*
 * ALETHEIA-Core: Simplified Lexer Implementation
 */

#include "lexer.h"
#include "core.h"

/* Keywords */
static struct {
    char* word;
    TokenType type;
} keywords[] = {
    {"int", TOK_INT},
    {"char", TOK_CHAR},
    {"void", TOK_VOID},
    {"return", TOK_RETURN},
    {"if", TOK_IF},
    {"else", TOK_ELSE},
    {"while", TOK_WHILE},
    {0, TOK_EOF}
};

/* Create lexer */
Lexer* create_lexer(char* source) {
    Lexer* lexer = core_malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->pos = 0;
    lexer->line = 1;
    return lexer;
}

/* Free lexer */
void free_lexer(Lexer* lexer) {
    core_free(lexer);
}

/* Check if character is whitespace */
static bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/* Check if character is digit */
static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

/* Check if character is alpha */
static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/* Check if character is alphanumeric */
static bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

/* Peek at current character */
static char peek(Lexer* lexer) {
    return lexer->source[lexer->pos];
}

/* Advance to next character */
static void advance(Lexer* lexer) {
    if (peek(lexer) == '\n') {
        lexer->line++;
    }
    lexer->pos++;
}

/* Check if at end of input */
static bool is_at_end(Lexer* lexer) {
    return peek(lexer) == '\0';
}

/* Create token */
static Token* make_token(TokenType type, char* value, int line) {
    Token* token = core_malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    token->line = line;
    return token;
}

/* Skip whitespace */
static void skip_whitespace(Lexer* lexer) {
    while (!is_at_end(lexer) && is_whitespace(peek(lexer))) {
        advance(lexer);
    }
}

/* Read identifier */
static char* read_identifier(Lexer* lexer) {
    int start = lexer->pos;
    while (!is_at_end(lexer) && is_alnum(peek(lexer))) {
        advance(lexer);
    }

    int length = lexer->pos - start;
    char* ident = core_malloc(length + 1);
    for (int i = 0; i < length; i++) {
        ident[i] = lexer->source[start + i];
    }
    ident[length] = '\0';

    return ident;
}

/* Read number */
static char* read_number(Lexer* lexer) {
    int start = lexer->pos;
    while (!is_at_end(lexer) && is_digit(peek(lexer))) {
        advance(lexer);
    }

    int length = lexer->pos - start;
    char* num = core_malloc(length + 1);
    for (int i = 0; i < length; i++) {
        num[i] = lexer->source[start + i];
    }
    num[length] = '\0';

    return num;
}

/* Check if identifier is keyword */
static TokenType check_keyword(char* ident) {
    for (int i = 0; keywords[i].word != 0; i++) {
        bool match = true;
        char* k = keywords[i].word;
        char* w = ident;
        while (*k && *w) {
            if (*k != *w) {
                match = false;
                break;
            }
            k++;
            w++;
        }
        if (match && *k == '\0' && *w == '\0') {
            return keywords[i].type;
        }
    }
    return TOK_IDENT;
}

/* Get next token */
Token* next_token(Lexer* lexer) {
    skip_whitespace(lexer);

    if (is_at_end(lexer)) {
        return make_token(TOK_EOF, 0, lexer->line);
    }

    char c = peek(lexer);

    /* Single character tokens */
    switch (c) {
        case '(': advance(lexer); return make_token(TOK_LPAREN, 0, lexer->line);
        case ')': advance(lexer); return make_token(TOK_RPAREN, 0, lexer->line);
        case '{': advance(lexer); return make_token(TOK_LBRACE, 0, lexer->line);
        case '}': advance(lexer); return make_token(TOK_RBRACE, 0, lexer->line);
        case ';': advance(lexer); return make_token(TOK_SEMI, 0, lexer->line);
        case ',': advance(lexer); return make_token(TOK_COMMA, 0, lexer->line);
        case '+': advance(lexer); return make_token(TOK_PLUS, 0, lexer->line);
        case '-': advance(lexer); return make_token(TOK_MINUS, 0, lexer->line);
        case '*': advance(lexer); return make_token(TOK_STAR, 0, lexer->line);
        case '/': advance(lexer); return make_token(TOK_SLASH, 0, lexer->line);
        case '=':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOK_EQ, 0, lexer->line);
            }
            return make_token(TOK_ASSIGN, 0, lexer->line);
        case '<': advance(lexer); return make_token(TOK_LT, 0, lexer->line);
        case '>': advance(lexer); return make_token(TOK_GT, 0, lexer->line);
    }

    /* Numbers */
    if (is_digit(c)) {
        char* num = read_number(lexer);
        return make_token(TOK_NUM, num, lexer->line);
    }

    /* Identifiers and keywords */
    if (is_alpha(c)) {
        char* ident = read_identifier(lexer);
        TokenType type = check_keyword(ident);
        return make_token(type, ident, lexer->line);
    }

    /* String literals (simplified) */
    if (c == '"') {
        advance(lexer); /* skip opening quote */
        int start = lexer->pos;
        while (!is_at_end(lexer) && peek(lexer) != '"') {
            advance(lexer);
        }
        if (is_at_end(lexer)) {
            core_error("Unterminated string");
        }
        advance(lexer); /* skip closing quote */

        int length = lexer->pos - start - 1; /* -1 for closing quote */
        char* str = core_malloc(length + 1);
        for (int i = 0; i < length; i++) {
            str[i] = lexer->source[start + i];
        }
        str[length] = '\0';

        return make_token(TOK_STR, str, lexer->line);
    }

    /* Unknown character */
    advance(lexer);
    return make_token(TOK_EOF, 0, lexer->line); /* Error token */
}

/* Free token */
void free_token(Token* token) {
    if (token->value) {
        core_free(token->value);
    }
    core_free(token);
}

/* Token type name for debugging */
const char* token_type_name(TokenType type) {
    switch (type) {
        case TOK_EOF: return "EOF";
        case TOK_INT: return "int";
        case TOK_CHAR: return "char";
        case TOK_VOID: return "void";
        case TOK_RETURN: return "return";
        case TOK_IF: return "if";
        case TOK_ELSE: return "else";
        case TOK_WHILE: return "while";
        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_SEMI: return ";";
        case TOK_COMMA: return ",";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_STAR: return "*";
        case TOK_SLASH: return "/";
        case TOK_EQ: return "==";
        case TOK_LT: return "<";
        case TOK_GT: return ">";
        case TOK_ASSIGN: return "=";
        case TOK_IDENT: return "identifier";
        case TOK_NUM: return "number";
        case TOK_STR: return "string";
        default: return "unknown";
    }
}

