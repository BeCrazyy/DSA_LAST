#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
using namespace std;

/**
 * Trie Node structure for efficient prefix checking and word storage
 * 
 * Space Complexity: O(1) per node
 * - Each node stores: map of children + boolean + string
 * - Total trie space: O(W * L) where W = number of words, L = average word length
 */
struct TrieNode {
    unordered_map<char, TrieNode*> children;  // Character → child node mapping
    bool isEndOfWord;                         // True if a valid word ends at this node
    string word;                              // Store the complete word (for easy retrieval)
    
    TrieNode() : isEndOfWord(false) {}
};

/**
 * Word Search using DFS with String-based Prefix Pruning
 * 
 * OVERALL CLASS COMPLEXITY:
 * Time Complexity: O(N * M * 4^L * W * L)
 * - N * M: iterate through each starting cell in grid
 * - 4^L: worst case DFS explores 4 directions for L levels (max word length)
 * - W * L: hasPrefix checks W dictionary words, O(L) substring comparison each
 * - Pruning reduces practical paths explored significantly
 * 
 * Space Complexity: O(L + N * M)
 * - O(L): recursion stack depth + currentWord string length
 * - O(N * M): visited array for grid
 * - O(W): dictionary storage (given as input)
 * 
 * Best for: Small to medium dictionaries (< 1000 words)
 * Limitation: Expensive O(W*L) prefix checking becomes bottleneck for large dictionaries
 */
class AllValidWordsDFS {
private:
    /**
     * Check if the given prefix exists in any dictionary word
     * Enables early termination when current path cannot lead to valid words
     * 
     * Time Complexity: O(W * L)
     * - W: scan all dictionary words  
     * - L: substring comparison for each word
     * 
     * Space Complexity: O(1)
     * - No additional space used
     * 
     * @param prefix current string being built in DFS path
     * @param dict dictionary of valid words to search
     * @return true if at least one dictionary word starts with this prefix
     */
    static bool hasPrefix(const string& prefix, const unordered_set<string>& dict) {
        for (const string& word : dict) {
            if (word.length() >= prefix.length() && 
                word.substr(0, prefix.length()) == prefix) {
                return true;  // Found at least one word with this prefix
            }
        }
        return false;  // No dictionary word starts with this prefix
    }
    
    /**
     * DFS with string-based prefix pruning
     * Builds words character by character and checks prefixes against dictionary
     * 
     * Time Complexity: O(4^L * W * L) per starting position
     * - 4^L: worst case explores 4 directions for L levels (heavily pruned in practice)
     * - W * L: hasPrefix call at each DFS step
     * 
     * Space Complexity: O(L)
     * - O(L): recursion stack depth
     * - O(L): currentWord string length
     * - visited array is shared across calls
     */
    static void dfs(vector<vector<char>>& grid, vector<vector<bool>>& visited, 
                   int i, int j, string currentWord, 
                   const unordered_set<string>& dict, set<string>& result) {
        
        // Boundary and visited checks - O(1)
        if (i < 0 || i >= grid.size()) return;
        if (j < 0 || j >= grid[0].size()) return;  
        if (visited[i][j]) return;
        
        // Build word by adding current character - O(1) amortized
        currentWord += grid[i][j];
        
        // PREFIX PRUNING: Stop if no dictionary word starts with current prefix - O(W*L)
        if (!hasPrefix(currentWord, dict)) {
            return;  // Early termination - no valid words possible from this path
        }
        
        // Check if current word exists in dictionary - O(1) average case
        if (dict.find(currentWord) != dict.end()) {
            result.insert(currentWord);  // O(log R) where R = result set size
        }
        
        // Mark current cell as visited - O(1)
        visited[i][j] = true;
        
        // Explore 4 directions (up, down, left, right) - 4 recursive calls
        dfs(grid, visited, i-1, j, currentWord, dict, result);
        dfs(grid, visited, i+1, j, currentWord, dict, result);
        dfs(grid, visited, i, j-1, currentWord, dict, result);
        dfs(grid, visited, i, j+1, currentWord, dict, result);
        
        // Backtrack: unmark current cell - O(1)
        visited[i][j] = false;
    }
    
public:
    /**
     * Find all valid words using DFS with string-based prefix checking
     * 
     * Time Complexity: O(N * M * 4^L * W * L)
     * - N * M: try each grid cell as starting position
     * - 4^L * W * L: DFS complexity per starting position
     * 
     * Space Complexity: O(L + N * M)
     * - O(L): DFS recursion depth + string building
     * - O(N * M): visited array
     */
    static void solve(vector<vector<char>>& grid, const unordered_set<string>& dict, set<string>& result) {
        int N = grid.size();        // Grid rows
        int M = grid[0].size();     // Grid columns
        vector<vector<bool>> visited(N, vector<bool>(M, false));  // O(N*M) space
        
        // Start DFS from every cell in the grid - O(N*M) iterations
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                dfs(grid, visited, i, j, "", dict, result);  // Each call: O(4^L * W * L)
            }
        }
    }
};

