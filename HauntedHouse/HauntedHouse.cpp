#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

/*
================================================================================
PROBLEM EXPLANATION:
N people want to visit haunted house. Each person has constraints (L, H):
- L: minimum number of OTHER people they need with them
- H: maximum number of OTHER people they're comfortable with

Goal: Find maximum number of people that can go together such that everyone 
      in the group is satisfied with the group size.

CORE INSIGHT: 
To form a group of size K, you need AT LEAST K people who are each individually 
willing to be in a group of that size. It's like organizing a dinner party - 
if you want 5 people at the table, you need at least 5 people who are each 
okay with a 5-person dinner!

DETAILED EXAMPLE WALKTHROUGH:
================================================================================

Sample Input: 6 people with constraints:
Person 1: (1,2) → needs 1-2 others → willing for group sizes 2-3
Person 2: (1,4) → needs 1-4 others → willing for group sizes 2-5
Person 3: (0,3) → okay with 0-3 others → willing for group sizes 1-4
Person 4: (0,1) → okay with 0-1 others → willing for group sizes 1-2
Person 5: (3,4) → needs 3-4 others → willing for group sizes 4-5
Person 6: (0,2) → okay with 0-2 others → willing for group sizes 1-3

O(N²) APPROACH - Manual Counting:
================================================================================
We build a count array showing how many people are willing for each group size.

Step 1: For each person, increment count for all their valid group sizes
Person 1: willing for [2,3] → count[2]++, count[3]++
Person 2: willing for [2,5] → count[2]++, count[3]++, count[4]++, count[5]++
Person 3: willing for [1,4] → count[1]++, count[2]++, count[3]++, count[4]++
Person 4: willing for [1,2] → count[1]++, count[2]++
Person 5: willing for [4,5] → count[4]++, count[5]++
Person 6: willing for [1,3] → count[1]++, count[2]++, count[3]++

Final count array: [-, 3, 5, 4, 3, 2, 0]
                    k  1  2  3  4  5  6

Step 2: Check feasibility for each group size K
Group size 1: count[1] = 3, need ≥ 1 → 3 ≥ 1 ✓
Group size 2: count[2] = 5, need ≥ 2 → 5 ≥ 2 ✓
Group size 3: count[3] = 4, need ≥ 3 → 4 ≥ 3 ✓
Group size 4: count[4] = 3, need ≥ 4 → 3 ≥ 4 ✗ (Not enough people!)
Group size 5: count[5] = 2, need ≥ 5 → 2 ≥ 5 ✗ (Not enough people!)

Answer: Maximum feasible group size = 3

O(N) APPROACH - Difference Array Magic:
================================================================================
Instead of incrementing each position individually, we use range markers.

Step 1: Mark range boundaries in difference array
Person 1: range [2,3] → diff[2]++, diff[4]--
Person 2: range [2,5] → diff[2]++, diff[6]--
Person 3: range [1,4] → diff[1]++, diff[5]--
Person 4: range [1,2] → diff[1]++, diff[3]--
Person 5: range [4,5] → diff[4]++, diff[6]--
Person 6: range [1,3] → diff[1]++, diff[4]--

Final diff array: [0, +3, +2, -1, 0, -1, -2]

Step 2: Convert to actual counts using prefix sum
count[1] = 0 + 3 = 3
count[2] = 3 + 2 = 5
count[3] = 5 + (-1) = 4
count[4] = 4 + 0 = 4  (Wait, this should be 3! Let me recalculate...)

Actually, let me redo this correctly:
After Person 1: diff = [0, 0, +1, 0, -1, 0, 0]
After Person 2: diff = [0, 0, +2, 0, -1, 0, -1]
After Person 3: diff = [0, +1, +2, 0, -1, -1, -1]
After Person 4: diff = [0, +2, +2, -1, -1, -1, -1]
After Person 5: diff = [0, +2, +2, -1, 0, -1, -2]
After Person 6: diff = [0, +3, +2, -1, -1, -1, -2]

Converting to counts:
count[1] = 0 + 3 = 3
count[2] = 3 + 2 = 5
count[3] = 5 + (-1) = 4
count[4] = 4 + (-1) = 3
count[5] = 3 + (-1) = 2

Same result: [-, 3, 5, 4, 3, 2] → Answer = 3

KEY INSIGHT: 
Both approaches give the same answer, but O(N) is much faster because:
- O(N²): Each person might do many increments (worst case: N increments per person)
- O(N): Each person does exactly 2 operations (mark start and end)

THE ALGORITHM: For each group size K, count willing people. If count >= K, it's feasible!
================================================================================
*/

