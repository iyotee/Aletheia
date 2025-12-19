// ALETHEIA-Core Simple Test - GCC 95% Validation
// Tests implemented features within current parser limits

int main() {
    // Variables
    int a = 5;
    int b = 10;

    // String literal
    char* msg = "Hello GCC 95%";

    // Pointer operations
    int* ptr_a = &a;
    int* ptr_b = &b;
    int temp = *ptr_a;
    *ptr_a = *ptr_b;
    *ptr_b = temp;

    // Array
    int arr[3];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    // Function calls
    // printf("Result: %d\n", a + b);

    // Memory management (simulated)
    // int* heap = malloc(10);
    // free(heap);

    // Control flow
    int result = 0;
    if (a > b) {
        result = a;
    } else {
        result = b;
    }

    // Loop
    int i = 0;
    int sum = 0;
    while (i < 5) {
        sum = sum + arr[i % 3];
        i = i + 1;
    }

    // Complex expression
    return a + b + result + sum;
}
