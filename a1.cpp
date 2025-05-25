#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <utility>

class StringGenerator {
private:
    const std::vector<char> kValidChars = {
        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
        'P','Q','R','S','T','U','V','W','X','Y','Z',
        'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
        'p','q','r','s','t','u','v','w','x','y','z',
        '0','1','2','3','4','5','6','7','8','9',
        '!','@','#','%',':',';','^','&','*','(',')','-'
    };

    std::string BuildRandomString(int min_len = 10, int max_len = 200) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> len_dist(min_len, max_len);
        std::uniform_int_distribution<> char_dist(0, kValidChars.size()-1);
        
        int len = len_dist(gen);
        std::string s;
        s.reserve(len);
        
        for (int i = 0; i < len; ++i) {
            s.push_back(kValidChars[char_dist(gen)]);
        }
        return s;
    }

public:
    std::vector<std::string> CreateRandomDataset(int size) {
        std::vector<std::string> set(size);
        for (int i = 0; i < size; ++i) {
            set[i] = BuildRandomString();
        }
        return set;
    }

    std::vector<std::string> CreateReverseSortedDataset(int size) {
        auto set = CreateRandomDataset(size);
        std::sort(set.begin(), set.end());
        std::reverse(set.begin(), set.end());
        return set;
    }

    std::vector<std::string> CreateNearlySortedDataset(int size) {
        auto set = CreateRandomDataset(size);
        std::sort(set.begin(), set.end());
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, size-1);
        
        for (int i = 0; i < size/20; ++i) {
            std::swap(set[dist(gen)], set[dist(gen)]);
        }
        return set;
    }

    std::vector<std::string> CreatePrefixDataset(int size) {
        std::vector<std::string> set(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> prefix_len(5, 15);
        
        for (int i = 0; i < 10; ++i) {
            std::string prefix;
            int len = prefix_len(gen);
            for (int j = 0; j < len; ++j) {
                prefix.push_back(kValidChars[gen() % kValidChars.size()]);
            }
            
            for (int j = i*size/10; j < (i+1)*size/10; ++j) {
                set[j] = prefix + BuildRandomString(5, 185);
            }
        }
        
        std::shuffle(set.begin(), set.end(), gen);
        return set;
    }
};

class StringSortTester {
private:
    struct StringWithLCP {
        std::string str;
        int lcp;
    };

    struct PerformanceParams {
        long long microseconds;
        long long comparisons;
    };

