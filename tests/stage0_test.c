
// Test program that reads first char and returns different values
#include <stdio.h>
#include <unistd.h>

int main() {
    char c;
    ssize_t n = read(0, &c, 1);  // read from stdin
    if (n > 0 && c == 'H') {
        return 123;  // Recognized hex format
    }
    return 42;  // Default
}

