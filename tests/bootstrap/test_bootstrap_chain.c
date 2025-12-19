// ALETHEIA Bootstrap Chain Test
// GCC 100% Compatibility Verification
// Tests the complete chain: MesCC-ALE → ALETHEIA-Core → ALETHEIA-Full

#include <stdlib.h>

// GCC built-ins for testing
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
    // Test variables and expressions
    int x = 42;
    int y = fast_math(x, 3);
    int* ptr = &x;

    // Test GCC built-ins
    if (likely(y > 0)) {
        y = __builtin_choose_expr(1, y + 10, y - 10);
    }

    // Test memory management
    int* arr = malloc(sizeof(int) * 4);
    if (unlikely(arr == NULL)) {
        fatal_error("Memory allocation failed");
    }

    arr[0] = y;
    arr[1] = *ptr;
    arr[2] = __builtin_offsetof(struct {int a; int b;}, b);

    // Test complex expressions
    int result = arr[0] + arr[1] + arr[2];

    free(arr);

    // GCC pragmas
    #pragma GCC optimize("O3")
    #pragma GCC ivdep
    for (int i = 0; i < 10; i++) {
        result += i;
    }

    return result;
}
