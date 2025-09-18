#include <vector>
#include <queue>
#include <iostream>
using namespace std;

/**
 * Definition for N-ary tree node
 */
struct TreeNode {
    int val;
    vector<TreeNode*> children;
    TreeNode(int x) : val(x) {}
};

/**
 * Solution 1: BFS Approach for N-ary Tree Arc Walk Problem
 * 
 * ALGORITHM:
 * 1. Perform single BFS traversal to get both left and right views
 * 2. Left view: First node at each level (leftmost visible)
 * 3. Right view: Last node at each level (rightmost visible)
 * 4. Combine views: reverse(left_view) + right_view[1:] (skip root duplication)
 * 
 * WHY THIS WORKS:
 * - Walking from bottom-left to bottom-right in an arc via root means:
 *   * Start from leftmost bottom node, go up the left side
 *   * Reach root, then go down the right side
 *   * This path exactly matches: bottom-to-top left view + top-to-bottom right view
 * 
 * TIME COMPLEXITY: O(N) where N = number of nodes
 * - Single BFS traversal visits each node exactly once
 * - Additional O(H) for combining views where H = height of tree
 * 
 * SPACE COMPLEXITY: O(W + H) where W = maximum width, H = height
 * - BFS queue can hold at most W nodes (maximum nodes at any level)
 * - Left and right view arrays store at most H nodes (one per level)
 * - Result array stores at most 2*H nodes
 */
vector<int> arcWalkBFS(TreeNode* root) {
    if (!root) return {};
    
    vector<int> leftView, rightView;
    queue<TreeNode*> q;
    q.push(root);
    
    // Single BFS to capture both views simultaneously
    while (!q.empty()) {
        int size = q.size();
        
        // Process all nodes at current level
        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front();
            q.pop();
            
            // First node of level = left view (leftmost visible)
            if (i == 0) leftView.push_back(node->val);
            
            // Last node of level = right view (rightmost visible)
            if (i == size - 1) rightView.push_back(node->val);
            
            // Add all children for next level
            for (TreeNode* child : node->children) {
                q.push(child);
            }
        }
    }
    
    vector<int> result;
    
    // Add left view in reverse order (bottom to top of left side)
    for (int i = leftView.size() - 1; i >= 0; i--) {
        result.push_back(leftView[i]);
    }
    
    // Add right view excluding root (top to bottom of right side)
    // Skip index 0 to avoid duplicating root node
    for (int i = 1; i < rightView.size(); i++) {
        result.push_back(rightView[i]);
    }
    
    return result;
}

/**
 * Helper function for DFS approach
 * Recursively traverses tree and builds left/right views using depth tracking
 */
void dfsHelper(TreeNode* node, int depth, vector<int>& leftView, vector<int>& rightView) {
    if (!node) return;
    
    // Expand arrays if we've reached a new depth
    if (depth >= leftView.size()) {
        leftView.resize(depth + 1, -1);  // -1 indicates uninitialized
        rightView.resize(depth + 1, -1);
    }
    
    // First node at this depth = left view
    if (leftView[depth] == -1) {
        leftView[depth] = node->val;
    }
    
    // Always update right view (last node processed at this depth wins)
    rightView[depth] = node->val;
    
    // Recursively traverse children left-to-right
    // This ensures rightmost child overwrites rightView[depth]
    for (TreeNode* child : node->children) {
        dfsHelper(child, depth + 1, leftView, rightView);
    }
}

/**
 * Solution 2: DFS Approach with Depth Tracking for N-ary Tree Arc Walk Problem
 * 
 * ALGORITHM:
 * 1. Use DFS with depth parameter to traverse tree
 * 2. Track depth and maintain leftView[depth] and rightView[depth] arrays
 * 3. For each node at depth d:
 *    - If first time seeing depth d, record in leftView[d]
 *    - Always update rightView[d] (last node processed at this depth)
 * 4. Traverse children left-to-right so rightmost node is recorded last
 * 5. Combine views: reverse(leftView) + rightView[1:] (skip root duplication)
 * 
 * WHY THIS WORKS:
 * - DFS naturally explores depth-first, allowing us to track depth explicitly
 * - Processing children left-to-right ensures:
 *   * First node encountered at each depth = leftmost visible
 *   * Last node processed at each depth = rightmost visible
 * - Same logic as BFS: arc walk = left boundary (reversed) + right boundary
 * 
 * TIME COMPLEXITY: O(N) where N = number of nodes
 * - Single DFS traversal visits each node exactly once
 * - Additional O(H) for combining views where H = height of tree
 * 
 * SPACE COMPLEXITY: O(H) where H = height of tree
 * - DFS recursion stack: O(H) in worst case (skewed tree)
 * - Left and right view arrays: O(H) 
 * - Better space complexity than BFS for wide trees (no queue needed)
 */
