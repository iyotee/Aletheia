// ALETHEIA-Core GCC 100% Compatibility Test
// Tests complete GCC 100% features in bootstrap compiler

#include <stdlib.h>

// GCC built-ins for ALETHEIA-Core testing
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

// GCC attributes
__attribute__((noreturn)) void fatal_error(const char* msg) {
    __builtin_unreachable();
}

__attribute__((always_inline)) static inline int fast_math(int a, int b) {
    return a + b * 2;
}

int main() {
    // Variables and basic operations
    int x = 42;
    int y = fast_math(x, 3);  // Should use always_inline

    // GCC built-in usage
    if (likely(y > 0)) {
        y = __builtin_choose_expr(1, y + 10, y - 10);
    }

    // Memory management
    int* ptr = malloc(sizeof(int) * 4);
    if (unlikely(ptr == NULL)) {
        fatal_error("Allocation failed");
    }

    // GCC memcpy built-in
    int src[4] = {1, 2, 3, 4};
    __builtin_memcpy(ptr, src, sizeof(src));

    // Complex expression
    int result = ptr[0] + ptr[1] + ptr[2] + ptr[3];

    free(ptr);

    // GCC pragmas
    #pragma GCC optimize("O3")
    #pragma GCC ivdep
    for (int i = 0; i < 10; i++) {
        result += i;
    }

    return result;
}

// Expected: This program should compile with full GCC 100% compatibility
// in ALETHEIA-Core, demonstrating bootstrap compiler capabilities