// O(N^2) Solution - Straightforward Counting Approach
/*
INTUITION: 
Think of this like organizing a dinner party. For each group size K, we need to count 
how many people are willing to attend a party of that size. If fewer than K people 
are willing, we can't form the group - it's like having a 5-person table but only 
3 people showing up.

APPROACH:
1. For each person, look at their constraints (L,H)
2. They're willing for group sizes (L+1) to (H+1) 
3. Increment a counter for each of those group sizes
4. Finally, check each group size K: if count[K] >= K, it's feasible

This is like manually checking each table size and counting willing attendees.
*/
int solveNSquared(int n, vector<pair<int, int>>& constraints) {
    vector<int> count(n + 1, 0);
    
    // For each person, increment count for all valid group sizes
    for (int i = 0; i < n; i++) {
        int L = constraints[i].first;
        int H = constraints[i].second;
        
        // Person is willing for group sizes (L+1) to (H+1)
        for (int groupSize = L + 1; groupSize <= H + 1 && groupSize <= n; groupSize++) {
            count[groupSize]++;
        }
    }
    
    // Find maximum group size where count[k] >= k
    int answer = 0;
    for (int k = 1; k <= n; k++) {
        if (count[k] >= k) {
            answer = k;
        }
    }
    
    return answer;
}

// O(N) Solution using Difference Array Technique
/*
INTUITION:
Imagine you're painting a fence. The O(N²) approach paints each plank individually. 
The O(N) approach is smarter: instead of painting each plank, you just mark where 
to "start painting" and where to "stop painting", then do all the painting in one pass.

THE MAGIC OF DIFFERENCE ARRAYS:
- Person wants to be in groups [2,4]? 
  O(N²): increment count[2], count[3], count[4] (3 operations)
  O(N): mark diff[2]++ and diff[5]-- (2 operations)
  
- When we do prefix sum, diff[2]++ affects ALL positions from 2 onwards
- And diff[5]-- cancels that effect from position 5 onwards
- So effectively we've incremented positions 2,3,4 with just 2 operations!

ANALOGY: 
It's like saying "turn on the lights at position 2, turn off at position 5"
instead of manually flipping each light switch between 2 and 4.

KEY INSIGHT: 
Range updates become O(1) instead of O(range_size), making the entire 
algorithm O(N) instead of O(N²).
*/
int solveLinear(int n, vector<pair<int, int>>& constraints) {
    vector<int> diff(n + 2, 0);  // Extra space to avoid boundary checks
    
    // Mark range boundaries using difference array
    for (int i = 0; i < n; i++) {
        int L = constraints[i].first;
        int H = constraints[i].second;
        
        int start = L + 1;  // Willing from group size (L+1)
        int end = H + 1;    // Willing until group size (H+1)
        
        if (start <= n) {
            diff[start]++;
        }
        if (end + 1 <= n + 1) {
            diff[end + 1]--;
        }
    }
    
    // Convert difference array to actual counts using prefix sum
    vector<int> count(n + 1, 0);
    for (int k = 1; k <= n; k++) {
        count[k] = count[k - 1] + diff[k];
    }
    
    // Find maximum group size where count[k] >= k
    int answer = 0;
    for (int k = 1; k <= n; k++) {
        if (count[k] >= k) {
            answer = k;
        }
    }
    
    return answer;
}

