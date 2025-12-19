// ALETHEIA Compiler Optimization Test
// Demonstrates constant folding and dead code elimination

int main() {
    int a = 5 + 3 * 2;  // Should fold to: a = 11
    int b = a - 1;       // Should fold to: b = 10

    if (1) {             // Constant condition, should eliminate else branch
        return b * 2;    // Should fold to: return 20
    } else {
        return 0;        // This should be eliminated
    }
}





