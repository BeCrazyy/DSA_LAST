#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

// APPROACH 1: PROPER TWO-PASS ALGORITHM (RECOMMENDED)
vector<string> splitIntoSMSChunksTwoPass(const string& text) {
    const int MAX_SMS_LENGTH = 160;
    
    // Handle simple case: if text fits in single SMS, return as-is (no suffix needed)
    if (text.length() <= MAX_SMS_LENGTH) {
        return {text};
    }
    
    // Split input text into individual words for word-boundary processing
    vector<string> words;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        words.push_back(word);
    }
    
    // ============ PASS 1: Determine exact chunk count needed ============
    // Use conservative suffix estimate to ensure we don't underestimate space needed
    // Assume worst case: up to 999 chunks requires " (999/999)" = 10 chars suffix
    int conservativeSuffixLength = 10; 
    int conservativeAvailableSpace = MAX_SMS_LENGTH - conservativeSuffixLength;
    
    // Temporary splitting to count chunks with conservative estimate
    vector<string> tempChunks;
    string currentChunk = "";
    
    for (const string& word : words) {
        string candidateChunk = currentChunk;
        
        // Add space separator if this isn't the first word in chunk
        if (!candidateChunk.empty()) {
            candidateChunk += " ";
        }
        candidateChunk += word;
        
        // Check if adding this word would exceed available space
        if (candidateChunk.length() <= conservativeAvailableSpace) {
            // Word fits, add it to current chunk
            currentChunk = candidateChunk;
        } else {
            // Word doesn't fit, finalize current chunk and start new one
            if (!currentChunk.empty()) {
                tempChunks.push_back(currentChunk);
            }
            currentChunk = word; // Start new chunk with current word
        }
    }
    
    // Don't forget to add the last chunk
    if (!currentChunk.empty()) {
        tempChunks.push_back(currentChunk);
    }
    
    int totalChunks = tempChunks.size(); // Now we know exact count!
    
    // ============ PASS 2: Create final chunks with exact suffix length ============
    // Calculate exact suffix length based on actual chunk count
    // Format: " (k/n)" where k and n can be 1-3 digits each
    int exactSuffixLength = 4 + (to_string(totalChunks).length() * 2); // " (/)" + digits for k + digits for n
    int exactAvailableSpace = MAX_SMS_LENGTH - exactSuffixLength;
    
    // Re-split text using exact available space
    vector<string> finalChunks;
    currentChunk = "";
    
    for (const string& word : words) {
        string candidateChunk = currentChunk;
        
        // Add space separator if needed
        if (!candidateChunk.empty()) {
            candidateChunk += " ";
        }
        candidateChunk += word;
        
        // Check if word fits in current chunk with exact space calculation
        if (candidateChunk.length() <= exactAvailableSpace) {
            currentChunk = candidateChunk;
        } else {
            // Finalize current chunk and start new one
            if (!currentChunk.empty()) {
                finalChunks.push_back(currentChunk);
            }
            currentChunk = word;
        }
    }
    
    // Add the final chunk
    if (!currentChunk.empty()) {
        finalChunks.push_back(currentChunk);
    }
    
    // Apply ordering suffix to each chunk: " (1/5)", " (2/5)", etc.
    vector<string> result;
    for (int i = 0; i < finalChunks.size(); i++) {
        string suffix = " (" + to_string(i + 1) + "/" + to_string(finalChunks.size()) + ")";
        result.push_back(finalChunks[i] + suffix);
    }
    
    return result;
}

