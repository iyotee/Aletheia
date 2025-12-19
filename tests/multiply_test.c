
int multiply(int a, int b) {
    int result = 0;
    int i = 0;
    while (i < b) {
        result = result + a;
        i = i + 1;
    }
    return result;
}

int main() {
    int x = 6;
    int y = 7;
    int result = multiply(x, y);
    return result; // Should return 42
}

