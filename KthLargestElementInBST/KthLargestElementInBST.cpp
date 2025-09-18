#include <iostream>
#include <stack>
#include <vector>
using namespace std;

// Node structure for BST
struct Node {
    int key;
    Node* left;
    Node* right;
    
    Node(int data) {
        key = data;
        left = nullptr;
        right = nullptr;
    }
};

/*
=============================================================================
PROBLEM EXPLANATION:
=============================================================================

Given a Binary Search Tree and a positive integer k, find the k'th largest 
element in the tree.

Example BST:
       10
      /  \
     4    20
    /    /  \
   2    15   40

In-order traversal (ascending):  2, 4, 10, 15, 20, 40
Reverse in-order (descending): 40, 20, 15, 10, 4, 2

For k=3: 3rd largest = 15
For k=5: 5th largest = 4

KEY INSIGHT: Reverse in-order traversal of BST gives elements in descending order
*/

// =============================================================================
// SOLUTION 1: RECURSIVE APPROACH (Space: O(H), Time: O(H + k))
// =============================================================================

/*
ALGORITHM EXPLANATION:
1. Perform reverse in-order traversal: RIGHT → ROOT → LEFT
2. Keep a count of visited nodes
3. When count reaches k, we found our answer
4. Use pass-by-reference to maintain count across recursive calls

WHY THIS WORKS:
- BST property: left subtree < root < right subtree
- Reverse in-order visits nodes in descending order
- We can stop early when we reach the kth element

VISUAL WALKTHROUGH for k=3:
       10
      /  \
     4    20
    /    /  \
   2    15   40

Step 1: Start at 10, go RIGHT to 20
Step 2: At 20, go RIGHT to 40
Step 3: At 40, no right child → Process 40 (count=1)
Step 4: Return to 20 → Process 20 (count=2)
Step 5: At 20, go LEFT to 15 → Process 15 (count=3) → FOUND!
*/

void kthLargestUtil(Node* root, int k, int& count, int& result) {
    // Base case: null node or already found kth element
    if (root == nullptr || count >= k) {
        return;
    }
    
    // STEP 1: Traverse RIGHT subtree first (larger elements)
    kthLargestUtil(root->right, k, count, result);
    
    // STEP 2: Process current node (increment count)
    count++;
    if (count == k) {
        result = root->key;
        return; // Found kth largest, stop further traversal
    }
    
    // STEP 3: Traverse LEFT subtree (smaller elements)
    kthLargestUtil(root->left, k, count, result);
}

int kthLargestRecursive(Node* root, int k) {
    int count = 0;
    int result = -1;
    kthLargestUtil(root, k, count, result);
    return result;
}

// =============================================================================
// SOLUTION 2: ITERATIVE APPROACH (Space: O(H), Time: O(H + k))
// =============================================================================

/*
ALGORITHM EXPLANATION:
1. Use explicit stack to simulate recursion
2. First, push all right-path nodes to stack (go to rightmost)
3. Pop from stack, process node, then go to its left child
4. Continue until we process k nodes

WHY STACK-BASED APPROACH:
- Eliminates recursion (useful for deep trees)
- Same traversal order as recursive approach
- More control over the traversal process

STACK SIMULATION for k=3:
       10
      /  \
     4    20
    /    /  \
   2    15   40

Initial: curr=10, stack=[]
Step 1: Push 10,20,40 → stack=[10,20,40], curr=null
Step 2: Pop 40, process (count=1), curr=null
Step 3: Pop 20, process (count=2), curr=15
Step 4: Push 15 → stack=[10,15], curr=null  
Step 5: Pop 15, process (count=3) → FOUND!
*/

int kthLargestIterative(Node* root, int k) {
    if (root == nullptr) {
        return -1;
    }
    
    stack<Node*> stk;
    Node* curr = root;
    int count = 0;
    
    while (true) {
        // PHASE 1: Go to rightmost node, pushing all nodes to stack
        if (curr != nullptr) {
            stk.push(curr);
            curr = curr->right;
        }
        // PHASE 2: Process nodes from stack (reverse in-order)
        else if (!stk.empty()) {
            count++;
            curr = stk.top();
            stk.pop();
            
            // Check if this is the kth largest
            if (count == k) {
                return curr->key;
            }
            
            // Move to left subtree
            curr = curr->left;
        }
        else {
            break; // Stack empty and no current node
        }
    }
    
    return -1; // k is larger than number of nodes
}

