int main() {
    int a = 5;
    int b = 10;
    int result = 0;

    char* msg = "GCC 95% Test";

    int* ptr_a = &a;
    int* ptr_b = &b;
    int temp = *ptr_a;
    *ptr_a = *ptr_b;
    *ptr_b = temp;

    int arr[3];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;

    if (a < b) {
        result = a + b;
    } else {
        result = a - b;
    }

    int i = 0;
    int sum = 0;
    while (i < 3) {
        sum = sum + arr[i];
        i = i + 1;
    }

    return result + sum;
}
