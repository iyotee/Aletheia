
// Test TinyCC-ALE compilation by MesCC-ALE
// This is a simplified version without preprocessor directives

typedef enum {
    TOK_EOF = 0,
    TOK_INT, TOK_CHAR, TOK_LONG,
    TOK_RETURN,
    TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,
    TOK_SEMI, TOK_COMMA,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQUAL, TOK_EQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_ADDR, TOK_DOT,
    TOK_IDENT, TOK_NUM, TOK_STR
} TokenType;

int main() {
    TokenType t = TOK_INT;
    return 42;
}

