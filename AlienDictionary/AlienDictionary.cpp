#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cassert>

using namespace std;

class Solution {
public:
    /**
     * Performs BFS-based topological sorting using Kahn's algorithm
     * @param adjacencyList: Graph representing character dependencies
     * @param indegree: Map storing in-degree count for each character
     * @return: Topologically sorted string of characters, empty if cycle detected
     */
    string bfs(map<char, vector<char>>& adjacencyList, unordered_map<char, int>& indegree) {
        int uniqueChars = indegree.size();
        queue<char> q;
        
        // Add all characters with in-degree 0 to queue (starting points)
        for(auto& pair : indegree) {
            if(pair.second == 0) {
                q.push(pair.first);
            }
        }
        
        string topologicalSort = "";
        
        // Process characters in topological order
        while(!q.empty()) {
            char currentChar = q.front();
            q.pop();
            topologicalSort.push_back(currentChar);
            
            // Reduce in-degree of adjacent characters
            for(char adjacent : adjacencyList[currentChar]) {
                indegree[adjacent]--;
                if(indegree[adjacent] == 0) {
                    q.push(adjacent);
                }
            }
        }
        
        // Return result only if all characters were processed (no cycle)
        return (topologicalSort.size() == uniqueChars) ? topologicalSort : "";
    }
    
    /**
     * Finds the character order in alien language from lexicographically sorted dictionary
     * @param words: Vector of words sorted according to alien language rules
     * @return: String representing character order, empty if invalid input
     */
    string alienOrder(vector<string>& words) {
        // Handle empty input
        if(words.empty()) {
            return "";
        }
        
        map<char, vector<char>> adjacencyList;  // Graph: char -> list of chars that come after it
        unordered_map<char, int> indegree;      // In-degree count for each character
        
        // Initialize in-degree for all characters to 0
        for(const string& word : words) {
            for(char ch : word) {
                indegree[ch] = 0;
            }
        }
        
        // Build the graph by comparing adjacent words
        for(int i = 0; i < words.size() - 1; i++) {
            string word1 = words[i];
            string word2 = words[i + 1];
            int len1 = word1.length();
            int len2 = word2.length();
            
            // Edge case: Invalid lexicographic order
            // If word1 is longer and word2 is its prefix, it's invalid
            // Example: ["abcd", "abc"] is invalid because "abc" should come before "abcd"
            if(len1 > len2 && word1.substr(0, len2) == word2) {
                return "";
            }
            
            // Find the first differing character and create edge
            for(int j = 0; j < min(len1, len2); j++) {
                char char1 = word1[j];
                char char2 = word2[j];
                
                if(char1 == char2) {
                    continue;  // Characters are same, check next position
                }
                
                // Found first difference: char1 comes before char2 in alien alphabet
                adjacencyList[char1].push_back(char2);
                indegree[char2]++;
                break;  // Only need first difference between two words
            }
        }
        
        return bfs(adjacencyList, indegree);
    }
};

/* 
 * TIME COMPLEXITY: O(N*M + V + E)
 * - N: Number of words in input
 * - M: Average length of words
 * - V: Number of unique characters
 * - E: Number of edges in the graph
 * 
 * Breakdown:
 * - Building indegree map: O(N*M)
 * - Comparing adjacent words: O(N*M) 
 * - BFS traversal: O(V + E)
 * 
 * SPACE COMPLEXITY: O(V + E)
 * - Adjacency list: O(E)
 * - Indegree map: O(V)
 * - Queue: O(V) in worst case
 */

