
// Euclidean GCD algorithm - standard computer science
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int main() {
    int x = 48;
    int y = 18;
    int result = gcd(x, y);
    return result; // Should return 6
}

