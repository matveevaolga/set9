#include <iostream>
#include <vector>
#include <string>

using StringVector = std::vector<std::string>;

void moveStringsWithCurrentLengthToFront(StringVector& strings, int start, int end, int current_depth) {
    int insert_position = start;
    int current = start;
    
    while (current <= end) {
        if (strings[current].length() == static_cast<size_t>(current_depth)) {
            std::swap(strings[insert_position], strings[current]);
            insert_position++;
        }
        current++;
    }
}

void countCharacterFrequencies(const StringVector& strings, int start, int end, int depth, std::vector<int>& count) {
    int current = start;
    while (current <= end) {
        unsigned char current_char = strings[current][depth];
        count[current_char + 1]++;
        current++;
    }
}

void computePrefixSums(std::vector<int>& count) {
    int i = 1;
    while (i < 256) {
        count[i] += count[i - 1];
        i++;
    }
}

void distributeStringsToBuckets(const StringVector& strings, int start, int end, int depth, 
                               std::vector<int>& count, StringVector& temp_buffer) {
    std::vector<int> position(count.begin(), count.begin() + 256);
    
    int current = start;
    while (current <= end) {
        unsigned char current_char = strings[current][depth];
        temp_buffer[position[current_char]] = strings[current];
        position[current_char]++;
        current++;
    }
}

void copyBackFromBuffer(StringVector& strings, int start, const StringVector& temp_buffer, int length) {
    int i = 0;
    while (i < length) {
        strings[start + i] = temp_buffer[i];
        i++;
    }
}

void msdRadixSortRecursive(StringVector& strings, int start, int end, int depth) {
    if (start >= end) return;

    moveStringsWithCurrentLengthToFront(strings, start, end, depth);
    
    int first_long_string = start;
    while (first_long_string <= end && strings[first_long_string].length() == static_cast<size_t>(depth)) {
        first_long_string++;
    }
    
    if (first_long_string > end) return;

    const int bucket_count = 256;
    std::vector<int> count(bucket_count + 1, 0);
    const int segment_length = end - first_long_string + 1;
    StringVector temp_buffer(segment_length);

    countCharacterFrequencies(strings, first_long_string, end, depth, count);
    computePrefixSums(count);
    distributeStringsToBuckets(strings, first_long_string, end, depth, count, temp_buffer);
    copyBackFromBuffer(strings, first_long_string, temp_buffer, segment_length);

    int char_value = 0;
    while (char_value < bucket_count) {
        int segment_start = first_long_string + count[char_value];
        int segment_end = first_long_string + count[char_value + 1] - 1;
        msdRadixSortRecursive(strings, segment_start, segment_end, depth + 1);
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
        msdRadixSortRecursive(strings, 0, strings.size() - 1, 0);
        printSortedStrings(strings);
    }
    
    return 0;
}