/**
 * Word Search using DFS with Trie-based Optimization
 * 
 * OVERALL CLASS COMPLEXITY:
 * Time Complexity: O(W * L) preprocessing + O(N * M * 4^L) search
 * - O(W * L): one-time trie construction from dictionary
 * - O(N * M * 4^L): DFS search with O(1) prefix validation per step
 * - No W*L factor in search phase - this is the key optimization!
 * 
 * Space Complexity: O(W * L + L + N * M)
 * - O(W * L): trie storage for all dictionary characters
 * - O(L): recursion stack depth
 * - O(N * M): visited array for grid
 * 
 * Best for: Large dictionaries (1000+ words), multiple searches on same dictionary
 * Trade-off: Higher space usage for dramatically improved time complexity
 */
class AllValidWordsTrie {
private:
    TrieNode* root;  // Root of the trie - O(1) space per instance
    
    /**
     * Build trie from dictionary words for efficient prefix checking
     * Each path from root to node represents a prefix, complete paths represent words
     * 
     * Time Complexity: O(W * L)
     * - W: number of words in dictionary
     * - L: average length of words
     * - Each character insertion: O(1) average case hash map operation
     * 
     * Space Complexity: O(W * L)
     * - Worst case: no shared prefixes, store all W*L characters
     * - Best case: many shared prefixes, significantly less than W*L
     * - Practical: usually much better than worst case due to prefix sharing
     * 
     * @param dict dictionary of words to build trie from
     */
    void buildTrie(const unordered_set<string>& dict) {
        root = new TrieNode();  // O(1)
        
        // Process each word in dictionary - O(W) iterations
        for (const string& word : dict) {
            TrieNode* curr = root;
            
            // Create/traverse path for each character in word - O(L) iterations
            for (char c : word) {
                // Check if child exists - O(1) average case
                if (curr->children.find(c) == curr->children.end()) {
                    curr->children[c] = new TrieNode();  // Create new node - O(1)
                }
                curr = curr->children[c];  // Move to child node - O(1)
            }
            
            // Mark end of word and store complete word - O(1) + O(L) for string copy
            curr->isEndOfWord = true;
            curr->word = word;
        }
    }
    