    void Merge(std::vector<std::string>& arr, int l, int m, int r, long long& cmp_count) {
        int n1 = m - l + 1;
        int n2 = r - m;

        std::vector<std::string> L(n1), R(n2);

        for (int i = 0; i < n1; i++)
            L[i] = arr[l + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[m + 1 + j];

        int i = 0, j = 0, k = l;

        while (i < n1 && j < n2) {
            cmp_count++;
            if (L[i] <= R[j]) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }

        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    void MergeSort(std::vector<std::string>& arr, int l, int r, long long& cmp_count) {
        if (l >= r) return;
        int m = l + (r - l) / 2;
        MergeSort(arr, l, m, cmp_count);
        MergeSort(arr, m + 1, r, cmp_count);
        Merge(arr, l, m, r, cmp_count);
    }

    int Partition(std::vector<std::string>& arr, int low, int high, long long& cmp_count) {
        std::string pivot = arr[high];
        int i = low - 1;

        for (int j = low; j <= high - 1; j++) {
            cmp_count++;
            if (arr[j] <= pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    void QuickSort(std::vector<std::string>& arr, int low, int high, long long& cmp_count) {
        if (low < high) {
            int pi = Partition(arr, low, high, cmp_count);
            QuickSort(arr, low, pi - 1, cmp_count);
            QuickSort(arr, pi + 1, high, cmp_count);
        }
    }

    std::pair<int, int> CompareStrings(const std::string& a, const std::string& b, int depth, long long& cmp_count) {
        int i = depth;
        while (i < a.size() && i < b.size()) {
            cmp_count++;
            if (a[i] != b[i]) {
                return {(a[i] < b[i]) ? -1 : 1, i};
            }
            i++;
        }
        cmp_count++;
        if (i == a.size() && i == b.size()) return {0, i};
        if (i == a.size()) return {-1, i};
        return {1, i};
    }

    void MergeStrings(std::vector<StringWithLCP>& arr, int left, int mid, int right, long long& cmp_count) {
        std::vector<StringWithLCP> left_part(arr.begin() + left, arr.begin() + mid + 1);
        std::vector<StringWithLCP> right_part(arr.begin() + mid + 1, arr.begin() + right + 1);
        
        int i = 0, j = 0, k = left;
        while (i < left_part.size() && j < right_part.size()) {
            if (left_part[i].lcp > right_part[j].lcp) {
                arr[k++] = left_part[i++];
            }
            else if (left_part[i].lcp < right_part[j].lcp) {
                arr[k++] = right_part[j++];
            }
            else {
                auto [cmp, new_lcp] = CompareStrings(left_part[i].str, right_part[j].str, left_part[i].lcp, cmp_count);
                if (cmp == -1) {
                    arr[k++] = left_part[i++];
                    if (j < right_part.size()) right_part[j].lcp = new_lcp;
                }
                else {
                    arr[k++] = right_part[j++];
                    if (i < left_part.size()) left_part[i].lcp = new_lcp;
                }
            }
        }
        
        while (i < left_part.size()) arr[k++] = left_part[i++];
        while (j < right_part.size()) arr[k++] = right_part[j++];
    }

    void MergeSortStrings(std::vector<StringWithLCP>& arr, int left, int right, long long& cmp_count) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        MergeSortStrings(arr, left, mid, cmp_count);
        MergeSortStrings(arr, mid + 1, right, cmp_count);
        MergeStrings(arr, left, mid, right, cmp_count);
    }

    void TernaryStringQuickSort(std::vector<std::string>& arr, int left, int right, int depth, long long& cmp_count) {
        if (left >= right) return;
        
        int pivot_pos = left;
        for (int i = left; i <= right; ++i) {
            if (arr[i].length() == depth) {
                std::swap(arr[pivot_pos++], arr[i]);
            }
        }
        
        if (pivot_pos > right) return;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(pivot_pos, right);
        std::swap(arr[pivot_pos], arr[dist(gen)]);
        char pivot = arr[pivot_pos][depth];
        
        int lt = pivot_pos;
        int gt = right;
        int i = pivot_pos + 1;
        
        while (i <= gt) {
            cmp_count++;
            if (arr[i][depth] < pivot) {
                std::swap(arr[lt++], arr[i++]);
            }
            else if (arr[i][depth] > pivot) {
                std::swap(arr[i], arr[gt--]);
            }
            else {
                i++;
            }
        }
        
        TernaryStringQuickSort(arr, left, lt - 1, depth, cmp_count);
        TernaryStringQuickSort(arr, lt, gt, depth + 1, cmp_count);
        TernaryStringQuickSort(arr, gt + 1, right, depth, cmp_count);
    }

    void MSDRadixSort(std::vector<std::string>& arr, int left, int right, int depth, long long& cmp_count) {
        if (left >= right) return;
        
        int pivot_pos = left;
        for (int i = left; i <= right; ++i) {
            if (arr[i].length() == depth) {
                std::swap(arr[pivot_pos++], arr[i]);
            }
        }
        
        if (pivot_pos > right) return;
        
        const int kCharRange = 256;
        std::vector<int> count(kCharRange + 1, 0);
        
        for (int i = pivot_pos; i <= right; ++i) {
            cmp_count++;
            count[static_cast<unsigned char>(arr[i][depth]) + 1]++;
        }
        
        for (int i = 1; i <= kCharRange; ++i) {
            count[i] += count[i - 1];
        }
        
        std::vector<std::string> temp(right - pivot_pos + 1);
        for (int i = pivot_pos; i <= right; ++i) {
            cmp_count++;
            temp[count[arr[i][depth]]++] = arr[i];
        }
        
        for (int i = 0; i < temp.size(); ++i) {
            arr[pivot_pos + i] = temp[i];
        }
        
        for (int i = 0; i < kCharRange; ++i) {
            int new_left = pivot_pos + count[i];
            int new_right = pivot_pos + count[i + 1] - 1;
            MSDRadixSort(arr, new_left, new_right, depth + 1, cmp_count);
        }
    }

    void RadixQuickSort(std::vector<std::string>& arr, int left, int right, int depth, long long& cmp_count) {
        if (left >= right) return;
        
        if (right - left + 1 < 74) {
            TernaryStringQuickSort(arr, left, right, depth, cmp_count);
            return;
        }
        
        int pivot_pos = left;
        for (int i = left; i <= right; ++i) {
            if (arr[i].length() == depth) {
                std::swap(arr[pivot_pos++], arr[i]);
            }
        }
        
        if (pivot_pos > right) return;
        
        const int kCharRange = 256;
        std::vector<int> count(kCharRange + 1, 0);
        
        for (int i = pivot_pos; i <= right; ++i) {
            cmp_count++;
            count[static_cast<unsigned char>(arr[i][depth]) + 1]++;
        }
        
        for (int i = 1; i <= kCharRange; ++i) {
            count[i] += count[i - 1];
        }
        
        std::vector<std::string> temp(right - pivot_pos + 1);
        for (int i = pivot_pos; i <= right; ++i) {
            cmp_count++;
            temp[count[arr[i][depth]]++] = arr[i];
        }
        
        for (int i = 0; i < temp.size(); ++i) {
            arr[pivot_pos + i] = temp[i];
        }
        
        for (int i = 0; i < kCharRange; ++i) {
            int new_left = pivot_pos + count[i];
            int new_right = pivot_pos + count[i + 1] - 1;
            RadixQuickSort(arr, new_left, new_right, depth + 1, cmp_count);
        }
    }

    PerformanceParams TestStandardMergeSort(std::vector<std::string> data) {
        long long cmp_count = 0;
        auto start = std::chrono::high_resolution_clock::now();
        MergeSort(data, 0, data.size()-1, cmp_count);
        auto end = std::chrono::high_resolution_clock::now();
        
        return {
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
            cmp_count
        };
    }

    PerformanceParams TestStandardQuickSort(std::vector<std::string> data) {
        long long cmp_count = 0;
        auto start = std::chrono::high_resolution_clock::now();
        QuickSort(data, 0, data.size()-1, cmp_count);
        auto end = std::chrono::high_resolution_clock::now();
        
        return {
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
            cmp_count
        };
    }

    PerformanceParams TestStringMergeSort(std::vector<std::string> data) {
        std::vector<StringWithLCP> lcp_data(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            lcp_data[i] = {data[i], 0};
        }
        
        long long cmp_count = 0;
        auto start = std::chrono::high_resolution_clock::now();
        MergeSortStrings(lcp_data, 0, lcp_data.size()-1, cmp_count);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = lcp_data[i].str;
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        return {
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
            cmp_count
        };
    }

    PerformanceParams TestStringQuickSort(std::vector<std::string> data) {
        long long cmp_count = 0;
        auto start = std::chrono::high_resolution_clock::now();
        TernaryStringQuickSort(data, 0, data.size()-1, 0, cmp_count);
        auto end = std::chrono::high_resolution_clock::now();
        
        return {
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
            cmp_count
        };
    }

    PerformanceParams TestMSDRadixSort(std::vector<std::string> data) {
        long long cmp_count = 0;
        auto start = std::chrono::high_resolution_clock::now();
        MSDRadixSort(data, 0, data.size()-1, 0, cmp_count);
        auto end = std::chrono::high_resolution_clock::now();
        
        return {
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
            cmp_count
        };
    }

    PerformanceParams TestRadixQuickSort(std::vector<std::string> data) {
        long long cmp_count = 0;
        auto start = std::chrono::high_resolution_clock::now();
        RadixQuickSort(data, 0, data.size()-1, 0, cmp_count);
        auto end = std::chrono::high_resolution_clock::now();
        
        return {
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(),
            cmp_count
        };
    }

    void testDataset(int size, 
                    const std::vector<std::vector<std::string>>& datasets,
                    const std::vector<std::string>& labels, 
                    int runs,
                    std::ofstream& time_log, 
                    std::ofstream& comp_log) {
        for (size_t i = 0; i < labels.size(); ++i) {
            std::vector<long long> time_stats(6, 0);
            std::vector<long long> comp_stats(6, 0);
            
            for (int j = 0; j < runs; ++j) {
                auto res1 = TestStandardMergeSort(datasets[i]);
                auto res2 = TestStandardQuickSort(datasets[i]);
                auto res3 = TestStringMergeSort(datasets[i]);
                auto res4 = TestStringQuickSort(datasets[i]);
                auto res5 = TestMSDRadixSort(datasets[i]);
                auto res6 = TestRadixQuickSort(datasets[i]);
                
                time_stats[0] += res1.microseconds;
                time_stats[1] += res2.microseconds;
                time_stats[2] += res3.microseconds;
                time_stats[3] += res4.microseconds;
                time_stats[4] += res5.microseconds;
                time_stats[5] += res6.microseconds;
                
                comp_stats[0] += res1.comparisons;
                comp_stats[1] += res2.comparisons;
                comp_stats[2] += res3.comparisons;
                comp_stats[3] += res4.comparisons;
                comp_stats[4] += res5.comparisons;
                comp_stats[5] += res6.comparisons;
            }
            
            time_log << size << "," << labels[i];
            comp_log << size << "," << labels[i];
            
            for (int k = 0; k < 6; ++k) {
                time_log << "," << (time_stats[k] / runs);
                comp_log << "," << (comp_stats[k] / runs);
            }
            
            time_log << "\n";
            comp_log << "\n";
        }
    }

public:
    void RunTests() {
        StringGenerator dataGenerator;
        const std::vector<int> testSizes = {100,200,300,400,500,600,700,800,900,1000,
                                          1500,2000,2500,3000};
        const int numTestRuns = 3;
        
        std::ofstream timeResultsFile("microseconds_results.csv");
        std::ofstream compResultsFile("comparisons_results.csv");
        
        timeResultsFile << "Size,Type,StandardMerge,StandardQuick,CustomMerge,CustomQuick,Radix,RadixQuick\n";
        compResultsFile << "Size,Type,StandardMerge,StandardQuick,CustomMerge,CustomQuick,Radix,RadixQuick\n";
        
        for (int currentSize : testSizes) {
            std::cout << "Current size of dataset: " << currentSize << std::endl;
            
            std::vector<std::string> datasetTypes = {"Random", "Reverse", "NearlySorted", "Prefix"};
            std::vector<std::vector<std::string>> testData = {
                dataGenerator.CreateRandomDataset(currentSize),
                dataGenerator.CreateReverseSortedDataset(currentSize),
                dataGenerator.CreateNearlySortedDataset(currentSize),
                dataGenerator.CreatePrefixDataset(currentSize)
            };
            
            testDataset(currentSize, testData, datasetTypes, numTestRuns, timeResultsFile, compResultsFile);
        }
    }
};

int main() {
    StringSortTester tester;
    tester.RunTests();
    return 0;
}