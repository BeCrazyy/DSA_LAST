#include <iostream>
#include <vector>
using namespace std;

/**
 * @brief Constructs a product array where prod[i] equals product of all elements except arr[i]
 * @param arr Input array of integers
 * @return vector<int> Product array of same size as input
 * 
 * Algorithm:
 * 1. First pass: Calculate left products (product of all elements to the left of each index)
 * 2. Second pass: Calculate right products on-the-fly and multiply with left products
 * 
 * Time Complexity: O(n) - Two passes through the array
 * Space Complexity: O(1) - Only using the output array (which doesn't count as extra space)
 */
vector<int> productExceptSelf(vector<int>& arr) {
    int n = arr.size();
    
    // Handle edge cases
    if (n == 0) return {};
    if (n == 1) {
        throw invalid_argument("Array of size 1 cannot have product except self");
    }
    
    vector<int> prod(n, 1);
    
    // First pass: Calculate left products
    // prod[i] will contain product of all elements to the left of arr[i]
    for (int i = 1; i < n; i++) {
        prod[i] = prod[i - 1] * arr[i - 1];
    }
    
    // Second pass: Calculate right products on-the-fly and multiply with left products
    int rightProduct = 1;
    for (int i = n - 1; i >= 0; i--) {
        prod[i] *= rightProduct;  // Multiply left product with right product
        rightProduct *= arr[i];   // Update right product for next iteration
    }
    
    return prod;
}



/**
 * @brief Utility function to print vector
 */
void printVector(const vector<int>& vec) {
    cout << "[";
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

int main() {
    // Test Case 1: Normal case with positive numbers
    vector<int> arr1 = {1, 2, 3, 4};
    cout << "Input: ";
    printVector(arr1);
    vector<int> result1 = productExceptSelf(arr1);
    cout << "Output: ";
    printVector(result1);
    cout << endl;
    
    // Test Case 2: Array with zeros
    vector<int> arr2 = {1, 0, 3, 4};
    cout << "Input: ";
    printVector(arr2);
    vector<int> result2 = productExceptSelf(arr2);
    cout << "Output: ";
    printVector(result2);
    cout << endl;
    
    // Test Case 3: Array with negative numbers
    vector<int> arr3 = {-1, 2, -3, 4};
    cout << "Input: ";
    printVector(arr3);
    vector<int> result3 = productExceptSelf(arr3);
    cout << "Output: ";
    printVector(result3);
    cout << endl;
    
    // Test Case 4: Empty array
    vector<int> arr4 = {};
    cout << "Input: ";
    printVector(arr4);
    vector<int> result4 = productExceptSelf(arr4);
    cout << "Output: ";
    printVector(result4);
    cout << endl;
    
    // Test Case 5: Array with single element (will throw exception)
    try {
        vector<int> arr5 = {5};
        cout << "Input: ";
        printVector(arr5);
        vector<int> result5 = productExceptSelf(arr5);
    } catch (const invalid_argument& e) {
        cout << "Exception: " << e.what() << endl;
    }
    
    return 0;
}

/*
PROBLEM DESCRIPTION:
===================

Given an array arr[] of n integers, construct a Product Array prod[] (of same size) 
such that prod[i] is equal to the product of all the elements of arr[] except arr[i].

CONSTRAINTS:
- Solve without division operator
- Time complexity: O(n)
- Space complexity: O(1) (output array doesn't count as extra space)

EXAMPLES:
Input:  [1, 2, 3, 4]
Output: [24, 12, 8, 6]
Explanation: 
- prod[0] = 2 * 3 * 4 = 24
- prod[1] = 1 * 3 * 4 = 12
- prod[2] = 1 * 2 * 4 = 8
- prod[3] = 1 * 2 * 3 = 6

Input:  [1, 0, 3, 4]
Output: [0, 12, 0, 0]
Explanation:
- prod[0] = 0 * 3 * 4 = 0
- prod[1] = 1 * 3 * 4 = 12
- prod[2] = 1 * 0 * 4 = 0
- prod[3] = 1 * 0 * 3 = 0

EDGE CASES TO CONSIDER:
- Empty array: Return empty array
- Single element array: Cannot compute product except self (throw exception)
- Array with zeros: Handle carefully
- Array with negative numbers: Should work normally

ALGORITHM APPROACH:
1. Create output array initialized with 1s
2. First pass (left to right): Calculate left products
   - For each position i, store product of all elements to its left
3. Second pass (right to left): Calculate right products on-the-fly
   - Multiply existing left product with running right product
   - Update running right product for next iteration

WHY THIS APPROACH WORKS:
- Left products give us product of all elements before current index
- Right products give us product of all elements after current index
- Multiplying them gives product of all elements except current index
- No division needed, handles zeros naturally

TIME COMPLEXITY: O(n) - Two passes through array
SPACE COMPLEXITY: O(1) - Only using output array (doesn't count as extra space)
*/