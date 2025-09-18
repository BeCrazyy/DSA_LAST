#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Function to get random element using binary search approach
int getRandom(std::vector<int>& candidates, std::vector<int>& frequencies) {
    // Build cumulative frequency array
    std::vector<int> cumulative;
    int totalSum = 0;
    
    for (int i = 0; i < frequencies.size(); i++) {
        totalSum += frequencies[i];
        cumulative.push_back(totalSum);
    }
    
    // Generate random number between 0 and totalSum-1
    int randomNum = rand() % totalSum;
    
    // Use upper_bound to find first cumulative sum > randomNum
    std::vector<int>::iterator it = std::upper_bound(cumulative.begin(), cumulative.end(), randomNum);
    int index = it - cumulative.begin();
    
    return candidates[index];
}

int main() {
    // Seed random number generator
    srand(time(0));
    
    // Test case
    std::vector<int> candidates = {0, 1, 2, 3, 6, 8};
    std::vector<int> frequencies = {1, 2, 1, 1, 3, 1};
    
    std::cout << "Candidates: ";
    for (int i = 0; i < candidates.size(); i++) {
        std::cout << candidates[i] << " ";
    }
    std::cout << "\nFrequencies: ";
    for (int i = 0; i < frequencies.size(); i++) {
        std::cout << frequencies[i] << " ";
    }
    std::cout << "\n\n";
    
    std::cout << "Random selections:\n";
    for (int i = 0; i < 20; i++) {
        std::cout << getRandom(candidates, frequencies) << " ";
    }
    std::cout << "\n";
    
    return 0;
}

/*
PROBLEM STATEMENT:
=================

Given an array of integers, return one element at random with weighted probability.

Input can be provided in two formats:

1. Flat Array: [0, 1, 1, 2, 3, 6, 6, 6, 8, ...]
   - Elements appear multiple times based on their frequency
   - Simple random index selection works

2. Histogram Format: 
   - candidates = [0, 1, 2, 3, 6, 8]
   - frequencies = [1, 2, 1, 1, 3, 1]
   - Need weighted random selection

SOLUTION APPROACH:
==================

Number Line Method with Binary Search:
1. Build cumulative frequency array: [1, 3, 4, 5, 8, 9]
2. Generate random number R between 0 and totalSum-1
3. Use upper_bound binary search to find first cumulative sum > R
4. Return candidate at that index

Example:
- If R = 0 → upper_bound finds index 0 → return candidates[0] = 0
- If R = 1 or 2 → upper_bound finds index 1 → return candidates[1] = 1
- If R = 5,6,7 → upper_bound finds index 4 → return candidates[4] = 6

Time Complexity: O(log M) where M is number of unique candidates
Space Complexity: O(M) for cumulative array
*/