// =============================================================================
// SOLUTION 3: MORRIS TRAVERSAL APPROACH (Space: O(1), Time: O(N))
// =============================================================================

/*
MORRIS TRAVERSAL EXPLANATION:

The challenge: How to traverse a tree without recursion or stack?
The solution: Temporarily modify the tree structure using "threading"

KEY CONCEPT: Threading
- Use NULL left pointers of leaf nodes to create "threads"
- Thread points to the in-order predecessor
- After processing, remove the thread to restore original structure

ALGORITHM STEPS:
1. If current node has no right child:
   - Process current node (increment count)
   - Move to left child

2. If current node has right child:
   - Find the leftmost node in right subtree (successor)
   - If successor's left is NULL: create thread, move right
   - If thread exists: remove thread, process current, move left

WHY O(1) SPACE:
- No recursion stack
- No explicit stack data structure  
- Only temporary modifications to existing tree structure

THREADING EXAMPLE for reverse in-order:
       10
      /  \
     4    20
    /    /  \
   2    15   40

Thread creation (dotted lines show threads):
   2 ←--- 4 ←--- 10 ←--- 15 ←--- 20 ←--- 40
   
These threads help us navigate back without using stack!
*/

int kthLargestMorris(Node* root, int k) {
    Node* curr = root;
    int count = 0;
    int kthLargest = -1;
    
    while (curr != nullptr) {
        // CASE 1: No right child - process current node
        if (curr->right == nullptr) {
            count++;
            if (count == k) {
                kthLargest = curr->key;
            }
            curr = curr->left;
        }
        // CASE 2: Has right child - need to handle threading
        else {
            // Find inorder successor (leftmost in right subtree)
            Node* succ = curr->right;
            while (succ->left != nullptr && succ->left != curr) {
                succ = succ->left;
            }
            
            // SUBCASE 2A: Thread doesn't exist - create it
            if (succ->left == nullptr) {
                succ->left = curr;  // Create thread
                curr = curr->right; // Move to right subtree
            }
            // SUBCASE 2B: Thread exists - remove it and process
            else {
                succ->left = nullptr; // Remove thread
                count++;
                if (count == k) {
                    kthLargest = curr->key;
                }
                curr = curr->left; // Move to left subtree
            }
        }
    }
    
    return kthLargest;
}

// =============================================================================
// UTILITY FUNCTIONS AND TESTING
// =============================================================================

Node* createSampleBST() {
    /*
    Creating the sample BST:
         10
        /  \
       4    20
      /    /  \
     2    15   40
    
    This will test various k values and edge cases
    */
    Node* root = new Node(10);
    root->left = new Node(4);
    root->right = new Node(20);
    root->left->left = new Node(2);
    root->right->left = new Node(15);
    root->right->right = new Node(40);
    
    return root;
}

void demonstrateTraversal(Node* root) {
    cout << "TRAVERSAL DEMONSTRATION:\n";
    cout << "========================\n";
    
    // Show normal in-order (ascending)
    cout << "Normal In-order (ascending):  ";
    cout << "2 → 4 → 10 → 15 → 20 → 40\n";
    
    // Show reverse in-order (descending)  
    cout << "Reverse In-order (descending): ";
    cout << "40 → 20 → 15 → 10 → 4 → 2\n\n";
    
    cout << "For k=3: We want 3rd in descending order = 15\n";
    cout << "For k=5: We want 5th in descending order = 4\n\n";
}

void testAllApproaches(Node* root) {
    cout << "TESTING ALL APPROACHES:\n";
    cout << "=======================\n";
    
    for (int k = 1; k <= 6; k++) {
        int recursive = kthLargestRecursive(root, k);
        int iterative = kthLargestIterative(root, k);
        int morris = kthLargestMorris(root, k);
        
        cout << k << " Largest Element: " << recursive;
        
        // Verify all approaches give same result
        if (recursive == iterative && iterative == morris) {
            cout << " ✓ (All approaches match)\n";
        } else {
            cout << " ✗ (Mismatch: R=" << recursive 
                 << " I=" << iterative << " M=" << morris << ")\n";
        }
    }
    cout << "\n";
}

