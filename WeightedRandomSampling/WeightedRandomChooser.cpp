#include <bits/stdc++.h>

/**
 * @brief Weighted Random Selection using Number Line + Upper Bound approach
 * 
 * This function selects a label randomly from a list of (label, weight) pairs
 * where the probability of selecting each label is proportional to its weight.
 * 
 * Algorithm:
 * 1. Build cumulative sums array (number line segments)
 * 2. Generate random number in range [0, total_weight)
 * 3. Use upper_bound to find the first cumulative sum >= random number
 * 
 * @param pairs Vector of (label, weight) pairs
 * @return Selected label based on weighted probability
 * 
 * Time Complexity: O(N) for preprocessing + O(log N) for selection
 * Space Complexity: O(N) for storing cumulative sums
 */
std::string weightedRandomSelection(const std::vector<std::pair<std::string, int>>& pairs) {
    if (pairs.empty()) {
        throw std::invalid_argument("Input pairs cannot be empty");
    }
    
    // Step 1: Build cumulative sums array (number line boundaries)
    std::vector<int> cumulativeSums;
    int totalWeight = 0;
    
    for (const auto& pair : pairs) {
        if (pair.second <= 0) {
            throw std::invalid_argument("Weights must be positive");
        }
        totalWeight += pair.second;
        cumulativeSums.push_back(totalWeight);
    }
    
    // Step 2: Generate random number in range [0, totalWeight)
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, totalWeight - 1);
    int randomNum = dis(gen);
    
    // Step 3: Use upper_bound to find where random number lands on number line
    // upper_bound finds first element > randomNum, which gives us the correct segment
    auto it = std::upper_bound(cumulativeSums.begin(), cumulativeSums.end(), randomNum);
    if (it == cumulativeSums.end()) it--;  // Add this line!
    int index = it - cumulativeSums.begin();
    
    return pairs[index].first;
}

/**
 * @brief Test function to verify the weighted random selection
 * 
 * Runs multiple iterations and counts frequency of each label
 * to verify the distribution matches expected probabilities
 */
void testWeightedSelection() {
    std::vector<std::pair<std::string, int>> testPairs = {
        {"a", 1},
        {"b", 2}, 
        {"c", 3}
    };
    
    const int iterations = 60000;  // Large sample for better accuracy
    std::map<std::string, int> counts;
    
    // Initialize counts
    for (const auto& pair : testPairs) {
        counts[pair.first] = 0;
    }
    
    // Run weighted selection multiple times
    for (int i = 0; i < iterations; i++) {
        std::string result = weightedRandomSelection(testPairs);
        counts[result]++;
    }
    
    // Calculate and display results
    int totalWeight = 1 + 2 + 3; // = 6
    std::cout << "=== Weighted Random Selection Test ===" << std::endl;
    std::cout << "Total iterations: " << iterations << std::endl;
    std::cout << "Expected probabilities: a=16.67%, b=33.33%, c=50.00%" << std::endl;
    std::cout << std::endl;
    
    for (const auto& pair : testPairs) {
        std::string label = pair.first;
        int weight = pair.second;
        int actualCount = counts[label];
        double expectedProb = (double)weight / totalWeight * 100;
        double actualProb = (double)actualCount / iterations * 100;
        
        std::cout << "Label '" << label << "': " 
                  << actualCount << "/" << iterations 
                  << " (" << std::fixed << std::setprecision(2) << actualProb << "%) "
                  << "Expected: " << expectedProb << "%" << std::endl;
    }
}

int main() {
    try {
        // Test the weighted selection function
        testWeightedSelection();
        
        std::cout << std::endl << "=== Individual Test Runs ===" << std::endl;
        
        // Show a few individual selections
        std::vector<std::pair<std::string, int>> example = {
            {"apple", 10},
            {"banana", 30},
            {"cherry", 60}
        };
        
        std::cout << "Example: [('apple', 10), ('banana', 30), ('cherry', 60)]" << std::endl;
        std::cout << "Expected probabilities: apple=10%, banana=30%, cherry=60%" << std::endl;
        std::cout << "Sample selections: ";
        
        for (int i = 0; i < 10; i++) {
            std::cout << weightedRandomSelection(example) << " ";
        }
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

/*
==================================================================================
PROBLEM STATEMENT:
==================================================================================

Given an input of a list of pairs, where the first element of each pair is a "label" 
and the second element an associated "weight", write a function which chooses and 
returns a single label in a correctly random fashion where the probability of 
returning any of the particular labels is distributed according to their weights.

For example, given input: [("a", 1), ("b", 2), ("c", 3)]

A single invocation of the function would return a label a, b or c with the 
following probabilities:
- a: 1/6 (16.67%)
- b: 2/6 (33.33%) 
- c: 3/6 (50.00%)

IMPORTANT NOTES:
- No saving information between runs of the function (stateless)
- "Correctly random" means each run is an independent event
- Function should return a single label, not a probability table
- Should handle large weights efficiently (avoid naive O(K) solutions)

==================================================================================
ALGORITHM EXPLANATION: Number Line + Upper Bound Approach
==================================================================================

1. NUMBER LINE VISUALIZATION:
   Weights: [1, 2, 3] 
   Cumulative: [1, 3, 6]
   Number line: 0----1----3----6
                 [a] [b ] [ c  ]

2. SELECTION PROCESS:
   - Generate random number r in range [0, total_weight)
   - Use upper_bound to find first cumulative sum > r
   - Return corresponding label

3. EXAMPLE:
   Random = 4.2 → upper_bound finds 6 → index 2 → return "c"

==================================================================================
COMPLEXITY ANALYSIS:
==================================================================================

TIME COMPLEXITY:
- Preprocessing: O(N) - building cumulative sums array
- Selection: O(log N) - binary search via upper_bound
- Overall: O(N + log N) = O(N) per function call

SPACE COMPLEXITY:
- O(N) - storing cumulative sums array
- Could be optimized to O(1) by recalculating on each call, but increases time to O(N)

WHERE:
- N = number of input pairs
- K = sum of all weights

COMPARISON WITH NAIVE APPROACH:
- Naive: O(K) time and space (create array of size K)
- Optimized: O(N) time and space (much better when K >> N)

==================================================================================
OPTIMIZATIONS FOR SCALE:
==================================================================================

1. STATEFUL VERSION (if allowed to save between calls):
   - Precompute cumulative sums once: O(N)
   - Each subsequent call: O(log N)

2. EXTREMELY LARGE DATASETS:
   - Use disk-based B-trees for cumulative sums
   - Still achieves O(log N) search time

3. FLOATING POINT WEIGHTS:
   - Replace int with double
   - Use uniform_real_distribution instead of uniform_int_distribution

==================================================================================
*/