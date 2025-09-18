#include <iostream>
#include <vector>
#include <cassert>

class TShirtSorter {
public:
    // Approach 1: Counting Sort - O(n) time, O(1) space
    static std::vector<char> countingSort(std::vector<char> sizes) {
        // Count occurrences of each size
        int countS = 0, countM = 0, countL = 0;
        
        for (char size : sizes) {
            if (size == 'S') countS++;
            else if (size == 'M') countM++;
            else if (size == 'L') countL++;
        }
        
        // Reconstruct the sorted array
        std::vector<char> result;
        result.reserve(sizes.size());
        
        // Add all S's, then M's, then L's
        for (int i = 0; i < countS; i++) result.push_back('S');
        for (int i = 0; i < countM; i++) result.push_back('M');
        for (int i = 0; i < countL; i++) result.push_back('L');
        
        return result;
    }
    
    // Approach 2: Dutch National Flag (Three-way Partitioning) - O(n) time, O(1) space
    static std::vector<char> dutchPartitioning(std::vector<char> sizes) {
        int low = 0, mid = 0, high = sizes.size() - 1;
        
        while (mid <= high) {
            if (sizes[mid] == 'S') {
                std::swap(sizes[low], sizes[mid]);
                low++;
                mid++;
            }
            else if (sizes[mid] == 'M') {
                mid++;
            }
            else { // sizes[mid] == 'L'
                std::swap(sizes[mid], sizes[high]);
                high--;
                // Don't increment mid here as we need to check the swapped element
            }
        }
        
        return sizes;
    }
};

// Unit Tests
void runTests() {
    std::cout << "Running Unit Tests...\n";
    
    // Test case 1: Example from problem
    std::vector<char> test1 = {'S', 'L', 'L', 'M', 'S'};
    std::vector<char> expected1 = {'S', 'S', 'M', 'L', 'L'};
    
    auto result1_counting = TShirtSorter::countingSort(test1);
    auto result1_dutch = TShirtSorter::dutchPartitioning(test1);
    
    assert(result1_counting == expected1);
    assert(result1_dutch == expected1);
    std::cout << "✓ Test 1 passed\n";
    
    // Test case 2: Already sorted
    std::vector<char> test2 = {'S', 'S', 'M', 'M', 'L', 'L'};
    std::vector<char> expected2 = {'S', 'S', 'M', 'M', 'L', 'L'};
    
    auto result2_counting = TShirtSorter::countingSort(test2);
    auto result2_dutch = TShirtSorter::dutchPartitioning(test2);
    
    assert(result2_counting == expected2);
    assert(result2_dutch == expected2);
    std::cout << "✓ Test 2 passed\n";
    
    // Test case 3: Reverse sorted
    std::vector<char> test3 = {'L', 'L', 'M', 'M', 'S', 'S'};
    std::vector<char> expected3 = {'S', 'S', 'M', 'M', 'L', 'L'};
    
    auto result3_counting = TShirtSorter::countingSort(test3);
    auto result3_dutch = TShirtSorter::dutchPartitioning(test3);
    
    assert(result3_counting == expected3);
    assert(result3_dutch == expected3);
    std::cout << "✓ Test 3 passed\n";
    
    // Test case 4: Single element
    std::vector<char> test4 = {'M'};
    std::vector<char> expected4 = {'M'};
    
    auto result4_counting = TShirtSorter::countingSort(test4);
    auto result4_dutch = TShirtSorter::dutchPartitioning(test4);
    
    assert(result4_counting == expected4);
    assert(result4_dutch == expected4);
    std::cout << "✓ Test 4 passed\n";
    
    // Test case 5: All same size
    std::vector<char> test5 = {'M', 'M', 'M', 'M'};
    std::vector<char> expected5 = {'M', 'M', 'M', 'M'};
    
    auto result5_counting = TShirtSorter::countingSort(test5);
    auto result5_dutch = TShirtSorter::dutchPartitioning(test5);
    
    assert(result5_counting == expected5);
    assert(result5_dutch == expected5);
    std::cout << "✓ Test 5 passed\n";
    
    // Test case 6: Empty array
    std::vector<char> test6 = {};
    std::vector<char> expected6 = {};
    
    auto result6_counting = TShirtSorter::countingSort(test6);
    auto result6_dutch = TShirtSorter::dutchPartitioning(test6);
    
    assert(result6_counting == expected6);
    assert(result6_dutch == expected6);
    std::cout << "✓ Test 6 passed\n";
    
    std::cout << "All tests passed! ✓\n\n";
}

void printVector(const std::vector<char>& vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        std::cout << vec[i];
        if (i < vec.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

int main() {
    // Run unit tests
    runTests();
    
    // Example usage
    std::vector<char> sizes = {'S', 'L', 'L', 'M', 'S'};
    
    std::cout << "Original: ";
    printVector(sizes);
    
    auto result1 = TShirtSorter::countingSort(sizes);
    std::cout << "Counting Sort: ";
    printVector(result1);
    
    auto result2 = TShirtSorter::dutchPartitioning(sizes);
    std::cout << "Dutch Partitioning: ";
    printVector(result2);
    
    return 0;
}

/*
PROBLEM DESCRIPTION:

Given an array of T-shirt sizes consisting of 'S', 'M', and 'L', sort them by size.

Example:
Input: ['S', 'L', 'L', 'M', 'S']
Output: ['S', 'S', 'M', 'L', 'L']

This problem is equivalent to sorting an array of 0s, 1s, and 2s without using any 
built-in sort function in linear time.

SOLUTION APPROACHES:

1. Counting Sort:
   - Time Complexity: O(n)
   - Space Complexity: O(1) - only 3 counters needed
   - Count occurrences of each size, then reconstruct array

2. Dutch National Flag (Three-way Partitioning):
   - Time Complexity: O(n)
   - Space Complexity: O(1) - in-place sorting
   - Use three pointers to partition array in single pass
   - Named after Dutch flag colors: red, white, blue

FOLLOW-UP EXTENSIONS:
1. More sizes: Add "XS", "XL" - tests scalability of solution
2. Stable sort: Input becomes tuples (size, RFID) where RFID is unique
   - Requires pigeonhole sort or pigeonhole-in-place sort
   - Tests understanding of stability in sorting algorithms

This problem tests:
- Algorithm selection based on constraints
- Understanding of linear-time sorting techniques
- Code optimization and space complexity analysis
- Ability to recognize special cases (only 3 distinct values)
*/