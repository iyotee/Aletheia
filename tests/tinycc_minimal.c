
// TINYCC-ALE BOOTSTRAP - Version ultra simple
// Just constants and structs that MesCC-ALE can handle

struct Token {
    int type;
    char* value;
    int line;
};

int main() {
    struct Token t;
    t.type = 1;  // TOK_INT
    return 42;
}