void explainComplexity() {
    cout << "COMPLEXITY ANALYSIS:\n";
    cout << "====================\n\n";
    
    cout << "1. RECURSIVE APPROACH:\n";
    cout << "   Time Complexity:  O(H + k) average, O(N) worst case\n";
    cout << "   Space Complexity: O(H) due to recursion stack\n";
    cout << "   - H is height of tree\n";
    cout << "   - Best case: O(log N + k) for balanced tree\n";
    cout << "   - Worst case: O(N) for skewed tree\n";
    cout << "   - Early termination when k is small\n\n";
    
    cout << "2. ITERATIVE APPROACH:\n";
    cout << "   Time Complexity:  O(H + k) average, O(N) worst case\n";
    cout << "   Space Complexity: O(H) due to explicit stack\n";
    cout << "   - Same complexity as recursive\n";
    cout << "   - Avoids recursion stack overflow\n";
    cout << "   - More control over traversal\n\n";
    
    cout << "3. MORRIS TRAVERSAL:\n";
    cout << "   Time Complexity:  O(N) - each edge traversed ≤ 3 times\n";
    cout << "   Space Complexity: O(1) - no extra space!\n";
    cout << "   - True constant space complexity\n";
    cout << "   - Temporarily modifies tree structure\n";
    cout << "   - Ideal for memory-constrained environments\n\n";
}

void explainWhenToUse() {
    cout << "WHEN TO USE EACH APPROACH:\n";
    cout << "===========================\n\n";
    
    cout << "🔸 RECURSIVE APPROACH:\n";
    cout << "   • Clean, readable code\n";
    cout << "   • Small to medium trees\n";
    cout << "   • When stack overflow isn't a concern\n";
    cout << "   • Interviews (easy to explain)\n\n";
    
    cout << "🔸 ITERATIVE APPROACH:\n";
    cout << "   • Deep trees (avoid stack overflow)\n";
    cout << "   • When you need more control over traversal\n";
    cout << "   • Systems programming\n";
    cout << "   • Converting recursive to iterative\n\n";
    
    cout << "🔸 MORRIS TRAVERSAL:\n";
    cout << "   • Memory-constrained systems\n";
    cout << "   • Embedded systems\n";
    cout << "   • Large trees with limited memory\n";
    cout << "   • When O(1) space is requirement\n";
    cout << "   • Advanced interviews (L5+)\n\n";
}

// =============================================================================
// EXTENSION: MULTI-QUERY OPTIMIZATION
// =============================================================================

/*
ADVANCED: OPTIMIZING FOR MULTIPLE QUERIES

Problem: If we need to answer many kth largest queries on the same BST,
repeating the traversal each time is inefficient.

SOLUTION 1: Preprocessing with Array
- Do one in-order traversal to store all elements in sorted array
- Answer queries in O(1) time using array[n-k]
- Space: O(N), Query time: O(1)

SOLUTION 2: Augmented BST with Subtree Sizes
- Add size information to each node during preprocessing
- Use BST property + sizes to navigate directly to kth largest
- Space: O(1) extra per node, Query time: O(log N)

SOLUTION 3: Threaded BST
- Create permanent threads for reverse in-order traversal
- Answer queries by following k-1 threads from largest element
- Space: O(1) extra, Query time: O(k)
*/

class MultiQueryOptimizer {
private:
    vector<int> sortedElements;
    int totalNodes;
    
public:
    // Preprocessing: Store all elements in sorted order
    MultiQueryOptimizer(Node* root) {
        inOrderTraversal(root);
        totalNodes = sortedElements.size();
    }
    
    void inOrderTraversal(Node* root) {
        if (root == nullptr) return;
        
        inOrderTraversal(root->left);
        sortedElements.push_back(root->key);
        inOrderTraversal(root->right);
    }
    
    // Answer kth largest query in O(1) time
    int kthLargest(int k) {
        if (k <= 0 || k > totalNodes) {
            return -1;
        }
        return sortedElements[totalNodes - k];
    }
    
    void printSortedElements() {
        cout << "Sorted elements: ";
        for (int elem : sortedElements) {
            cout << elem << " ";
        }
        cout << "\n\n";
    }
};

