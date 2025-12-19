// ALETHEIA AI Integration Test Program
// Tests the complete AI-enhanced compilation pipeline
// This program should be compiled with ALETHEIA-Full + AI

#include <stdlib.h>

// GCC built-ins that ALETHEIA AI should recognize
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

// AI should suggest inlining for this small function
__attribute__((always_inline))
static inline int fast_double(int x) {
    return x * 2;
}

// AI should suggest SIMD vectorization for this loop
void vector_add(float* a, float* b, float* result, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = a[i] + b[i];
    }
}

// AI should suggest cache blocking for this matrix operation
void matrix_multiply_optimized(float a[64][64], float b[64][64], float result[64][64]) {
    // AI should recognize this as a matrix multiplication pattern
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            result[i][j] = 0.0f;
            for (int k = 0; k < 64; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// AI should suggest memory pool optimization for this pattern
struct Node {
    int data;
    struct Node* next;
};

struct Node* create_list(int size) {
    struct Node* head = NULL;
    struct Node* current = NULL;

    for (int i = 0; i < size; i++) {
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        new_node->data = i;
        new_node->next = NULL;

        if (head == NULL) {
            head = new_node;
        } else {
            current->next = new_node;
        }
        current = new_node;
    }

    return head;
}

// AI should suggest recursion optimization for factorial
int factorial_recursive(int n) {
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);
}

// AI should suggest iteration instead of recursion
int factorial_iterative(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// AI should optimize this binary search
int binary_search_optimized(int arr[], int size, int target) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

// Main function with AI optimization opportunities
int main() {
    // AI should recognize likely/unlikely branch prediction
    int x = 42;
    if (likely(x > 0)) {
        x = fast_double(x);  // Should be inlined
    }

    // AI should optimize vector operations
    const int VECTOR_SIZE = 1000;
    float* a = (float*)malloc(VECTOR_SIZE * sizeof(float));
    float* b = (float*)malloc(VECTOR_SIZE * sizeof(float));
    float* result = (float*)malloc(VECTOR_SIZE * sizeof(float));

    // Initialize arrays
    for (int i = 0; i < VECTOR_SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)(i * 2);
    }

    vector_add(a, b, result, VECTOR_SIZE);  // Should be vectorized

    // AI should optimize matrix operations
    float matrix_a[64][64];
    float matrix_b[64][64];
    float matrix_result[64][64];

    // Initialize matrices
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            matrix_a[i][j] = (float)(i + j);
            matrix_b[i][j] = (float)(i * j);
        }
    }

    matrix_multiply_optimized(matrix_a, matrix_b, matrix_result);  // Should be cache-blocked

    // AI should optimize data structure operations
    struct Node* list = create_list(100);  // Should use memory pools

    // AI should optimize search operations
    int search_array[1000];
    for (int i = 0; i < 1000; i++) {
        search_array[i] = i * 2;
    }

    int found_index = binary_search_optimized(search_array, 1000, 42);  // Should be optimized

    // AI should recognize recursive vs iterative choice
    int fact_recursive = factorial_recursive(10);
    int fact_iterative = factorial_iterative(10);

    // Clean up
    free(a);
    free(b);
    free(result);

    // Free linked list
    struct Node* current = list;
    while (current != NULL) {
        struct Node* next = current->next;
        free(current);
        current = next;
    }

    return x + found_index + fact_recursive + fact_iterative;
}

// EXPECTED AI OPTIMIZATIONS:
// 1. Function inlining for fast_double()
// 2. SIMD vectorization for vector_add()
// 3. Cache blocking for matrix_multiply_optimized()
// 4. Memory pool optimization for create_list()
// 5. Loop unrolling for binary_search_optimized()
// 6. Branch prediction hints for likely/unlikely
// 7. GCC built-in function recognition
// 8. Tail recursion optimization for factorial_recursive()
//
// ALETHEIA AI should detect and apply these optimizations automatically
// during compilation, resulting in significantly improved performance
// compared to standard GCC -O3 compilation.
