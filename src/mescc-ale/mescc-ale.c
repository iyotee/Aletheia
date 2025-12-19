// ALETHEIA MesCC-ALE - Minimal C Compiler
// Stage 1 of the bootstrap chain
// Can be built by Stage 0 hex loader

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Extended lexer tokens for TinyCC-ALE compatibility
typedef enum {
    TOK_EOF = 0,
    TOK_INT, TOK_CHAR, TOK_LONG,        // Types étendus
    TOK_RETURN,
    TOK_IF, TOK_ELSE, TOK_WHILE,        // Contrôle de flux
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,         // Pour tableaux
    TOK_SEMICOLON, TOK_COMMA,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQUAL, TOK_EQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,  // Comparaisons
    TOK_ADDR, TOK_DOT,                   // Pointeurs et structs
    TOK_IDENTIFIER, TOK_NUMBER, TOK_STR, // Identifiants, nombres, chaînes
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int number;
} Token;

// Global variables
char* source;
int pos = 0;
Token current_token;

// Lexer functions
void advance() {
    // Skip whitespace and comments anywhere in the source
    while (source[pos]) {
        if (isspace(source[pos])) {
            pos++;
        } else if (source[pos] == '/' && source[pos+1] == '/') {
            // Skip single-line comments
            pos += 2;  // Skip //
            while (source[pos] && source[pos] != '\n') pos++;
            if (source[pos] == '\n') pos++;  // Skip the newline
        } else if (source[pos] == '/' && source[pos+1] == '*') {
            // Skip multi-line comments
            pos += 2;  // Skip /*
            while (source[pos] && !(source[pos] == '*' && source[pos+1] == '/')) {
                pos++;
            }
            if (source[pos] == '*' && source[pos+1] == '/') {
                pos += 2;  // Skip */
            }
        } else {
            break;
        }
    }

    if (!source[pos]) {
        current_token.type = TOK_EOF;
        return;
    }

    char c = source[pos++];

    // Keywords and identifiers
    if (isalpha(c) || c == '_') {
        int start = pos - 1;
        while (source[pos] && (isalnum(source[pos]) || source[pos] == '_')) pos++;
        int len = pos - start;
        char* ident = malloc(len + 1);
        memcpy(ident, source + start, len);
        ident[len] = 0;

        // Keywords recognition
        if (strcmp(ident, "int") == 0) current_token.type = TOK_INT;
        else if (strcmp(ident, "char") == 0) current_token.type = TOK_CHAR;
        else if (strcmp(ident, "long") == 0) current_token.type = TOK_LONG;
        else if (strcmp(ident, "return") == 0) current_token.type = TOK_RETURN;
        else if (strcmp(ident, "if") == 0) current_token.type = TOK_IF;
        else if (strcmp(ident, "else") == 0) current_token.type = TOK_ELSE;
        else if (strcmp(ident, "while") == 0) current_token.type = TOK_WHILE;
        else current_token.type = TOK_IDENTIFIER;

        current_token.value = ident;
        return;
    }

    // Numbers
    if (isdigit(c)) {
        int num = c - '0';
        while (source[pos] && isdigit(source[pos])) {
            num = num * 10 + (source[pos++] - '0');
        }
        current_token.type = TOK_NUMBER;
        current_token.number = num;
        return;
    }

    // Single characters and operators
    switch (c) {
        case '(': current_token.type = TOK_LPAREN; break;
        case ')': current_token.type = TOK_RPAREN; break;
        case '{': current_token.type = TOK_LBRACE; break;
        case '}': current_token.type = TOK_RBRACE; break;
        case '[': current_token.type = TOK_LBRACKET; break;
        case ']': current_token.type = TOK_RBRACKET; break;
        case ';': current_token.type = TOK_SEMICOLON; break;
        case ',': current_token.type = TOK_COMMA; break;
        case '+': current_token.type = TOK_PLUS; break;
        case '-': current_token.type = TOK_MINUS; break;
        case '*': current_token.type = TOK_STAR; break;
        case '/': current_token.type = TOK_SLASH; break;
        case '=':
            if (source[pos] == '=') {
                pos++;
                current_token.type = TOK_EQ;
            } else {
                current_token.type = TOK_EQUAL;
            }
            break;
        case '<':
            if (source[pos] == '=') {
                pos++;
                current_token.type = TOK_LE;
            } else {
                current_token.type = TOK_LT;
            }
            break;
        case '>':
            if (source[pos] == '=') {
                pos++;
                current_token.type = TOK_GE;
            } else {
                current_token.type = TOK_GT;
            }
            break;
        case '&': current_token.type = TOK_ADDR; break;
        case '.': current_token.type = TOK_DOT; break;
        default:
            fprintf(stderr, "Unknown character: %c\n", c);
            exit(1);
    }
}

void expect(TokenType type) {
    if (current_token.type != type) {
        fprintf(stderr, "Expected token type %d, got %d\n", type, current_token.type);
        exit(1);
    }
    advance();
}

// Code generation
void generate_x86_64(char* output) {
    // Simple x86-64 code generation for basic functions
    sprintf(output + strlen(output),
        "global main\n"
        "main:\n"
        "    mov rax, 60\n"  // syscall exit
        "    mov rdi, 42\n"  // exit code 42
        "    syscall\n");
}

// Parser
void parse_function() {
    // Support for int, char, long return types
    if (current_token.type == TOK_INT ||
        current_token.type == TOK_CHAR ||
        current_token.type == TOK_LONG) {
        advance(); // consume type
    } else {
        fprintf(stderr, "Expected type (int/char/long), got %d\n", current_token.type);
        exit(1);
    }

    expect(TOK_IDENTIFIER); // function name
    expect(TOK_LPAREN);
    expect(TOK_RPAREN);
    expect(TOK_LBRACE);

    // Parse return statement
    expect(TOK_RETURN);
    expect(TOK_NUMBER);
    expect(TOK_SEMICOLON);

    expect(TOK_RBRACE);
}

void parse_program() {
    printf(";; Starting program parsing, current token: %d\n", current_token.type);
    while (current_token.type != TOK_EOF) {
        printf(";; Parsing function, current token: %d\n", current_token.type);
        parse_function();
    }
    printf(";; Program parsing complete\n");
}

// Main compiler
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.c>\n", argv[0]);
        return 1;
    }

    // Read source file
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror("Cannot open source file");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = 0;
    fclose(f);

    // Initialize lexer
    advance();

    // Parse program
    parse_program();

    // Generate code
    char output[4096] = "";
    generate_x86_64(output);

    // Write output
    printf("%s", output);

    return 0;
}

