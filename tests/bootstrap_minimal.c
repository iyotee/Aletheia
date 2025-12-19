// TINYCC BOOTSTRAP MINIMAL
// Juste pour prouver que MesCC-ALE peut compiler TinyCC

int add(int a, int b) {
    return a + b;
}

char get_char() {
    return 65;
}

long get_long() {
    return 123456789;
}

int main() {
    int sum = add(5, 3);
    char c = get_char();
    long l = get_long();
    
    if (sum > 0) {
        return sum;
    } else {
        return 0;
    }
}