vector<int> arcWalkDFS(TreeNode* root) {
    if (!root) return {};
    
    vector<int> leftView, rightView;
    
    // Single DFS traversal with depth tracking
    dfsHelper(root, 0, leftView, rightView);
    
    vector<int> result;
    
    // Add left view in reverse order (bottom to top of left side)
    for (int i = leftView.size() - 1; i >= 0; i--) {
        if (leftView[i] != -1) {  // Skip uninitialized levels
            result.push_back(leftView[i]);
        }
    }
    
    // Add right view excluding root (top to bottom of right side)
    // Skip index 0 to avoid duplicating root node
    for (int i = 1; i < rightView.size(); i++) {
        if (rightView[i] != -1) {  // Skip uninitialized levels
            result.push_back(rightView[i]);
        }
    }
    
    return result;
}

// Test function
int main() {
    // Example: Create tree from image
    TreeNode* root = new TreeNode(4);
    TreeNode* n5 = new TreeNode(5);
    TreeNode* n7 = new TreeNode(7);
    TreeNode* n12 = new TreeNode(12);
    TreeNode* n1 = new TreeNode(1);
    TreeNode* n8 = new TreeNode(8);
    
    root->children = {n5, n7};
    n5->children = {n12, n1};
    n7->children = {n8};
    
    // Test both approaches
    vector<int> resultBFS = arcWalkBFS(root);
    vector<int> resultDFS = arcWalkDFS(root);
    
    cout << "BFS Arc walk result: ";
    for (int val : resultBFS) {
        cout << val << " ";
    }
    cout << endl;
    
    cout << "DFS Arc walk result: ";
    for (int val : resultDFS) {
        cout << val << " ";
    }
    cout << endl;
    
    // Verify both approaches give same result
    cout << "Both approaches match: " << (resultBFS == resultDFS ? "YES" : "NO") << endl;
    
    return 0;
}

/*
==============================================================================
                           PROBLEM STATEMENT
==============================================================================

Given an N-ary tree, print the nodes seen by a person when walking from 
bottom left to bottom right in an arc via root node.

VISUALIZATION:
- Imagine a person standing at the bottom left of the tree
- They walk in an arc shape around the tree's perimeter
- The path goes: bottom-left → up left side → root → down right side → bottom-right
- Print all nodes visible during this walk

EXAMPLES:

Tree 1:        4
              / \
             5   7
            / \   \
           12  1   8

Walking path: 12(bottom-left) → 5(up) → 4(root) → 7(down) → 8(bottom-right)
Output: 12, 5, 4, 7, 8

Tree 2:        4
              /|\
             5 7 8
            /
           12

Walking path: 12(bottom-left) → 5(up) → 4(root) → 8(down)
Output: 12, 5, 4, 8
Note: When reaching bottom-right, node 12 becomes visible again

KEY INSIGHTS:
1. The arc walk creates a "boundary" view of the tree
2. Left boundary = bottom-to-top traversal of leftmost nodes
3. Right boundary = top-to-bottom traversal of rightmost nodes  
4. Root appears only once (avoid duplication when combining views)
5. Some nodes may appear twice if visible from both sides

CONSTRAINTS:
- N-ary tree (each node can have 0 to N children)
- Tree is not empty
- Node values are unique integers
- No specific ordering required for children

SOLUTION APPROACHES COMPARISON:

1. BFS Approach (Level-order traversal):
   - Time: O(N), Space: O(W + H) where W = max width
   - Uses queue for level-by-level processing
   - Natural for "view" problems
   - Higher space usage for wide trees

2. DFS Approach (Depth-first with tracking):
   - Time: O(N), Space: O(H) where H = height  
   - Uses recursion stack and depth tracking
   - Better space complexity for wide trees
   - More intuitive for boundary traversal problems

Both approaches produce identical results but have different space trade-offs
depending on tree shape (wide vs tall).

ALGORITHM WALKTHROUGH (for both approaches):
1. Capture left view: leftmost visible node at each level
2. Capture right view: rightmost visible node at each level  
3. Combine: reverse(leftView) + rightView[1:]
   - Reverse left view to get bottom-to-top order
   - Skip root in right view to avoid duplication
4. Result represents the arc walk path

Example trace for Tree 1:
- Left view: [4, 5, 12] (top to bottom)
- Right view: [4, 7, 8] (top to bottom)
- Reversed left: [12, 5, 4] (bottom to top)
- Right without root: [7, 8] (skip 4)
- Final result: [12, 5, 4, 7, 8] ✓

==============================================================================
*/