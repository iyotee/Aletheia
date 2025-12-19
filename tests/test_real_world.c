// Real-world test program - simulates parts of busybox functionality
// Tests string manipulation, file operations, basic utilities

#include <stdint.h>

// Simple string functions (from busybox-style implementations)
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return p - s;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

// Simple utility functions
int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int atoi(const char *s) {
    int result = 0;
    int sign = 1;

    if (*s == '-') {
        sign = -1;
        s++;
    }

    while (is_digit(*s)) {
        result = result * 10 + (*s - '0');
        s++;
    }

    return sign * result;
}

// Test function - similar to busybox 'echo'
int echo_test(const char *msg) {
    // In real busybox, this would write to stdout
    // For now, just return message length
    return strlen(msg);
}

// Test function - similar to busybox 'cat'
int cat_test(const char *str) {
    // Simulate processing a string (like cat would process a file)
    int count = 0;
    while (*str) {
        if (*str == '\n') count++;
        str++;
    }
    return count; // Return number of lines
}

// Main test
int main(int argc, char *argv[]) {
    // Test string functions
    char buffer[100];
    strcpy(buffer, "Hello ALETHEIA");
    int len = strlen(buffer);

    // Test atoi
    int num = atoi("42");

    // Test utilities
    int echo_result = echo_test("test message");
    int cat_result = cat_test("line1\nline2\nline3");

    // Return combined result for verification
    return len + num + echo_result + cat_result; // Should be 13 + 42 + 12 + 3 = 70
}