// Helper function to show detailed analysis (optional)
void showAnalysis(int n, vector<pair<int, int>>& constraints, int result) {
    cout << "\n=== DETAILED ANALYSIS ===\n";
    
    // Build count array for analysis
    vector<int> count(n + 1, 0);
    for (int i = 0; i < n; i++) {
        int L = constraints[i].first;
        int H = constraints[i].second;
        for (int groupSize = L + 1; groupSize <= H + 1 && groupSize <= n; groupSize++) {
            count[groupSize]++;
        }
    }
    
    cout << "Group Size | Willing People | Need | Feasible?\n";
    cout << "-----------|----------------|------|-----------\n";
    
    for (int k = 1; k <= n; k++) {
        bool feasible = (count[k] >= k);
        cout << "     " << k << "     |       " << count[k] << "        |  " << k 
             << "   | " << (feasible ? "✓" : "✗") << "\n";
    }
    
    cout << "\nMaximum feasible group size: " << result << "\n";
}

int main() {
    cout << "=== HAUNTED HOUSE GROUP OPTIMIZATION ===\n\n";
    
    // Hardcoded sample input from our detailed example
    int n = 6;
    vector<pair<int, int>> constraints = {
        {1, 2},  // Person 1: needs 1-2 others
        {1, 4},  // Person 2: needs 1-4 others  
        {0, 3},  // Person 3: okay with 0-3 others
        {0, 1},  // Person 4: okay with 0-1 others
        {3, 4},  // Person 5: needs 3-4 others
        {0, 2}   // Person 6: okay with 0-2 others
    };
    
    cout << "--- PROBLEM SETUP ---\n";
    cout << "Number of people: " << n << "\n";
    cout << "Constraints (L = min others, H = max others):\n";
    for (int i = 0; i < n; i++) {
        int L = constraints[i].first;
        int H = constraints[i].second;
        cout << "Person " << (i + 1) << ": (" << L << ", " << H 
             << ") → willing for group sizes " << (L+1) << "-" << (H+1) << "\n";
    }
    
    // Test both solutions
    cout << "\nRunning O(N²) solution..." << endl;
    int result1 = solveNSquared(n, constraints);
    
    cout << "Running O(N) solution..." << endl;
    int result2 = solveLinear(n, constraints);
    
    cout << "\n=== RESULTS ===\n";
    cout << "O(N²) Solution: " << result1 << "\n";
    cout << "O(N) Solution: " << result2 << "\n";
    
    // Verify both solutions give same answer
    if (result1 == result2) {
        cout << "✓ Both solutions match!\n";
    } else {
        cout << "✗ Solutions don't match - there's a bug!\n";
    }
    
    cout << "\n🎉 ANSWER: Maximum group size = " << result1 << "\n";
    
    // Optional detailed analysis
    char analysisChoice;
    cout << "\nShow detailed analysis? (y/n): ";
    cin >> analysisChoice;
    if (analysisChoice == 'y' || analysisChoice == 'Y') {
        showAnalysis(n, constraints, result1);
    }
    
    return 0;
}

/*
================================================================================
COMPLEXITY COMPARISON:

O(N²) APPROACH:
- For each person, increment count for all their valid group sizes
- Worst case: everyone wants groups [1,N] → N increments per person → O(N²)
- Simple but inefficient for large ranges

O(N) APPROACH - DIFFERENCE ARRAY:
- Core insight: Range updates vs Individual updates
- Person wants groups [2,4] → Instead of count[2]++, count[3]++, count[4]++
  just do diff[2]++ and diff[5]--
- Prefix sum propagates the effect automatically
- Always exactly 2 operations per person + O(N) prefix sum = O(N) total

ANALOGY: 
O(N²) = Paint each fence plank individually (slow)
O(N) = Mark "start painting" and "stop painting", then paint in one pass (fast)
================================================================================

SAMPLE INPUT:
6
1 2
1 4
0 3
0 1
3 4
0 2

EXPECTED OUTPUT: 3

EXPLANATION:
- Group size 1: People 3,4,6 willing (3 people) → 3≥1 ✓
- Group size 2: People 1,2,3,4,6 willing (5 people) → 5≥2 ✓  
- Group size 3: People 1,2,3,6 willing (4 people) → 4≥3 ✓
- Group size 4: People 2,3,5 willing (3 people) → 3≥4 ✗
- Group size 5: People 2,5 willing (2 people) → 2≥5 ✗

Maximum feasible = 3
================================================================================
*/