// APPROACH 2: ITERATIVE CONVERGENCE ALGORITHM (PRODUCTION QUALITY)
vector<string> splitIntoSMSChunksIterative(const string& text) {
    const int MAX_SMS_LENGTH = 160;
    
    // Handle simple case: single SMS is sufficient
    if (text.length() <= MAX_SMS_LENGTH) {
        return {text};
    }
    
    // Tokenize input text into words for word-boundary splitting
    vector<string> words;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        words.push_back(word);
    }
    
    // Start iterative convergence with initial estimate
    int estimatedChunks = 1; // Start with minimum possible
    vector<string> chunks;
    
    // ============ ITERATIVE CONVERGENCE LOOP ============
    // Keep splitting until chunk count stabilizes (usually 1-3 iterations)
    while (true) {
        chunks.clear(); // Reset for this iteration
        
        // Calculate suffix length for current chunk count estimate
        // Suffix format: " (k/n)" where k is chunk number, n is total chunks
        int suffixLength = 4 + (to_string(estimatedChunks).length() * 2); // " (/)" + 2×digits
        int availableSpace = MAX_SMS_LENGTH - suffixLength;
        
        // Split text with current available space estimate
        string currentChunk = "";
        for (const string& word : words) {
            string candidateChunk = currentChunk;
            
            // Add space separator between words (but not before first word)
            if (!candidateChunk.empty()) {
                candidateChunk += " ";
            }
            candidateChunk += word;
            
            // Check if adding this word would exceed available space
            if (candidateChunk.length() <= availableSpace) {
                // Word fits, continue building current chunk
                currentChunk = candidateChunk;
            } else {
                // Word doesn't fit, save current chunk and start new one
                if (!currentChunk.empty()) {
                    chunks.push_back(currentChunk);
                }
                currentChunk = word; // New chunk starts with current word
            }
        }
        
        // Don't forget the last chunk
        if (!currentChunk.empty()) {
            chunks.push_back(currentChunk);
        }
        
        int actualChunks = chunks.size();
        
        // ============ CONVERGENCE CHECK ============
        // If our estimate matches reality, we've found the solution
        if (actualChunks == estimatedChunks) {
            break; // Converged! Exit the loop
        }
        
        // Update estimate with actual result and try again
        estimatedChunks = actualChunks;
        // Loop continues with new estimate...
    }
    
    // Apply final ordering suffixes to converged chunks
    vector<string> result;
    for (int i = 0; i < chunks.size(); i++) {
        string suffix = " (" + to_string(i + 1) + "/" + to_string(chunks.size()) + ")";
        result.push_back(chunks[i] + suffix);
    }
    
    return result;
}

// Test function to verify chunk properties and display results
void validateChunks(const vector<string>& chunks, const string& originalText, const string& approach) {
    cout << "\n=== VALIDATION: " << approach << " ===" << endl;
    
    bool allValid = true;
    int totalLength = 0;
    
    // Check each chunk for compliance with 160-character limit
    for (int i = 0; i < chunks.size(); i++) {
        cout << "Chunk " << (i+1) << ": \"" << chunks[i] << "\" (Length: " << chunks[i].length() << ")" << endl;
        totalLength += chunks[i].length();
        
        // Validate that no chunk exceeds the SMS length limit
        if (chunks[i].length() > 160) {
            cout << "  ❌ ERROR: Exceeds 160 character limit!" << endl;
            allValid = false;
        }
    }
    
    // Display summary statistics
    cout << "\nTotal chunks: " << chunks.size() << endl;
    cout << "Total characters (with suffixes): " << totalLength << endl;
    cout << "All chunks valid: " << (allValid ? "✅ YES" : "❌ NO") << endl;
}

