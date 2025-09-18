#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

/**
 * Verifies if a text follows the order rule defined by an order string.
 * 
 * @param orderString The string defining the order rule (e.g., "abcd")
 * @param text The text to verify against the order rule
 * @return true if text follows the order rule, false otherwise
 * 
 * Time Complexity: O(n + m) where n is length of text and m is length of orderString
 * Space Complexity: O(m) where m is length of orderString (for the position map)
 * 
 * Algorithm:
 * 1. Create a map to store the order position of each character in orderString
 * 2. Track the maximum order position seen so far while scanning the text
 * 3. For each character in text that exists in orderString:
 *    - If its order position is less than the maximum seen so far, violation found
 *    - Otherwise, update the maximum order position
 */
bool verifyOrderRule(const string& orderString, const string& text) {
    // Map each character in orderString to its position (0-indexed)
    unordered_map<char, int> charToOrder;
    for (int i = 0; i < orderString.length(); i++) {
        charToOrder[orderString[i]] = i;
    }
    
    // Track the maximum order position seen so far
    int maxOrderSeen = -1;
    
    // Scan through the text
    for (char c : text) {
        // If character is part of the order string
        if (charToOrder.find(c) != charToOrder.end()) {
            int currentOrder = charToOrder[c];
            
            // If current character's order is less than max seen so far,
            // it means this character appears after some character that should come later
            if (currentOrder < maxOrderSeen) {
                return false; // Order violation found
            }
            
            // Update the maximum order seen
            maxOrderSeen = max(maxOrderSeen, currentOrder);
        }
    }
    
    return true; // No violations found
}

int main() {
    // Test cases
    string orderString = "abcd";
    
    // Test case 1: Valid order
    string text1 = "axubxcxd";
    cout << "Order string: " << orderString << endl;
    cout << "Text: " << text1 << endl;
    cout << "Follows order rule: " << (verifyOrderRule(orderString, text1) ? "Yes" : "No") << endl;
    cout << endl;
    
    // Test case 2: Invalid order (given example)
    string text2 = "axubbxcxbxd";
    cout << "Order string: " << orderString << endl;
    cout << "Text: " << text2 << endl;
    cout << "Follows order rule: " << (verifyOrderRule(orderString, text2) ? "Yes" : "No") << endl;
    cout << endl;
    
    // Test case 3: Valid order with repetitions
    string text3 = "aaabbbcccddd";
    cout << "Order string: " << orderString << endl;
    cout << "Text: " << text3 << endl;
    cout << "Follows order rule: " << (verifyOrderRule(orderString, text3) ? "Yes" : "No") << endl;
    cout << endl;
    
    // Test case 4: Invalid order
    string text4 = "abcdcba";
    cout << "Order string: " << orderString << endl;
    cout << "Text: " << text4 << endl;
    cout << "Follows order rule: " << (verifyOrderRule(orderString, text4) ? "Yes" : "No") << endl;
    
    return 0;
}

/*
Problem Description:
================

Given an order string and a text, verify whether the text follows the order rule defined by the order string.

The order rule states that for an order string like "abcd":
- 'a' cannot appear at any position after 'b', 'c', or 'd' in the text
- 'b' cannot appear at any position after 'c' or 'd' in the text  
- 'c' cannot appear at any position after 'd' in the text

Example:
- Order string: "abcd"
                 0123   
- Text: "axubbxcxbxd"
         0--11-2-
- Result: Invalid (violates rule because 'b' appears after 'c' in substring "cxb")

The algorithm works by:
1. Mapping each character in the order string to its position
2. Scanning the text left to right
3. Tracking the maximum order position seen so far
4. For each character from the order string found in text:
   - If its order position < maximum seen so far → violation found
   - Otherwise, update the maximum order position seen

Time Complexity: O(n + m) where n = text length, m = order string length
Space Complexity: O(m) for storing character-to-order mapping
*/