#include <iostream>
#include <vector>
#include <string>
#include <utility>

using StringWithLCP = std::pair<std::string, int>;

std::pair<int, int> compareStringsByLCP(const std::string& first_str, 
                                      const std::string& second_str, 
                                      int start_from) {
    int lcp_length = start_from;
    const int first_len = first_str.length();
    const int second_len = second_str.length();
    
    while (lcp_length < first_len && 
           lcp_length < second_len && 
           first_str[lcp_length] == second_str[lcp_length]) {
        lcp_length++;
    }
    
    if (lcp_length == first_len && lcp_length == second_len) 
        return {0, lcp_length};
    if (lcp_length == first_len) 
        return {-1, lcp_length};
    if (lcp_length == second_len) 
        return {1, lcp_length};
    
    return (first_str[lcp_length] < second_str[lcp_length]) ? 
           std::make_pair(-1, lcp_length) : 
           std::make_pair(1, lcp_length);
}

void copyStringRange(const std::vector<StringWithLCP>& source,
                    std::vector<StringWithLCP>& destination,
                    int start_idx, 
                    int end_idx) {
    int current_pos = 0;
    while (start_idx <= end_idx) {
        destination[current_pos] = source[start_idx];
        start_idx++;
        current_pos++;
    }
}

void mergeSortedParts(std::vector<StringWithLCP>& strings_array,
                     int left_start,
                     int middle_pos,
                     int right_end) {
    const int left_size = middle_pos - left_start + 1;
    const int right_size = right_end - middle_pos;
    
    std::vector<StringWithLCP> left_part(left_size);
    std::vector<StringWithLCP> right_part(right_size);
    
    copyStringRange(strings_array, left_part, left_start, middle_pos);
    copyStringRange(strings_array, right_part, middle_pos + 1, right_end);
    
    int left_idx = 0;
    int right_idx = 0;
    int current_pos = left_start;
    
    while (left_idx < left_size && right_idx < right_size) {
        if (left_part[left_idx].second > right_part[right_idx].second) {
            strings_array[current_pos] = left_part[left_idx];
            left_idx++;
        } 
        else if (left_part[left_idx].second < right_part[right_idx].second) {
            strings_array[current_pos] = right_part[right_idx];
            right_idx++;
        } 
        else {
            auto [comparison_result, new_lcp] = 
                compareStringsByLCP(left_part[left_idx].first, 
                                  right_part[right_idx].first, 
                                  left_part[left_idx].second);
            
            if (comparison_result == -1) {
                strings_array[current_pos] = left_part[left_idx];
                left_idx++;
                if (right_idx < right_size) 
                    right_part[right_idx].second = new_lcp;
            } 
            else {
                strings_array[current_pos] = right_part[right_idx];
                right_idx++;
                if (left_idx < left_size) 
                    left_part[left_idx].second = new_lcp;
            }
        }
        current_pos++;
    }
    
    while (left_idx < left_size) {
        strings_array[current_pos] = left_part[left_idx];
        left_idx++;
        current_pos++;
    }
    
    while (right_idx < right_size) {
        strings_array[current_pos] = right_part[right_idx];
        right_idx++;
        current_pos++;
    }
}

void performMergeSort(std::vector<StringWithLCP>& strings_array, 
                     int left_bound, 
                     int right_bound) {
    if (left_bound >= right_bound) return;
    
    const int middle = left_bound + (right_bound - left_bound) / 2;
    performMergeSort(strings_array, left_bound, middle);
    performMergeSort(strings_array, middle + 1, right_bound);
    mergeSortedParts(strings_array, left_bound, middle, right_bound);
}

std::vector<StringWithLCP> readInputStrings() {
    int string_count;
    std::cin >> string_count;
    
    if (string_count == 0) 
        return {};
    
    std::vector<StringWithLCP> input_strings(string_count);
    int idx = 0;
    while (idx < string_count) {
        std::cin >> input_strings[idx].first;
        input_strings[idx].second = 0;
        idx++;
    }
    return input_strings;
}

void printSortedStrings(const std::vector<StringWithLCP>& sorted_strings) {
    int idx = 0;
    while (idx < sorted_strings.size()) {
        std::cout << sorted_strings[idx].first << '\n';
        idx++;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    std::vector<StringWithLCP> strings_to_sort = readInputStrings();
    
    if (!strings_to_sort.empty()) {
        performMergeSort(strings_to_sort, 0, strings_to_sort.size() - 1);
        printSortedStrings(strings_to_sort);
    }
    
    return 0;
}