// Test function with assert statements
void testAlienDictionary() {
    Solution sol;
    cout << "=== ALIEN DICTIONARY TEST CASES ===" << endl << endl;
    
    // Test Case 1: Basic example
    {
        cout << "1. BASIC EXAMPLE:" << endl;
        vector<string> words1 = {"xww", "wxyz", "wxyw", "ywx", "ywz"};
        string result1 = sol.alienOrder(words1);
        cout << "Input: {\"xww\", \"wxyz\", \"wxyw\", \"ywx\", \"ywz\"}" << endl;
        cout << "Output: \"" << result1 << "\"" << endl;
        assert(result1 == "xzwy");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 2: Complex dependencies
    {
        cout << "2. COMPLEX DEPENDENCIES:" << endl;
        vector<string> words2 = {"baa", "abcd", "abca", "cab", "cad"};
        string result2 = sol.alienOrder(words2);
        cout << "Input: {\"baa\", \"abcd\", \"abca\", \"cab\", \"cad\"}" << endl;
        cout << "Output: \"" << result2 << "\"" << endl;
        assert(result2 == "bdac");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 3: Simple ordering
    {
        cout << "3. SIMPLE ORDERING:" << endl;
        vector<string> words3 = {"caa", "aaa", "aab"};
        string result3 = sol.alienOrder(words3);
        cout << "Input: {\"caa\", \"aaa\", \"aab\"}" << endl;
        cout << "Output: \"" << result3 << "\"" << endl;
        assert(result3 == "cab");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 4: Invalid - Prefix violation
    {
        cout << "4. INVALID PREFIX VIOLATION:" << endl;
        vector<string> words4 = {"abc", "ab"};
        string result4 = sol.alienOrder(words4);
        cout << "Input: {\"abc\", \"ab\"}" << endl;
        cout << "Output: \"" << result4 << "\"" << endl;
        assert(result4 == "");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 5: Invalid - Cycle detection
    {
        cout << "5. INVALID CYCLE DETECTION:" << endl;
        vector<string> words5 = {"z", "x", "z"};
        string result5 = sol.alienOrder(words5);
        cout << "Input: {\"z\", \"x\", \"z\"}" << endl;
        cout << "Output: \"" << result5 << "\"" << endl;
        assert(result5 == "");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 6: Single character words
    {
        cout << "6. SINGLE CHARACTER WORDS:" << endl;
        vector<string> words6 = {"a", "b", "c", "d"};
        string result6 = sol.alienOrder(words6);
        cout << "Input: {\"a\", \"b\", \"c\", \"d\"}" << endl;
        cout << "Output: \"" << result6 << "\"" << endl;
        assert(result6 == "abcd");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 7: Single word - any permutation valid
    {
        cout << "7. SINGLE WORD INPUT:" << endl;
        vector<string> words7 = {"abcde"};
        string result7 = sol.alienOrder(words7);
        cout << "Input: {\"abcde\"}" << endl;
        cout << "Output: \"" << result7 << "\"" << endl;
        // Check if result contains all chars from input and has correct length
        assert(result7.length() == 5);
        assert(result7.find('a') != string::npos && result7.find('b') != string::npos && 
               result7.find('c') != string::npos && result7.find('d') != string::npos && 
               result7.find('e') != string::npos);
        cout << "✅ PASSED (valid permutation)" << endl << endl;
    }
    
    // Test Case 8: Identical words - any permutation valid
    {
        cout << "8. IDENTICAL WORDS:" << endl;
        vector<string> words8 = {"abc", "abc", "abc"};
        string result8 = sol.alienOrder(words8);
        cout << "Input: {\"abc\", \"abc\", \"abc\"}" << endl;
        cout << "Output: \"" << result8 << "\"" << endl;
        // Check if result contains all chars and has correct length
        assert(result8.length() == 3);
        assert(result8.find('a') != string::npos && result8.find('b') != string::npos && result8.find('c') != string::npos);
        cout << "✅ PASSED (valid permutation)" << endl << endl;
    }
    
    // Test Case 9: Valid prefix relationship - e before y
    {
        cout << "9. VALID PREFIX RELATIONSHIP:" << endl;
        vector<string> words9 = {"app", "apple", "apply"};
        string result9 = sol.alienOrder(words9);
        cout << "Input: {\"app\", \"apple\", \"apply\"}" << endl;
        cout << "Output: \"" << result9 << "\"" << endl;
        // Check if 'e' comes before 'y'
        size_t pos_e = result9.find('e');
        size_t pos_y = result9.find('y');
        assert(pos_e != string::npos && pos_y != string::npos && pos_e < pos_y);
        cout << "✅ PASSED (e before y)" << endl << endl;
    }
    
    // Test Case 10: No common prefix - c < d < e < f
    {
        cout << "10. NO COMMON PREFIX:" << endl;
        vector<string> words10 = {"cat", "dog", "elephant", "fox"};
        string result10 = sol.alienOrder(words10);
        cout << "Input: {\"cat\", \"dog\", \"elephant\", \"fox\"}" << endl;
        cout << "Output: \"" << result10 << "\"" << endl;
        // Check if c < d < e < f
        size_t pos_c = result10.find('c'), pos_d = result10.find('d');
        size_t pos_e = result10.find('e'), pos_f = result10.find('f');
        assert(pos_c < pos_d && pos_d < pos_e && pos_e < pos_f);
        cout << "✅ PASSED (c < d < e < f)" << endl << endl;
    }
    
    // Test Case 11: Empty input
    {
        cout << "11. EMPTY INPUT:" << endl;
        vector<string> words11 = {};
        string result11 = sol.alienOrder(words11);
        cout << "Input: {}" << endl;
        cout << "Output: \"" << result11 << "\"" << endl;
        assert(result11 == "");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 12: Multiple solutions - c < b constraint
    {
        cout << "12. MULTIPLE VALID SOLUTIONS:" << endl;
        vector<string> words12 = {"ac", "ab", "zc", "zb"};
        string result12 = sol.alienOrder(words12);
        cout << "Input: {\"ac\", \"ab\", \"zc\", \"zb\"}" << endl;
        cout << "Output: \"" << result12 << "\"" << endl;
        // Check if c comes before b (from ac->ab and zc->zb)
        size_t pos_c = result12.find('c');
        size_t pos_b = result12.find('b');
        assert(pos_c != string::npos && pos_b != string::npos && pos_c < pos_b);
        cout << "✅ PASSED (c < b)" << endl << endl;
    }
    
    // Test Case 13: Long dependency chain
    {
        cout << "13. LONG DEPENDENCY CHAIN:" << endl;
        vector<string> words13 = {"f", "e", "d", "c", "b", "a"};
        string result13 = sol.alienOrder(words13);
        cout << "Input: {\"f\", \"e\", \"d\", \"c\", \"b\", \"a\"}" << endl;
        cout << "Output: \"" << result13 << "\"" << endl;
        assert(result13 == "fedcba");
        cout << "✅ PASSED" << endl << endl;
    }
    
    // Test Case 14: Mixed length - a < c and b < c
    {
        cout << "14. MIXED LENGTH ORDERING:" << endl;
        vector<string> words14 = {"b", "ba", "bc", "ca"};
        string result14 = sol.alienOrder(words14);
        cout << "Input: {\"b\", \"ba\", \"bc\", \"ca\"}" << endl;
        cout << "Output: \"" << result14 << "\"" << endl;
        // Check constraints: a < c and b < c
        size_t pos_a = result14.find('a'), pos_b = result14.find('b'), pos_c = result14.find('c');
        assert(pos_a < pos_c && pos_b < pos_c);
        cout << "✅ PASSED (a < c and b < c)" << endl << endl;
    }
    
    // Test Case 15: Complex case - b < d < c
    {
        cout << "15. COMPLEX ORDERING:" << endl;
        vector<string> words15 = {"ab", "adc", "ac"};
        string result15 = sol.alienOrder(words15);
        cout << "Input: {\"ab\", \"adc\", \"ac\"}" << endl;
        cout << "Output: \"" << result15 << "\"" << endl;
        // Check constraints: b < d and d < c
        size_t pos_b = result15.find('b'), pos_d = result15.find('d'), pos_c = result15.find('c');
        assert(pos_b < pos_d && pos_d < pos_c);
        cout << "✅ PASSED (b < d < c)" << endl << endl;
    }
    
    cout << "🎉 ALL 15 TEST CASES PASSED! 🎉" << endl;
    cout << "=== END OF TEST CASES ===" << endl;
}

int main() {
    testAlienDictionary();
    return 0;
}

/*
 * PROBLEM DESCRIPTION:
 * 
 * Given a lexicographically sorted dictionary (array of words) of an alien language, 
 * find the order of characters in the alien alphabet.
 * 
 * The words are sorted according to the alien language's lexicographic rules, which 
 * means if word A comes before word B in the dictionary, then word A is 
 * lexicographically smaller than word B in the alien language.
 * 
 * EXAMPLES:
 * - If "xww" comes before "wxyz", it means 'x' comes before 'w' in the alien alphabet
 * - If "wxyz" comes before "wxyw", it means 'z' comes before 'w' in the alien alphabet
 * 
 * INPUT: Vector of strings representing words in sorted order
 * OUTPUT: String representing the character order in alien alphabet
 * 
 * CONSTRAINTS:
 * - Each character is a lowercase English letter
 * - The input represents a valid alien dictionary (unless cycle exists)
 * - Return empty string if no valid ordering exists
 * 
 * ALGORITHM APPROACH:
 * 1. Treat this as a topological sorting problem
 * 2. Build a directed graph where edges represent character ordering
 * 3. Use Kahn's algorithm (BFS-based topological sort) to find valid ordering
 * 4. Return empty string if cycle detected (invalid input)
 * 
 * EDGE CASES:
 * - Invalid lexicographic order: ["abc", "ab"] → ""
 * - Cycle detection: ["z", "x", "z"] → ""
 * - Single word: ["abcde"] → any permutation valid
 * - Empty input: [] → ""
 * - Identical words: ["abc", "abc"] → any permutation valid
 */