    /**
     * DFS with trie-based prefix validation
     * Instead of building strings, we traverse the trie in parallel with the grid
     * 
     * Time Complexity: O(4^L) per starting position
     * - 4^L: worst case explores 4 directions for L levels (heavily pruned by O(1) checks)
     * - No W*L factor - prefix checking is O(1) via trie traversal!
     * 
     * Space Complexity: O(L)
     * - O(L): recursion stack depth
     * - No string building overhead
     * - visited array is shared across calls
     * 
     * @param grid 2D character grid
     * @param visited tracking array for current path
     * @param i, j current grid position
     * @param trieNode current position in trie (represents current prefix)
     * @param result set to store found words
     */
    void dfs(vector<vector<char>>& grid, vector<vector<bool>>& visited,
             int i, int j, TrieNode* trieNode, set<string>& result) {
        
        // Boundary and visited checks - O(1)
        if (i < 0 || i >= grid.size()) return;
        if (j < 0 || j >= grid[0].size()) return;
        if (visited[i][j]) return;
        
        char currentChar = grid[i][j];  // O(1)
        
        // TRIE OPTIMIZATION: O(1) prefix validation!
        // Check if current character extends any valid prefix in trie
        if (trieNode->children.find(currentChar) == trieNode->children.end()) {
            return;  // No trie path for this character - stop immediately! O(1)
        }
        
        // Move to next node in trie (represents extending our prefix) - O(1)
        TrieNode* nextNode = trieNode->children[currentChar];
        
        // Check if we've reached the end of a valid word - O(1)
        if (nextNode->isEndOfWord) {
            result.insert(nextNode->word);  // Found complete word! O(log R)
        }
        
        // Mark current cell as visited - O(1)
        visited[i][j] = true;
        
        // Continue DFS with the next trie node (representing extended prefix) - 4 recursive calls
        dfs(grid, visited, i-1, j, nextNode, result);  // Up
        dfs(grid, visited, i+1, j, nextNode, result);  // Down
        dfs(grid, visited, i, j-1, nextNode, result);  // Left  
        dfs(grid, visited, i, j+1, nextNode, result);  // Right
        
        // Backtrack: unmark current cell - O(1)
        visited[i][j] = false;
    }
    
public:
    /**
     * Constructor
     * Time Complexity: O(1)
     * Space Complexity: O(1)
     */
    AllValidWordsTrie() : root(nullptr) {}
    
    /**
     * Find all valid words using DFS with trie optimization
     * 
     * Time Complexity: O(W * L + N * M * 4^L)
     * - O(W * L): build trie from dictionary (one-time preprocessing)
     * - O(N * M * 4^L): DFS search from each starting position
     * - Key insight: No W*L factor in search phase!
     * 
     * Space Complexity: O(W * L + L + N * M)
     * - O(W * L): trie storage
     * - O(L): DFS recursion depth
     * - O(N * M): visited array
     */
    void solve(vector<vector<char>>& grid, const unordered_set<string>& dict, set<string>& result) {
        // Build trie from dictionary (one-time preprocessing) - O(W * L)
        buildTrie(dict);
        
        int N = grid.size();        // Grid rows
        int M = grid[0].size();     // Grid columns
        vector<vector<bool>> visited(N, vector<bool>(M, false));  // O(N*M) space
        
        // Start DFS from every cell, beginning at trie root - O(N*M) iterations
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                dfs(grid, visited, i, j, root, result);  // Each call: O(4^L)
            }
        }
    }
    
    /**
     * Clean up trie memory (destructor would handle this in production code)
     * 
     * Time Complexity: O(number of nodes in trie) = O(W * L) worst case
     * Space Complexity: O(L) for recursion stack during cleanup
     */
    void cleanup(TrieNode* node) {
        if (!node) return;
        for (auto& pair : node->children) {
            cleanup(pair.second);  // Recursive cleanup
        }
        delete node;
    }
};

/**
 * Test function comparing both DFS and Trie approaches
 * 
 * Time Complexity: O(W * L + N * M * 4^L * W * L) for DFS + O(W * L + N * M * 4^L) for Trie
 * Space Complexity: O(W * L + L + N * M) for both approaches combined
 * 
 * Tests the given example and demonstrates performance difference:
 * - Grid: 3x3 with characters C,A,T,O,S,K,P,Y,U
 * - Dictionary: CAT, COPY, ASK, SOS
 * - Expected output: CAT, COPY, ASK (SOS cannot be formed due to cell reuse)
 */