void demonstrateMultiQuery(Node* root) {
    cout << "MULTI-QUERY OPTIMIZATION DEMO:\n";
    cout << "===============================\n";
    
    MultiQueryOptimizer optimizer(root);
    optimizer.printSortedElements();
    
    cout << "Testing multiple queries with O(1) response time:\n";
    for (int k = 1; k <= 6; k++) {
        cout << k << " largest: " << optimizer.kthLargest(k) << "\n";
    }
    cout << "\n";
    
    cout << "Multi-query approach benefits:\n";
    cout << "- Preprocessing: O(N) time, O(N) space\n";
    cout << "- Per query: O(1) time, O(1) space\n";
    cout << "- Best when: Many queries on same tree\n\n";
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    cout << "===========================================\n";
    cout << "FINDING KTH LARGEST ELEMENT IN BST\n";
    cout << "===========================================\n\n";
    
    // Create and display the sample BST
    Node* root = createSampleBST();
    
    cout << "Sample Binary Search Tree:\n";
    cout << "       10\n";
    cout << "      /  \\\n";
    cout << "     4    20\n";
    cout << "    /    /  \\\n";
    cout << "   2    15   40\n\n";
    
    // Demonstrate the traversal concept
    demonstrateTraversal(root);
    
    // Test all three main approaches
    testAllApproaches(root);
    
    // Explain complexity analysis
    explainComplexity();
    
    // Explain when to use each approach
    explainWhenToUse();
    
    // Demonstrate multi-query optimization
    demonstrateMultiQuery(root);
    
    // Test edge cases
    cout << "EDGE CASE TESTING:\n";
    cout << "==================\n";
    
    // Test with k larger than tree size
    cout << "k=10 (larger than tree): " << kthLargestRecursive(root, 10) << " (should be -1)\n";
    
    // Test with k=0
    cout << "k=0 (invalid): " << kthLargestRecursive(root, 0) << " (should be -1)\n";
    
    // Test with single node tree
    Node* singleNode = new Node(42);
    cout << "Single node tree, k=1: " << kthLargestRecursive(singleNode, 1) << " (should be 42)\n\n";
    
    cout << "INTERVIEW TIPS:\n";
    cout << "===============\n";
    cout << "✓ Start with recursive solution (easiest to code)\n";
    cout << "✓ Mention iterative alternative (avoids stack overflow)\n";
    cout << "✓ For L5+: Discuss Morris traversal for O(1) space\n";
    cout << "✓ Consider multi-query optimization if asked\n";
    cout << "✓ Always test edge cases (k=0, k>n, empty tree)\n";
    cout << "✓ Analyze time/space complexity for each approach\n\n";
    
    cout << "FINAL RECOMMENDATIONS:\n";
    cout << "======================\n";
    cout << "• Production code: Use recursive (clean & readable)\n";
    cout << "• Memory critical: Use Morris traversal\n";
    cout << "• Multiple queries: Preprocess with sorted array\n";
    cout << "• Very deep trees: Use iterative to avoid stack overflow\n\n";
    
    return 0;
}

/*
=============================================================================
ADDITIONAL NOTES FOR INTERVIEWS:
=============================================================================

FOLLOW-UP QUESTIONS INTERVIEWERS MIGHT ASK:

1. "What if we need to find kth smallest instead?"
   → Use normal in-order traversal instead of reverse

2. "How would you handle duplicate values?"
   → Clarify if duplicates count as separate elements or not

3. "What if the tree is not balanced?"
   → Discuss how skewed trees affect performance (O(N) instead of O(log N))

4. "Can you do this iteratively?"
   → Show the stack-based solution

5. "What about O(1) space?"
   → Demonstrate Morris traversal

6. "How would you optimize for multiple queries?"
   → Discuss preprocessing approaches

7. "What if k is very large (close to N)?"
   → Might be better to find (N-k+1)th smallest from left

COMMON MISTAKES TO AVOID:
• Forgetting to handle edge cases (k=0, k>N, null tree)
• Not optimizing for early termination in recursive solution
• Incorrect threading logic in Morris traversal
• Not explaining the BST property that enables the solution
• Mixing up largest vs smallest in the traversal order

COMPLEXITY GOTCHAS:
• Morris traversal is O(N) time even for small k (due to threading setup)
• For very small k, simple approaches might be faster in practice
• Space complexity includes implicit recursion stack
• Consider cache locality and practical performance, not just Big-O
*/