#include <iostream>
#include <vector>
#include <string>
#include <random>

using StringVector = std::vector<std::string>;

void swapStrings(std::string& a, std::string& b) {
    std::swap(a, b);
}

void moveShortStringsToFront(StringVector& strings, int start, int end, int depth) {
    int insertion_pos = start;
    int current = start;
    
    while (current <= end) {
        if (strings[current].length() == static_cast<size_t>(depth)) {
            swapStrings(strings[insertion_pos], strings[current]);
            insertion_pos++;
        }
        current++;
    }
}

void partitionByPivot(StringVector& strings, int left, int right, int depth, 
                     int& lower_bound, int& upper_bound) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(left, right);
    
    int pivot_index = dist(gen);
    swapStrings(strings[left], strings[pivot_index]);
    char pivot_char = strings[left][depth];
    
    lower_bound = left;
    upper_bound = right;
    int current = left + 1;
    
    while (current <= upper_bound) {
        char current_char = strings[current][depth];
        
        if (current_char < pivot_char) {
            swapStrings(strings[lower_bound], strings[current]);
            lower_bound++;
            current++;
        } else if (current_char > pivot_char) {
            swapStrings(strings[current], strings[upper_bound]);
            upper_bound--;
        } else {
            current++;
        }
    }
}

void ternaryQuickSort(StringVector& strings, int start, int end, int depth) {
    if (start >= end) return;
    
    moveShortStringsToFront(strings, start, end, depth);
    
    int first_long = start;
    while (first_long <= end && strings[first_long].length() == static_cast<size_t>(depth)) {
        first_long++;
    }
    
    if (first_long > end) return;
    
    int lower, upper;
    partitionByPivot(strings, first_long, end, depth, lower, upper);
    
    ternaryQuickSort(strings, first_long, lower - 1, depth);
    ternaryQuickSort(strings, lower, upper, depth + 1);
    ternaryQuickSort(strings, upper + 1, end, depth);
}

StringVector readInputStrings() {
    int count;
    std::cin >> count;
    
    StringVector strings(count);
    int i = 0;
    while (i < count) {
        std::cin >> strings[i];
        i++;
    }
    return strings;
}

void printSortedStrings(const StringVector& strings) {
    int i = 0;
    while (i < strings.size()) {
        std::cout << strings[i] << '\n';
        i++;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    StringVector strings = readInputStrings();
    
    if (!strings.empty()) {
        ternaryQuickSort(strings, 0, strings.size() - 1, 0);
        printSortedStrings(strings);
    }
    
    return 0;
}