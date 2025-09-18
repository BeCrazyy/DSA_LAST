#include <iostream>
#include <vector>
using namespace std;

// Part 1: Find all local minima - O(n) time complexity
vector<int> findAllLocalMinima(const vector<int>& arr) {
    vector<int> result;
    int n = arr.size();
    
    if (n == 0) return result;
    
    // Check first element
    if (n == 1 || arr[0] <= arr[1]) {
        result.push_back(0);
    }
    
    // Check middle elements
    for (int i = 1; i < n - 1; i++) {
        if (arr[i] <= arr[i-1] && arr[i] <= arr[i+1]) {
            result.push_back(i);
        }
    }
    
    // Check last element
    if (n > 1 && arr[n-1] <= arr[n-2]) {
        result.push_back(n-1);
    }
    
    return result;
}

// Part 2: Find any one local minima efficiently - O(log n) time complexity
int findOneLocalMinimum(const vector<int>& arr) {
    int n = arr.size();
    
    if (n == 0) return -1;  // No local minimum in empty array
    if (n == 1) return 0;   // Single element is always a local minimum
    
    int left = 0, right = n - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        // Check if mid is a local minimum
        bool isLocalMin = true;
        
        // Check left neighbor
        if (mid > 0 && arr[mid] > arr[mid-1]) {
            isLocalMin = false;
        }
        
        // Check right neighbor
        if (mid < n-1 && arr[mid] > arr[mid+1]) {
            isLocalMin = false;
        }
        
        if (isLocalMin) {
            return mid;  // Found a local minimum
        }
        
        // If not a local minimum, decide which direction to search
        if (mid > 0 && arr[mid] > arr[mid-1]) {
            // Current element is greater than left neighbor,
            // so there must be a local minimum in the left half
            right = mid - 1;
        } else {
            // Current element is greater than right neighbor,
            // so there must be a local minimum in the right half
            left = mid + 1;
        }
    }
    
    return -1;  // Should not reach here for valid input
}

// Helper function to print vector
void printVector(const vector<int>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i];
        if (i < vec.size() - 1) cout << ",";
    }
    cout << endl;
}

int main() {
    // Test cases
    vector<int> test1 = {4, 8, 2, 10};
    vector<int> test2 = {7, 3, 5, 7, 9, 0, 2};
    
    cout << "Part 1: Find all local minima" << endl;
    cout << "Test 1: ";
    printVector(test1);
    cout << "All local minima indices: ";
    vector<int> result1 = findAllLocalMinima(test1);
    printVector(result1);
    cout << endl;
    
    cout << "Test 2: ";
    printVector(test2);
    cout << "All local minima indices: ";
    vector<int> result2 = findAllLocalMinima(test2);
    printVector(result2);
    cout << endl;
    
    cout << "Part 2: Find any one local minimum efficiently" << endl;
    cout << "Test 1 - One local minimum index: " << findOneLocalMinimum(test1) << endl;
    cout << "Test 2 - One local minimum index: " << findOneLocalMinimum(test2) << endl;
    
    // Additional test cases
    vector<int> test3 = {1};  // Single element
    vector<int> test4 = {5, 4, 3, 2, 1};  // Decreasing sequence
    vector<int> test5 = {1, 2, 3, 4, 5};  // Increasing sequence
    
    cout << "\nAdditional tests:" << endl;
    cout << "Single element [1]: " << findOneLocalMinimum(test3) << endl;
    cout << "Decreasing [5,4,3,2,1]: " << findOneLocalMinimum(test4) << endl;
    cout << "Increasing [1,2,3,4,5]: " << findOneLocalMinimum(test5) << endl;
    
    return 0;
}

/*
PROBLEM STATEMENT:

Part 1, Basic Coding:
Given an input array of integers, return the indices of all local minima in the array.
● An element is a local minima if it is <= its immediate neighbors.
● Ex. 1
  ○ In: 4,8,2,10
  ○ Out: 0,2
● Ex. 2
  ○ In: 7,3,5,7,9,0,2
  ○ Out: 1,5

Part 2, Efficiency:
Given an input array of integers, return the index of any one local minima in the array.

SOLUTION APPROACH:

Part 1: O(n) Linear Search
- Iterate through array once
- Check each element against its neighbors
- Handle boundary cases (first/last elements)

Part 2: O(log n) Binary Search
- Use divide and conquer approach
- If current element isn't local minimum, determine search direction:
  - If arr[mid] > arr[mid-1], search left half
  - If arr[mid] > arr[mid+1], search right half
- Guaranteed to find a local minimum due to array boundaries
*/