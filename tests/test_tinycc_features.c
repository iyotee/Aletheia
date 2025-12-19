
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

struct Point {
    int x;
    int y;
};

char get_char() {
    return 65;
}

long get_long() {
    return 123456789;
}

int add(int* ptr, int value) {
    return *ptr + value;
}

int main() {
    struct Point p;
    char c = get_char();
    long l = get_long();
    int arr[5];
    int x = 42;
    int* ptr = &x;
    int result = add(ptr, 8);
    
    if (result > 40) {
        return result;
    } else {
        return 0;
    }
}

