/**
 * Simple Arithmetic Language Parser
 * 
 * Parses and evaluates expressions in a simple language with two operations:
 * - add(x, y): returns x + y
 * - sub(x, y): returns x - y
 * 
 * Features:
 * - Supports nested expressions: add(sub(3,4), 1)
 * - Handles whitespace and negative numbers
 * - Provides error reporting with exact position
 * - O(n) time complexity with single-pass parsing
 */

#include<bits/stdc++.h>

class SimpleArithmeticParser {
private:
    std::string inputString;      // The expression string being parsed
    size_t currentPosition;       // Current position in the string (0-based index)
    
    /**
     * Skips all whitespace characters starting from current position
     * Moves currentPosition forward past spaces, tabs, newlines
     */
    void skipWhitespace() { 
        // Keep advancing position while we encounter whitespace
        while (currentPosition < inputString.size() && 
               std::isspace(inputString[currentPosition])) {
            currentPosition++;  // Move to next character
        }
    }
    
    /**
     * Parses an integer number from current position (handles negative numbers)
     * @return The parsed integer value
     * @throws std::runtime_error if number format is invalid
     */
    int parseInteger() {
        skipWhitespace();  // Skip any leading whitespace before number
        
        size_t errorPosition = currentPosition;  // Save position for error reporting
        bool isNegative = false;
        
        // Check if number is negative
        if (currentPosition < inputString.size() && 
            inputString[currentPosition] == '-') {
            isNegative = true;
            currentPosition++;  // Skip the negative sign
        }
        
        // After optional negative sign, we must have at least one digit
        if (currentPosition >= inputString.size() || 
            !std::isdigit(inputString[currentPosition])) {
            throw std::runtime_error("Error at " + std::to_string(errorPosition));
        }
        
        // Build the number digit by digit
        int result = 0;
        while (currentPosition < inputString.size() && 
               std::isdigit(inputString[currentPosition])) {
            // Convert character to digit and add to result
            int digit = inputString[currentPosition] - '0';
            result = result * 10 + digit;
            currentPosition++;  // Move to next character
        }
        
        // Apply negative sign if needed
        return isNegative ? -result : result;
    }
    
    /**
     * Parses a complete expression (either a function call or a number)
     * This is the main parsing function that handles recursion for nested expressions
     * @return The evaluated result of the expression
     * @throws std::runtime_error for any syntax errors
     */
    int parseExpression() {
        skipWhitespace();  // Skip leading whitespace
        
        // Check if this looks like a function call (starts with a letter)
        if (currentPosition < inputString.size() && 
            std::isalpha(inputString[currentPosition])) {
            
            // Parse function call
            size_t functionStartPosition = currentPosition;  // For error reporting
            std::string functionName;
            
            // Extract the function name character by character
            while (currentPosition < inputString.size() && 
                   std::isalpha(inputString[currentPosition])) {
                functionName += inputString[currentPosition];
                currentPosition++;
            }
            
            // Validate that we recognize this function
            if (functionName != "add" && functionName != "sub") {
                throw std::runtime_error("Error at " + std::to_string(functionStartPosition));
            }
            
            // Parse opening parenthesis '('
            skipWhitespace();  // Skip whitespace before opening paren
            if (currentPosition >= inputString.size() || 
                inputString[currentPosition] != '(') {
                throw std::runtime_error("Error at " + std::to_string(currentPosition));
            }
            currentPosition++;  // Consume the '(' character
            
            // Parse first argument (this is recursive - handles nested expressions)
            int firstArgument = parseExpression();
            
            // Parse comma separator ','
            skipWhitespace();  // Skip whitespace before comma
            if (currentPosition >= inputString.size() || 
                inputString[currentPosition] != ',') {
                throw std::runtime_error("Error at " + std::to_string(currentPosition));
            }
            currentPosition++;  // Consume the ',' character
            
            // Parse second argument (also recursive)
            int secondArgument = parseExpression();
            
            // Parse closing parenthesis ')'
            skipWhitespace();  // Skip whitespace before closing paren
            if (currentPosition >= inputString.size() || 
                inputString[currentPosition] != ')') {
                throw std::runtime_error("Error at " + std::to_string(currentPosition));
            }
            currentPosition++;  // Consume the ')' character
            
            // Evaluate the function based on its name
            if (functionName == "add") {
                return firstArgument + secondArgument;
            } else {  // functionName == "sub"
                return firstArgument - secondArgument;
            }
        }
        
        // If it doesn't start with a letter, it must be a number
        return parseInteger();
    }
    
public:
    /**
     * Main entry point: evaluates a string expression
     * Resets parser state and begins parsing from the beginning
     * @param expression The string expression to parse and evaluate
     * @return The computed result
     * @throws std::runtime_error for syntax errors with position information
     */
    int evaluateExpression(const std::string& expression) { 
        inputString = expression;        // Store the input string
        currentPosition = 0;             // Start parsing from beginning
        return parseExpression();        // Begin recursive parsing
    }
};

/**
 * Test function demonstrating parser functionality
 * Tests both valid expressions and error cases
 */
