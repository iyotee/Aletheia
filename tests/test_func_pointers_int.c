
int set_value(int* ptr, int value) {
    *ptr = value;
    return value;
}

int main() {
    int x = 0;
    set_value(&x, 123);
    return x;
}

