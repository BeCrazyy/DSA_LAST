#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

class Solution {
public:
    // Approach 1: Binary Search - O(m * log n) time, O(1) space
    int findFirstColumnBinarySearch(const std::vector<std::vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) {
            return -1; // No matrix or empty matrix
        }
        
        int m = matrix.size();
        int n = matrix[0].size();
        int firstCol = n; // Initialize to beyond last column
        
        // For each row, use binary search to find the first 1
        for (int i = 0; i < m; i++) {
            int left = 0, right = n - 1;
            int rowFirstOne = n; // Default to no 1 found in this row
            
            // Binary search for leftmost 1 in current row
            while (left <= right) {
                int mid = left + (right - left) / 2;
                if (matrix[i][mid] == 1) {
                    rowFirstOne = mid;
                    right = mid - 1; // Look for earlier 1s
                } else {
                    left = mid + 1;
                }
            }
            
            // Update global minimum
            firstCol = std::min(firstCol, rowFirstOne);
        }
        
        return (firstCol == n) ? -1 : firstCol; // Return -1 if no 1 found
    }
    
    // Approach 2: Top-Right Traversal - O(m + n) time, O(1) space
    int findFirstColumnTraversal(const std::vector<std::vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) {
            return -1;
        }
        
        int m = matrix.size();
        int n = matrix[0].size();
        
        // Start from top-right corner
        int row = 0;
        int col = n - 1;
        int firstCol = n; // Initialize to beyond last column
        
        while (row < m && col >= 0) {
            if (matrix[row][col] == 1) {
                // Found a 1, this could be our answer
                firstCol = col;
                col--; // Move left to find earlier 1s
            } else {
                // Found a 0, move down
                row++;
            }
        }
        
        return (firstCol == n) ? -1 : firstCol; // Return -1 if no 1 found
    }
};

// Unit Tests
void runTests() {
    Solution solution;
    
    // Test Case 1: Example from problem
    std::vector<std::vector<int>> matrix1 = {
        {0, 0, 0, 0, 1, 1, 1},
        {0, 0, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0}
    };
    
    int result1_bs = solution.findFirstColumnBinarySearch(matrix1);
    int result1_tr = solution.findFirstColumnTraversal(matrix1);
    
    std::cout << "Test 1 - Binary Search: " << result1_bs << " (Expected: 2)" << std::endl;
    std::cout << "Test 1 - Traversal: " << result1_tr << " (Expected: 2)" << std::endl;
    assert(result1_bs == 2);
    assert(result1_tr == 2);
    
    // Test Case 2: All zeros
    std::vector<std::vector<int>> matrix2 = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    
    int result2_bs = solution.findFirstColumnBinarySearch(matrix2);
    int result2_tr = solution.findFirstColumnTraversal(matrix2);
    
    std::cout << "Test 2 - Binary Search: " << result2_bs << " (Expected: -1)" << std::endl;
    std::cout << "Test 2 - Traversal: " << result2_tr << " (Expected: -1)" << std::endl;
    assert(result2_bs == -1);
    assert(result2_tr == -1);
    
    // Test Case 3: All ones
    std::vector<std::vector<int>> matrix3 = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };
    
    int result3_bs = solution.findFirstColumnBinarySearch(matrix3);
    int result3_tr = solution.findFirstColumnTraversal(matrix3);
    
    std::cout << "Test 3 - Binary Search: " << result3_bs << " (Expected: 0)" << std::endl;
    std::cout << "Test 3 - Traversal: " << result3_tr << " (Expected: 0)" << std::endl;
    assert(result3_bs == 0);
    assert(result3_tr == 0);
    
    // Test Case 4: First column has 1
    std::vector<std::vector<int>> matrix4 = {
        {1, 1, 1},
        {0, 1, 1},
        {0, 0, 1}
    };
    
    int result4_bs = solution.findFirstColumnBinarySearch(matrix4);
    int result4_tr = solution.findFirstColumnTraversal(matrix4);
    
    std::cout << "Test 4 - Binary Search: " << result4_bs << " (Expected: 0)" << std::endl;
    std::cout << "Test 4 - Traversal: " << result4_tr << " (Expected: 0)" << std::endl;
    assert(result4_bs == 0);
    assert(result4_tr == 0);
    
    // Test Case 5: Last column has 1
    std::vector<std::vector<int>> matrix5 = {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 0}
    };
    
    int result5_bs = solution.findFirstColumnBinarySearch(matrix5);
    int result5_tr = solution.findFirstColumnTraversal(matrix5);
    
    std::cout << "Test 5 - Binary Search: " << result5_bs << " (Expected: 2)" << std::endl;
    std::cout << "Test 5 - Traversal: " << result5_tr << " (Expected: 2)" << std::endl;
    assert(result5_bs == 2);
    assert(result5_tr == 2);
    
    // Test Case 6: Empty matrix
    std::vector<std::vector<int>> matrix6 = {};
    
    int result6_bs = solution.findFirstColumnBinarySearch(matrix6);
    int result6_tr = solution.findFirstColumnTraversal(matrix6);
    
    std::cout << "Test 6 - Binary Search: " << result6_bs << " (Expected: -1)" << std::endl;
    std::cout << "Test 6 - Traversal: " << result6_tr << " (Expected: -1)" << std::endl;
    assert(result6_bs == -1);
    assert(result6_tr == -1);
    
    std::cout << "\nAll tests passed!" << std::endl;
}

int main() {
    std::cout << "Running unit tests for Find First Column with 1 problem:\n" << std::endl;
    runTests();
    
    std::cout << "\nTime Complexity Analysis:" << std::endl;
    std::cout << "- Binary Search Approach: O(m * log n)" << std::endl;
    std::cout << "- Top-Right Traversal: O(m + n)" << std::endl;
    std::cout << "\nSpace Complexity: O(1) for both approaches" << std::endl;
    
    return 0;
}

/*
Problem Statement:
Given a two dimensional array that contains only 0s and 1s, and all 0s come before 
all 1s in any given row. Find the first column containing a 1.

Example input:
0, 0, 0, 0, 1, 1, 1
0, 0, 1, 1, 1, 1, 1  
0, 0, 0, 0, 0, 0, 0

Example output: 2

Key Observations:
1. Each row is sorted: all 0s come before all 1s
2. We need to find the leftmost column that contains at least one 1
3. Two main approaches:
   - Binary Search: For each row, find first 1 using upper_bound, take minimum
   - Top-Right Traversal: Start from top-right, move left on 1, down on 0

Algorithm Comparison:
- Binary Search: O(m * log n) time, good when n >> m
- Traversal: O(m + n) time, better when m and n are similar or m >> log n

Follow-up considerations:
- Multi-threading: Can parallelize binary search across rows
- Distributed: Partition rows across machines, find local minimum, then global minimum
- Scaling 10X: Traversal approach scales better for very large matrices
*/