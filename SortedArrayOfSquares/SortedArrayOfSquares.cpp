#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Comparator function to sort by squares of elements
bool compareBySquare(int a, int b) {
    return (a * a) < (b * b);
}

// Approach 1: Using sorting with custom comparator - O(n log n)
vector<int> sortedSquaresComparator(vector<int>& nums) {
    vector<int> result = nums; // Copy the array
    
    // Sort using custom comparator
    sort(result.begin(), result.end(), compareBySquare);
    
    // Square all elements after sorting
    for (int& num : result) {
        num = num * num;
    }
    
    return result;
}

// Approach 2: Two pointer technique - O(n) - RECOMMENDED
vector<int> sortedSquares(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n);
    
    int left = 0, right = n - 1;
    
    // Fill result array from right to left (largest squares first)
    for (int pos = n - 1; pos >= 0; pos--) {
        // Compare absolute values to avoid computing squares twice
        if (abs(nums[left]) > abs(nums[right])) {
            result[pos] = nums[left] * nums[left];
            left++;
        } else {
            result[pos] = nums[right] * nums[right];
            right--;
        }
    }
    
    return result;
}

// Approach 3: Find K-th smallest square - O(log n + k)
int kthSmallestSquare(vector<int>& nums, int k) {
    int n = nums.size();
    
    // Step 1: Binary search to find first non-negative element - O(log n)
    int splitIndex = n; // Default: all elements are negative
    int left = 0, right = n - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] >= 0) {
            splitIndex = mid;
            right = mid - 1; // Look for earlier non-negative
        } else {
            left = mid + 1;
        }
    }
    
    // Step 2: Set up pointers for outward expansion - O(k)
    int leftPtr = splitIndex - 1;  // Rightmost negative (or -1 if no negatives)
    int rightPtr = splitIndex;     // Leftmost non-negative (or n if no positives)
    
    // Step 3: Expand outward for k steps
    int result = -1;
    for (int i = 0; i < k; i++) {
        // Check boundary conditions
        if (leftPtr < 0) {
            // Only positive numbers left
            result = nums[rightPtr];
            rightPtr++;
        } else if (rightPtr >= n) {
            // Only negative numbers left
            result = nums[leftPtr];
            leftPtr--;
        } else {
            // Compare squares of both sides
            int leftSquare = nums[leftPtr] * nums[leftPtr];
            int rightSquare = nums[rightPtr] * nums[rightPtr];
            
            if (leftSquare <= rightSquare) {
                result = nums[leftPtr];
                leftPtr--;
            } else {
                result = nums[rightPtr];
                rightPtr++;
            }
        }
    }
    
    return result;
}

// Test function for K-th smallest square
void testKthSmallest() {
    cout << "\n=== Testing K-th Smallest Square ===" << endl;
    
    vector<int> nums = {-7, -5, -3, -1, 2, 4, 8, 10};
    cout << "Array: [-7, -5, -3, -1, 2, 4, 8, 10]" << endl;
    cout << "Squares: [49, 25, 9, 1, 4, 16, 64, 100]" << endl;
    cout << "Sorted squares: [1, 4, 9, 16, 25, 49, 64, 100]" << endl << endl;
    
    for (int k = 1; k <= 5; k++) {
        int kthElement = kthSmallestSquare(nums, k);
        int kthSquare = kthElement * kthElement;
        cout << k << "-th smallest square: " << kthSquare 
             << " (from element " << kthElement << ")" << endl;
    }
    
    cout << endl;
}

int main() {
    // Test case 1: Mix of negative and positive numbers
    vector<int> nums1 = {-5, -3, -3, 2, 4, 4, 8};
    cout << "Input: [-5, -3, -3, 2, 4, 4, 8]" << endl;
    
    // Test both approaches
    vector<int> result1_sort = sortedSquaresComparator(nums1);
    vector<int> result1_opt = sortedSquares(nums1);
    
    cout << "Comparator approach: [";
    for (int i = 0; i < result1_sort.size(); i++) {
        cout << result1_sort[i];
        if (i < result1_sort.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    
    cout << "Two-pointer approach: [";
    for (int i = 0; i < result1_opt.size(); i++) {
        cout << result1_opt[i];
        if (i < result1_opt.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
    
    // Test case 2: All positive numbers
    vector<int> nums2 = {1, 5, 7, 7, 8, 10};
    cout << "Input: [1, 5, 7, 7, 8, 10]" << endl;
    
    vector<int> result2_sort = sortedSquaresComparator(nums2);
    vector<int> result2_opt = sortedSquares(nums2);
    
    cout << "Comparator approach: [";
    for (int i = 0; i < result2_sort.size(); i++) {
        cout << result2_sort[i];
        if (i < result2_sort.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    
    cout << "Two-pointer approach: [";
    for (int i = 0; i < result2_opt.size(); i++) {
        cout << result2_opt[i];
        if (i < result2_opt.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
    
    // Test case 3: All negative numbers
    vector<int> nums3 = {-7, -3, -1};
    cout << "Input: [-7, -3, -1]" << endl;
    
    vector<int> result3_sort = sortedSquaresComparator(nums3);
    vector<int> result3_opt = sortedSquares(nums3);
    
    cout << "Comparator approach: [";
    for (int i = 0; i < result3_sort.size(); i++) {
        cout << result3_sort[i];
        if (i < result3_sort.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
    
    cout << "Two-pointer approach: [";
    for (int i = 0; i < result3_opt.size(); i++) {
        cout << result3_opt[i];
        if (i < result3_opt.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
    
    // Test K-th smallest square function
    testKthSmallest();
    
    return 0;
}

/*
PROBLEM STATEMENT:
Given an integer array nums sorted in non-decreasing order, return an array 
of the squares of each number sorted in non-decreasing order.

Example 1:
Input: nums = [-4,-1,0,3,10]
Output: [0,1,9,16,100]

Example 2:
Input: nums = [-7,-3,2,3,11]
Output: [4,9,9,49,121]

FOLLOW-UP: Find K-th smallest square element

THREE APPROACHES:

APPROACH 1: Custom Comparator + Sorting
- Use custom comparator to sort by squares
- Then square all elements
- TIME COMPLEXITY: O(n log n) - due to sorting
- SPACE COMPLEXITY: O(n) - for result array

APPROACH 2: Two Pointer Technique (RECOMMENDED for full sorting)
- Use two pointers from both ends
- Compare absolute values and place larger square at rightmost position
- Fill result array from right to left
- TIME COMPLEXITY: O(n) - single pass
- SPACE COMPLEXITY: O(n) - for result array

APPROACH 3: K-th Smallest Square (Follow-up)
- Use binary search to find split point between negative/positive
- Expand outward from split point for k steps
- Compare squares and pick smaller one at each step
- TIME COMPLEXITY: O(log n + k) - O(log n) for split + O(k) for expansion
- SPACE COMPLEXITY: O(1) - only using pointers

KEY INSIGHTS:
1. Input array is sorted, but squares may not be sorted due to negative numbers
2. Largest squares will be either at leftmost (large negative) or 
   rightmost (large positive) positions
3. Two-pointer approach leverages the sorted property efficiently
4. For K-th element, binary search + outward expansion is most efficient

COMPARISON:
- Comparator: Easy to implement, but O(n log n)
- Two-pointer: More efficient O(n), leverages sorted input property
- K-th element: Most efficient O(log n + k) when k << n
*/