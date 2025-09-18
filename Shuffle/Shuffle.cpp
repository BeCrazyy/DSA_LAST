#include <bits/stdc++.h>

using namespace std;

/**
 * Fisher-Yates Shuffle Algorithm (Knuth Shuffle)
 * 
 * ALGORITHM EXPLANATION:
 * The Fisher-Yates shuffle generates a random permutation of an array where
 * all n! permutations are equally likely (uniform distribution).
 * 
 * HOW IT WORKS:
 * 1. Start from the last element (index n-1) and work backwards
 * 2. For each position i, randomly select an element from positions 0 to i (inclusive)
 * 3. Swap the current element with the randomly selected element
 * 4. Move to the previous position and repeat
 * 
 * MATHEMATICAL PROOF OF CORRECTNESS:
 * - For position n-1: choose randomly from all n elements (probability 1/n for each)
 * - For position n-2: choose randomly from remaining n-1 elements (probability 1/(n-1) for each)
 * - For position n-3: choose randomly from remaining n-2 elements (probability 1/(n-2) for each)
 * - And so on...
 * 
 * Final probability for any specific permutation = 1/n × 1/(n-1) × 1/(n-2) × ... × 1/1 = 1/n!
 * Since there are n! possible permutations, each has probability 1/n!, ensuring uniform distribution.
 * 
 * EXAMPLE WALKTHROUGH WITH [A, B, C, D]:
 * Step 1: i=3, pick random from {0,1,2,3}, say pick 1(B), swap → [A,D,C,B]
 * Step 2: i=2, pick random from {0,1,2}, say pick 0(A), swap → [C,D,A,B]
 * Step 3: i=1, pick random from {0,1}, say pick 1(D), swap → [C,D,A,B]
 * Step 4: i=0, only one element left, done → [C,D,A,B]
 * 
 * TIME COMPLEXITY: O(n) - single pass through the array
 * SPACE COMPLEXITY: O(1) - only swaps elements in place, no extra space needed
 * 
 * NOTE ON RANDOM NUMBER GENERATOR:
 * This implementation uses the simple rand() function which is perfectly adequate
 * for learning purposes and most practical applications. The Fisher-Yates algorithm
 * itself is mathematically sound regardless of the random generator used.
 * For cryptographic applications, you would need a more secure random generator.
 */

void shuffle(vector<int>& arr) {
    int n = arr.size();
    
    // Start from last element and work backwards
    for (int i = n - 1; i > 0; i--) {
        // Pick a random index from 0 to i
        int j = rand() % (i + 1);
        
        // Swap arr[i] with arr[j]
        swap(arr[i], arr[j]);
    }
}

/**
 * Fisher-Yates Partial Shuffle for Random Subset Generation
 * 
 * ALGORITHM EXPLANATION:
 * Instead of shuffling the entire array, we only shuffle the first m positions.
 * This gives us a random m-element subset in the first m positions.
 * 
 * HOW IT WORKS:
 * 1. Create array [0, 1, 2, ..., N-1]
 * 2. Apply Fisher-Yates shuffle but stop after m iterations
 * 3. First m elements form our random subset
 * 
 * WHY IT WORKS:
 * - Each of the first m positions gets a uniformly random element from remaining pool
 * - Position 0: choose from all N elements (probability 1/N for each)
 * - Position 1: choose from remaining N-1 elements (probability 1/(N-1) for each)
 * - Position m-1: choose from remaining N-m+1 elements
 * 
 * This ensures each possible m-element subset has equal probability of C(N,m)^(-1)
 * 
 * EXAMPLE WITH N=5, m=3:
 * Array: [0, 1, 2, 3, 4]
 * Step 1: i=0, pick from {0,1,2,3,4}, say pick 3, swap → [3, 1, 2, 0, 4]
 * Step 2: i=1, pick from {1,2,0,4}, say pick 0, swap → [3, 0, 2, 1, 4]  
 * Step 3: i=2, pick from {2,1,4}, say pick 4, swap → [3, 0, 4, 1, 2]
 * Result: subset {3, 0, 4}
 * 
 * TIME COMPLEXITY: O(N + m) - O(N) to create array, O(m) for partial shuffle
 * SPACE COMPLEXITY: O(N) - for storing the array
 */

vector<int> randomSubset(int N, int m) {
    // Create array [0, 1, 2, ..., N-1]
    vector<int> arr(N);
    for (int i = 0; i < N; i++) {
        arr[i] = i;
    }
    
    // Partial Fisher-Yates shuffle - only first m positions
    for (int i = 0; i < m; i++) {
        // Pick random element from remaining unshuffled portion
        int j = i + rand() % (N - i);
        
        // Swap current position with randomly selected element
        swap(arr[i], arr[j]);
    }
    
    // Return first m elements as our random subset
    return vector<int>(arr.begin(), arr.begin() + m);
}

void printArray(vector<int>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

void printVector(const vector<int>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

int main() {
    // Seed the random number generator
    srand(time(0));
    
    // Example 1: Shuffle entire array
    vector<int> arr = {1, 2, 3, 4, 5};
    
    cout << "=== FISHER-YATES SHUFFLE ===" << endl;
    cout << "Original array: ";
    printArray(arr);
    
    shuffle(arr);
    
    cout << "Shuffled array: ";
    printArray(arr);
    
    // Example 2: Random subset generation
    cout << "\n=== RANDOM SUBSET GENERATION ===" << endl;
    int N = 10;  // Numbers 0 to 9
    int m = 4;   // Select 4 numbers
    
    cout << "Generating " << m << " random numbers from 0 to " << N-1 << ": ";
    vector<int> subset = randomSubset(N, m);
    printVector(subset);
    
    return 0;
}

/*
PROBLEM DESCRIPTIONS:

1. ARRAY SHUFFLING:
Given an array x of length n, write code to shuffle the array such that all 
possible permutations are equally likely. This means each of the n! possible 
arrangements of the array elements should have exactly the same probability 
of occurring (1/n!).

Requirements:
- Uniform distribution: All permutations must be equally probable
- Efficiency: Should run in O(n) time
- In-place: Should not require extra space proportional to input size
- Unbiased: No systematic preference for certain arrangements

2. RANDOM SUBSET GENERATION:
Generate a random m-element subset of the numbers {0, 1, 2, ..., N-1}.
Each possible subset should have equal probability of being selected.

Requirements:
- Uniform distribution: All C(N,m) possible subsets equally likely
- Efficiency: Should be reasonably fast for typical values of N and m
- Correctness: Mathematical guarantee of uniformity

SOLUTIONS:
Both problems use variations of the Fisher-Yates shuffle algorithm:
- Full shuffle for array permutation
- Partial shuffle for subset generation

These algorithms are provably correct and widely used in practice for 
applications requiring truly random shuffling and sampling.

COMMON MISTAKES TO AVOID:
1. Don't pick from the entire array range for each position (creates bias)
2. Don't use approaches like "sort by random keys" (not truly uniform)
3. Don't use naive approaches that might favor certain permutations/subsets

The algorithms presented here are mathematically proven to be correct and
are widely used in practice for applications requiring truly random operations.
*/