int main() {
    SimpleArithmeticParser parser;
    
    // Test valid expressions with expected results
    std::cout << "=== TESTING VALID EXPRESSIONS ===\n";
    
    // Simple function calls
    std::cout << "add(1,3) = " << parser.evaluateExpression("add(1,3)") 
              << " (expected: 4)\n";
    std::cout << "sub(1,3) = " << parser.evaluateExpression("sub(1,3)") 
              << " (expected: -2)\n";
    
    // Single level nesting (depth 2)
    std::cout << "add(sub(3,4), 1) = " << parser.evaluateExpression("add(sub(3,4), 1)") 
              << " (expected: 0)\n";
    std::cout << "sub(add(10,5), sub(8,3)) = " << parser.evaluateExpression("sub(add(10,5), sub(8,3))") 
              << " (expected: 10)\n";
    
    // Deep nesting level 3: add(add(sub(10,3), 2), 5) = add(add(7, 2), 5) = add(9, 5) = 14
    std::cout << "add(add(sub(10,3), 2), 5) = " << parser.evaluateExpression("add(add(sub(10,3), 2), 5)") 
              << " (expected: 14)\n";
    
    // Deep nesting level 4: sub(add(sub(add(20,5), 10), 3), 2) = sub(add(sub(25, 10), 3), 2) = sub(add(15, 3), 2) = sub(18, 2) = 16
    std::cout << "sub(add(sub(add(20,5), 10), 3), 2) = " << parser.evaluateExpression("sub(add(sub(add(20,5), 10), 3), 2)") 
              << " (expected: 16)\n";
    
    // Complex deep nesting level 5: add(sub(add(sub(add(100,50), 25), 10), 5), 1) 
    // = add(sub(add(sub(150, 25), 10), 5), 1) = add(sub(add(125, 10), 5), 1) = add(sub(135, 5), 1) = add(130, 1) = 131
    std::cout << "add(sub(add(sub(add(100,50), 25), 10), 5), 1) = " 
              << parser.evaluateExpression("add(sub(add(sub(add(100,50), 25), 10), 5), 1)") 
              << " (expected: 131)\n";
    
    // Whitespace handling test
    std::cout << "add( 1 , 3 ) = " << parser.evaluateExpression("add( 1 , 3 )") 
              << " (expected: 4)\n";
    
    // Negative number test
    std::cout << "sub(-5, 3) = " << parser.evaluateExpression("sub(-5, 3)") 
              << " (expected: -8)\n";
    
    // Deep nesting with negative numbers: sub(add(-10, sub(5, -3)), add(-2, 4))
    // = sub(add(-10, sub(5, -3)), add(-2, 4)) = sub(add(-10, 8), 2) = sub(-2, 2) = -4
    std::cout << "sub(add(-10, sub(5, -3)), add(-2, 4)) = " 
              << parser.evaluateExpression("sub(add(-10, sub(5, -3)), add(-2, 4))") 
              << " (expected: -4)\n";
    
    // Test error cases with position reporting
    std::cout << "\n=== TESTING ERROR CASES ===\n";
    
    // Array of invalid expressions to test
    std::string invalidExpressions[] = {
        "add(bad,2)",     // Invalid number "bad"
        "mul(1,2)",       // Unknown function "mul"  
        "add(1",          // Missing closing parenthesis
        "sub(1,)",        // Missing second argument
        ""                // Empty string
    };
    
    // Test each invalid expression
    for (const auto& testExpression : invalidExpressions) {
        try {
            // This should throw an exception
            int result = parser.evaluateExpression(testExpression);
            std::cout << "FAILED: '" << testExpression 
                      << "' should have caused an error but returned: " << result << "\n";
        } catch (const std::exception& error) {
            // This is expected - show the error message
            std::cout << "PASSED: '" << testExpression 
                      << "' correctly produced error: " << error.what() << "\n";
        }
    }
    
    return 0;
}

/*
===============================================================================
PROBLEM STATEMENT:
===============================================================================
Parse and evaluate strings in a simple arithmetic language:
- Input: "add(1,3)" → Output: 4  
- Input: "sub(1,3)" → Output: -2
- Operations take exactly 2 parameters
- Commands may be nested: "add(sub(3,4), 1)" → 0
- Handle whitespace appropriately  
- Provide syntax error handling with exact position reporting
- Numbers are integers (including negative)
- Maximum string length: 1024 characters
- Cannot use built-in eval functionality

EXAMPLES:
- "add(1,3)" → 4
- "sub(1,3)" → -2  
- "add(sub(3,4), 1)" → 0
- "sub(add(238943, 2343), add(1, sub(323, 43)))" → -238662

ERROR EXAMPLES:
- "add(bad, 2342)" → "Error at position 4" (invalid number)
- "mul(1,2)" → "Error at position 0" (unknown function)

===============================================================================
ALGORITHM ANALYSIS:
===============================================================================
TIME COMPLEXITY: O(n)
- Single pass through input string
- Each character processed exactly once
- Recursion doesn't revisit processed characters
- No backtracking or multiple passes

SPACE COMPLEXITY: O(d) 
- Where d = maximum nesting depth
- Call stack space for recursive function calls
- No additional data structures that grow with input
- Worst case: d = O(n) for fully nested expressions

APPROACH: Recursive Descent Parser
- Single left-to-right traversal
- Recursive calls handle nesting naturally  
- Position tracking for precise error reporting
- No string manipulation (replace/split) that could cause O(n²)
===============================================================================
*/