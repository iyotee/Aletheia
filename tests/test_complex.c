
// Complex C program to test ALETHEIA
#include <stdio.h>

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    int x = 5;
    int y = 8;
    
    printf("Factorial of %d: %d\n", x, factorial(x));
    printf("Fibonacci of %d: %d\n", y, fibonacci(y));
    
    for (int i = 0; i < 10; i++) {
        printf("Loop iteration: %d\n", i);
    }
    
    return 42;
}

