#include <bits/stdc++.h>

using namespace std;

vector<int> findRobot(vector<vector<char>>& grid, vector<int>& query) {
    int rows = grid.size();
    int cols = grid[0].size();
    
    // Create distance matrices
    vector<vector<int>> leftDist(rows, vector<int>(cols, 0));
    vector<vector<int>> rightDist(rows, vector<int>(cols, 0));
    vector<vector<int>> topDist(rows, vector<int>(cols, 0));
    vector<vector<int>> bottomDist(rows, vector<int>(cols, 0));
    
    // Calculate left distances (scan left to right)
    for(int i = 0; i < rows; i++) {
        int dist = 0;
        for(int j = 0; j < cols; j++) {
            if(grid[i][j] == 'X') {
                dist = 0;
            } else {
                dist++;
            }
            leftDist[i][j] = dist;
        }
    }
    
    // Calculate right distances (scan right to left)
    for(int i = 0; i < rows; i++) {
        int dist = 0;
        for(int j = cols - 1; j >= 0; j--) {
            if(grid[i][j] == 'X') {
                dist = 0;
            } else {
                dist++;
            }
            rightDist[i][j] = dist;
        }
    }
    
    // Calculate top distances (scan top to bottom)
    for(int j = 0; j < cols; j++) {
        int dist = 0;
        for(int i = 0; i < rows; i++) {
            if(grid[i][j] == 'X') {
                dist = 0;
            } else {
                dist++;
            }
            topDist[i][j] = dist;
        }
    }
    
    // Calculate bottom distances (scan bottom to top)
    for(int j = 0; j < cols; j++) {
        int dist = 0;
        for(int i = rows - 1; i >= 0; i--) {
            if(grid[i][j] == 'X') {
                dist = 0;
            } else {
                dist++;
            }
            bottomDist[i][j] = dist;
        }
    }
    
    // Build map: distance signature -> robot position
    map<vector<int>, vector<int>> robotMap;
    
    // Find all robots and store their distance signatures
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(grid[i][j] == 'O') {
                vector<int> distances = {
                    leftDist[i][j], 
                    topDist[i][j], 
                    bottomDist[i][j], 
                    rightDist[i][j]
                };
                
                robotMap[distances] = {i, j};
            }
        }
    }
    
    // Direct vector lookup
    if(robotMap.find(query) != robotMap.end()) {
        return robotMap[query];
    }
    
    return {-1, -1}; // Robot not found
}

int main() {
    // Test with the given example
    vector<vector<char>> grid = {
        {'O','E','E','E','X'},
        {'E','O','X','X','X'},
        {'E','E','E','E','E'},
        {'X','E','O','E','E'},
        {'X','E','X','E','X'}
    };
    
    vector<int> query = {2, 2, 4, 1};
    
    vector<int> result = findRobot(grid, query);
    
    if(result[0] != -1) {
        cout << "Robot found at position: [" << result[0] << ", " << result[1] << "]" << endl;
    } else {
        cout << "Robot not found!" << endl;
    }
    
    return 0;
}

/*
PROBLEM STATEMENT:
================

Given two inputs:

1. Location map (2D array):
   | O | E | E | E | X |
   | E | O | X | X | X |
   | E | E | E | E | E |
   | X | E | O | E | E |
   | X | E | X | E | X |
   
   Where: O = Robot, E = Empty, X = Blocker

2. Query (1D array): Distance to closest blocker in order [left, top, bottom, right]
   Example: [2, 2, 4, 1] means:
   - Distance 2 to left closest blocker
   - Distance 2 to top closest blocker  
   - Distance 4 to bottom closest blocker
   - Distance 1 to right closest blocker

RULES:
- Grid boundaries are also considered blockers
- When robot hits boundary, it's considered hitting a blocker

TASK:
Write a function that returns the index/position of the robot that matches the given query.

SOLUTION APPROACH:
1. Precompute distance matrices for all 4 directions using DP
2. For each robot, calculate its distance signature [left, top, bottom, right]
3. Use HashMap to map distance signatures to robot positions
4. Lookup the query in the HashMap for O(1) retrieval

EXAMPLE:
For robot at position [1,1]:
- Left: 2 steps to boundary
- Top: 2 steps to boundary  
- Bottom: 4 steps to boundary
- Right: 1 step to blocker 'X'
- Signature: [2,2,4,1] matches query
- Answer: [1,1]

OPTIMIZATION STRATEGIES:
=======================

1. STATIC MAP + MANY QUERIES OPTIMIZATION:
------------------------------------------
Current solution: O(n×m) preprocessing + O(log R) per query
When map never changes but many queries expected:

a) Memory Layout Optimization:
   - Use perfect hashing since all distance signatures known upfront
   - Flatten HashMap to single array: index = hash(distance_signature)
   - Store robot positions in contiguous memory for better cache locality

b) Lookup Structure Optimization:
   - For few robots (R < 100): Use binary search on sorted array (better cache)
   - Build 4-level trie where each level = one direction's distance
   - Add Bloom filter as first-stage filter for impossible queries

c) Query Batching:
   - Process multiple queries together for better cache utilization
   - Sort incoming queries to improve memory access patterns

2. HUGE SPARSE MAP + FEW ROBOTS (SPACE OPTIMIZATION):
-----------------------------------------------------
Current solution: O(n×m) space complexity
When grid is huge and sparse with few robots:

a) On-Demand Calculation (O(r) space):
   - Don't precompute distance matrices
   - For each robot: scan in 4 directions until hitting blockers
   - Space: O(r) instead of O(n×m)
   - Time: O(r × max_distance) per query instead of O(1)

b) Sparse Representation:
   - Store only blocker positions in set/hash
   - Use run-length encoding for compression
   - Quadtree/spatial indexing for faster distance queries

c) Hybrid Approach:
   - Local distance matrices only around robots
   - Global sparse index of blockers for long-distance calculations
   - LRU cache for frequently queried robots

3. MAP UPDATE STRATEGIES (CACHE INVALIDATION):
----------------------------------------------
When map changes and cache needs updating:

a) Full Recompute (Simple):
   - Invalidate entire cache on any blocker change
   - Recompute all distance matrices: O(n×m) per update

b) Incremental Update (Optimal):
   - For blocker change at (x,y), only invalidate affected regions:
     * Left distances: row x, columns > y
     * Right distances: row x, columns < y  
     * Top distances: column y, rows > x
     * Bottom distances: column y, rows < x

c) Lazy Recomputation:
   - Mark affected regions as "dirty" but don't recompute immediately
   - On query: check if robot's distances are dirty, recompute only if needed
   - Avoid unnecessary computation for unqueried regions

4. APPROACH SELECTION GUIDELINES:
--------------------------------
Choose strategy based on specific constraints:

| Scenario | Best Strategy | Complexity |
|----------|---------------|------------|
| Dense grid, many robots | Full DP matrices | O(n×m) space |
| Sparse grid, few robots | Naive O(r) approach | O(r) space |
| Mixed usage patterns | Hybrid with caching | Variable |
| Frequent map updates | Lazy invalidation + O(r) | O(r) space |
| Static map, many queries | Perfect hashing + cache optimization | O(n×m) + optimized |

Key insight: The "optimal" approach heavily depends on grid density, robot count, 
query frequency, and update frequency. Consider your specific use case!
*/