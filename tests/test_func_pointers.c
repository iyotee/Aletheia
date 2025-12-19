
void set_value(int* ptr, int value) {
    *ptr = value;
}

int main() {
    int x = 0;
    set_value(&x, 123);
    return x;
}

