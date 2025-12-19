
// Binary search algorithm - from Wikipedia/Standard C code
int binary_search(int arr[], int left, int right, int target) {
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid;
        }
        
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1; // target not found
}

int main() {
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int target = 7;
    int result = binary_search(arr, 0, 9, target);
    return result; // Should return 6 (index of 7)
}

