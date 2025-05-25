#include <iostream>
#include <vector>
#include <string>
#include <random>

using StringVector = std::vector<std::string>;
const int alphabet = 256;
const int switch_to_quick = 74;

void swapStrings(std::string& a, std::string& b) {
    std::swap(a, b);
}

void moveStringsWithCurrentDepthToFront(StringVector& strings, int start, int end, int depth) {
    int insert_pos = start;
    int current = start;
    
    while (current <= end) {
        if (strings[current].length() == static_cast<size_t>(depth)) {
            swapStrings(strings[insert_pos], strings[current]);
            insert_pos++;
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
    
    moveStringsWithCurrentDepthToFront(strings, start, end, depth);
    
    int first_long_string = start;
    while (first_long_string <= end && 
           strings[first_long_string].length() == static_cast<size_t>(depth)) {
        first_long_string++;
    }
    
    if (first_long_string > end) return;
    
    int lower, upper;
    partitionByPivot(strings, first_long_string, end, depth, lower, upper);
    
    ternaryQuickSort(strings, first_long_string, lower - 1, depth);
    ternaryQuickSort(strings, lower, upper, depth + 1);
    ternaryQuickSort(strings, upper + 1, end, depth);
}

void countCharacterFrequencies(const StringVector& strings, int start, int end, 
                             int depth, std::vector<int>& count) {
    int current = start;
    while (current <= end) {
        unsigned char current_char = strings[current][depth];
        count[current_char + 1]++;
        current++;
    }
}

void computePrefixSums(std::vector<int>& count) {
    int i = 1;
    while (i <= alphabet) {
        count[i] += count[i - 1];
        i++;
    }
}

void distributeStrings(const StringVector& strings, int start, int end, int depth,
                     std::vector<int>& count, StringVector& temp_buffer) {
    std::vector<int> pos(count.begin(), count.begin() + alphabet);
    
    int current = start;
    while (current <= end) {
        unsigned char current_char = strings[current][depth];
        temp_buffer[pos[current_char]] = strings[current];
        pos[current_char]++;
        current++;
    }
}

void msdRadixSort(StringVector& strings, int start, int end, int depth) {
    if (start >= end) return;
    
    if ((end - start + 1) < switch_to_quick) {
        ternaryQuickSort(strings, start, end, depth);
        return;
    }
    
    moveStringsWithCurrentDepthToFront(strings, start, end, depth);
    
    int first_long_string = start;
    while (first_long_string <= end && 
           strings[first_long_string].length() == static_cast<size_t>(depth)) {
        first_long_string++;
    }
    
    if (first_long_string > end) return;
    
    std::vector<int> count(alphabet + 1, 0);
    const int segment_length = end - first_long_string + 1;
    StringVector temp_buffer(segment_length);
    
    countCharacterFrequencies(strings, first_long_string, end, depth, count);
    computePrefixSums(count);
    distributeStrings(strings, first_long_string, end, depth, count, temp_buffer);
    
    int i = 0;
    while (i < segment_length) {
        strings[first_long_string + i] = temp_buffer[i];
        i++;
    }
    
    int char_value = 0;
    while (char_value < alphabet) {
        int segment_start = first_long_string + count[char_value];
        int segment_end = first_long_string + count[char_value + 1] - 1;
        msdRadixSort(strings, segment_start, segment_end, depth + 1);
        char_value++;
    }
}

StringVector readInputStrings() {
    int string_count;
    std::cin >> string_count;
    
    StringVector strings;
    if (string_count > 0) {
        strings.resize(string_count);
        int i = 0;
        while (i < string_count) {
            std::cin >> strings[i];
            i++;
        }
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
        msdRadixSort(strings, 0, strings.size() - 1, 0);
        printSortedStrings(strings);
    }
    
    return 0;
}