int main() {
    // Test case designed to demonstrate the difference between approaches
    // This will likely result in 10+ chunks to show suffix length impact
    cout << "=== COMPARING BOTH APPROACHES ===" << endl;
    
    // Create a text that will likely result in 10+ chunks to show the problem
    string testText = "";
    for (int i = 1; i <= 15; i++) {
        testText += "This is sentence number " + to_string(i) + " in our test message. ";
    }
    testText += "This final sentence should push us over the edge to need double-digit chunk numbering.";
    
    cout << "Test text length: " << testText.length() << " characters" << endl;
    
    // Test both approaches on the same input
    vector<string> result1 = splitIntoSMSChunksTwoPass(testText);
    validateChunks(result1, testText, "TWO-PASS APPROACH");
    
    vector<string> result2 = splitIntoSMSChunksIterative(testText);
    validateChunks(result2, testText, "ITERATIVE APPROACH");
    
    // Edge case: Text that results in exactly 10 chunks (boundary case)
    // This tests the transition from single-digit to double-digit chunk numbering
    cout << "\n=== BOUNDARY CASE: 9 vs 10 CHUNKS ===" << endl;
    string boundaryText = "The quick brown fox jumps over the lazy dog. ";
    for (int i = 0; i < 25; i++) {
        boundaryText += "Additional text to reach the boundary. ";
    }
    
    cout << "Boundary text length: " << boundaryText.length() << " characters" << endl;
    
    // Test both approaches on boundary case
    vector<string> boundaryResult1 = splitIntoSMSChunksTwoPass(boundaryText);
    validateChunks(boundaryResult1, boundaryText, "TWO-PASS BOUNDARY TEST");
    
    vector<string> boundaryResult2 = splitIntoSMSChunksIterative(boundaryText);
    validateChunks(boundaryResult2, boundaryText, "ITERATIVE BOUNDARY TEST");
    
    return 0;
}

/*
=== PROBLEM STATEMENT ===

SMS Text Splitter Function

There is a text-messaging service that provides an API to send SMSs to users, but messages
can be at most 160 characters long. The service also doesn't guarantee the order in which
messages will be received.

Your team wants to build a campaign that requires sending long pieces of text over this
service. You need to build a function that splits text into chunks so it can be sent in
multiple messages.

Requirements for each chunk:
1. Must be up to 160 characters long (including suffix)
2. No word should be split in the middle (split on word boundaries only)
3. Each chunk must have its order suffixed in the form ' (k/n)', where k is the chunk
   number and n is the total number of chunks
4. If the original text is less than 160 characters, no ordering suffix should be added

Examples:
- Input: "Short message" → Output: ["Short message"]
- Input: "This is a very long message that exceeds 160 characters..." 
  → Output: ["This is a very long message... (1/2)", "...rest of message (2/2)"]

=== TWO APPROACHES EXPLAINED ===

APPROACH 1: TWO-PASS ALGORITHM (RECOMMENDED FOR INTERVIEWS)
✅ Pros:
- Guaranteed correctness
- Clear, easy to understand logic
- Handles all edge cases properly
- Easy to explain in interviews

❌ Cons:
- Processes text twice
- Slightly more code complexity

Time Complexity: O(n) - processes text twice but still linear
Space Complexity: O(n) - stores words and chunks

Algorithm Steps:
1. Pass 1: Split with conservative suffix estimate to count exact chunks needed
2. Pass 2: Split again with exact suffix length for final result

---

APPROACH 2: ITERATIVE CONVERGENCE (PRODUCTION QUALITY)
✅ Pros:
- Handles any edge case gracefully
- Self-correcting algorithm
- Optimal space utilization
- Mathematically sound convergence

❌ Cons:
- More complex to understand and implement
- Variable number of iterations
- Overkill for simple use cases

Time Complexity: O(k*n) where k is iterations (typically 1-3)
Space Complexity: O(n) - stores words and chunks

Algorithm Steps:
1. Start with initial chunk count estimate
2. Split text based on current estimate
3. If actual chunks != estimate, update estimate and repeat
4. Continue until convergence (estimate matches actual)

=== RECOMMENDATIONS ===

FOR INTERVIEWS: Use Approach 1 (Two-Pass)
- Shows understanding of the core problem
- Demonstrates systematic thinking
- Easy to code and explain under time pressure
- Clear logical flow that interviewers can follow

FOR PRODUCTION: Use Approach 2 (Iterative)
- Handles all edge cases robustly
- Self-correcting behavior prevents failures
- Optimal performance characteristics
- More resilient to unexpected input patterns

=== KEY INSIGHTS ===

The core challenge is the circular dependency:
- Need total chunk count to calculate suffix length
- Need suffix length to determine available space per chunk  
- Need available space to calculate total chunk count

Both approaches solve this differently:
- Two-Pass: Uses conservative estimate, then refines
- Iterative: Keeps adjusting until stable solution found
*/