int main() {
    // Test case: Given example from problem statement
    vector<vector<char>> grid = {
        {'C', 'A', 'T'},   // Row 0: C(0,0) A(0,1) T(0,2)
        {'O', 'S', 'K'},   // Row 1: O(1,0) S(1,1) K(1,2)  
        {'P', 'Y', 'U'}    // Row 2: P(2,0) Y(2,1) U(2,2)
    };
    
    // Dictionary as unordered_set for O(1) word lookup
    unordered_set<string> dict = {"CAT", "COPY", "ASK", "SOS"};
    
    cout << "Grid:" << endl;
    for (auto& row : grid) {
        for (char c : row) {
            cout << c << " ";
        }
        cout << endl;
    }
    
    cout << "\nDictionary: ";
    for (const string& word : dict) {
        cout << word << " ";
    }
    cout << endl;
    
    // Test DFS approach with string-based prefix checking
    set<string> resultDFS;
    AllValidWordsDFS::solve(grid, dict, resultDFS);  // O(N * M * 4^L * W * L)
    
    cout << "\n=== DFS with String-based Prefix Pruning ===" << endl;
    cout << "Found words: ";
    for (const string& word : resultDFS) {
        cout << word << " ";
    }
    cout << endl;
    
    // Test Trie approach
    set<string> resultTrie;
    AllValidWordsTrie trieSolver;
    trieSolver.solve(grid, dict, resultTrie);  // O(W * L + N * M * 4^L)
    
    cout << "\n=== DFS with Trie Optimization ===" << endl;
    cout << "Found words: ";
    for (const string& word : resultTrie) {
        cout << word << " ";
    }
    cout << endl;
    
    // Verify both approaches give same results
    cout << "\nResults match: " << (resultDFS == resultTrie ? "YES" : "NO") << endl;
    
    /* Expected paths for found words:
     * CAT:  C(0,0) → A(0,1) → T(0,2)
     * COPY: C(0,0) → O(1,0) → P(2,0) → Y(2,1)  
     * ASK:  A(0,1) → S(1,1) → K(1,2)
     * SOS:  Cannot be formed - would require S(1,1) twice in same path
     * 
     * Performance comparison for this example:
     * DFS: O(9 * 4^4 * 4 * 4) = O(9 * 256 * 16) = O(36,864) operations
     * Trie: O(4 * 4 + 9 * 4^4) = O(16 + 9 * 256) = O(2,320) operations
     * 
     * Trie is ~16x faster even for this small example!
     * For larger dictionaries, the difference becomes exponentially greater.
     */
    
    return 0;
}

/*
COMPREHENSIVE COMPLEXITY ANALYSIS COMPARISON:

=== DFS with String-based Prefix Pruning ===
Time Complexity: O(N * M * 4^L * W * L)
- N * M: starting positions in grid
- 4^L: possible paths of length L (reduced by pruning)
- W * L: hasPrefix function scans W words, O(L) per substring comparison
- Dominates for large dictionaries due to W*L factor

Space Complexity: O(L + N * M)
- O(L): recursion depth + currentWord string length
- O(N * M): visited array

=== DFS with Trie Optimization ===  
Time Complexity: O(W * L) preprocessing + O(N * M * 4^L) search
- O(W * L): one-time trie construction
- O(N * M * 4^L): DFS search with O(1) prefix checks
- Key advantage: Eliminates W*L factor from search phase!

Space Complexity: O(W * L + L + N * M)
- O(W * L): trie storage for all dictionary words
- O(L): recursion depth  
- O(N * M): visited array

PERFORMANCE SCALING:
For grid size N×N and dictionary size W:
- DFS approach: Time grows as O(N² * W) - linear in dictionary size
- Trie approach: Time grows as O(N²) after preprocessing - independent of dictionary size!

WHEN TO USE EACH:
- Small dictionaries (W < 100): DFS approach is simpler, performance difference minimal
- Medium dictionaries (100 ≤ W ≤ 1000): Trie starts showing significant benefits
- Large dictionaries (W > 1000): Trie approach becomes essential for reasonable performance
- Multiple searches: Trie preprocessing cost is amortized across searches

MEMORY TRADE-OFF:
- DFS: Low memory usage, higher time complexity
- Trie: Higher memory usage, dramatically lower time complexity
- Rule of thumb: If you have memory available, trie optimization is almost always worth it

PROBLEM STATEMENT:
Given a 2-dimensional grid of characters and a dictionary, find all words in the grid 
that also appear in the dictionary. A word can be formed by traversing the grid by 
going either left, right, top, or down, but NOT diagonal. Also, a single grid 
position cannot be used more than once in a word.

Example:
Grid:   C A T
        O S K  
        P Y U
        
Dictionary: [CAT, COPY, ASK, SOS]

Output: [CAT, COPY, ASK]
- CAT: C(0,0) -> A(0,1) -> T(0,2)
- COPY: C(0,0) -> O(1,0) -> P(2,0) -> Y(2,1)  
- ASK: A(0,1) -> S(1,1) -> K(1,2)
- SOS: Cannot be formed because S(1,1) would need to be used twice
*/