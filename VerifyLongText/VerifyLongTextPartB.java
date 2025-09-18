import java.util.*;
import java.util.concurrent.*;

/**
 * Result class to store processing outcome from each worker thread
 * 
 * Uses volatile fields for thread-safe communication between worker threads
 * and the main polling thread without explicit synchronization overhead.
 * 
 * Fields:
 * - min: minimum order position found in this chunk (0 if no order chars found)
 * - max: maximum order position found in this chunk  
 * - state: processing status (-1: running, 0: failed, 1: succeeded)
 */
class Result {
    volatile int min = 0, max = 0, state = -1; // -1: running, 0: failed, 1: success
}

/**
 * Multi-threaded solution for order verification problem
 * 
 * Problem: Given an order string like "abcd", verify if a text follows the rule:
 * - 'a' cannot appear after 'b', 'c', or 'd'
 * - 'b' cannot appear after 'c' or 'd' 
 * - 'c' cannot appear after 'd'
 * 
 * Approach:
 * 1. Split text into chunks (one per thread)
 * 2. Each thread finds min/max order positions in its chunk
 * 3. Validate that chunks maintain order: chunk1.max <= chunk2.min <= chunk3.min...
 * 
 * Time Complexity: O(n/k + m) where n=text length, k=threads, m=order length
 * Space Complexity: O(k) for results + O(1) for order mapping
 */
class Solution {
    private final int numThreads = 4;
    private final ExecutorService executor = Executors.newFixedThreadPool(numThreads);
    
    /**
     * Verify if text follows the order rule using multi-threading
     * 
     * @param text The text to verify
     * @param order The order string defining the rule (e.g., "abcd")
     * @return true if text follows order rule, false otherwise
     * @throws InterruptedException if thread is interrupted
     * 
     * Key Optimizations:
     * 1. Start from first unfinished result (avoid re-checking processed results)
     * 2. Early failure detection (don't wait for all threads to complete)
     * 3. Proper synchronization with volatile fields
     */
    public boolean isOrdered(String text, String order) throws InterruptedException {
        // Build order mapping: char -> position (1-indexed, 0 = not in order)
        int[] orderSeq = new int[256];
        for (int i = 0; i < order.length(); i++) {
            orderSeq[order.charAt(i)] = i + 1;
        }
        
        // Create results array and submit worker tasks
        Result[] results = new Result[numThreads];
        for (int i = 0; i < numThreads; i++) {
            results[i] = new Result();
            final int index = i;
            
            // Submit task to process one chunk of text
            executor.submit(() -> {
                String chunk = getChunk(text, index);
                int currMax = 0, currMin = 0;
                
                // Process each character in the chunk
                for (char c : chunk.toCharArray()) {
                    if (orderSeq[c] == 0) continue; // Skip chars not in order string
                    
                    if (currMin == 0) currMin = orderSeq[c]; // First order char found
                    if (orderSeq[c] < currMax) {
                        results[index].state = 0; // Order violation within chunk
                        return;
                    }
                    currMax = orderSeq[c];
                }
                
                // Chunk processed successfully
                results[index].min = currMin;
                results[index].max = currMax;
                results[index].state = 1;
            });
        }
        
        // Poll results with optimization: start from first unfinished result
        // This avoids re-checking already processed results in each iteration
        int start = 0, prevMax = 0;
        
        while (true) {
            boolean allDone = true;
            int currMax = prevMax;
            
            // Start checking from 'start' index instead of 0
            for (int i = start; i < numThreads; i++) {
                Result r = results[i];
                
                if (r.state == 0) return false; // Early failure detection
                
                if (r.state == -1) { // Still running
                    if (allDone) {
                        start = i; // Remember first unfinished for next iteration
                        prevMax = currMax; // Remember progress so far
                        allDone = false;
                    }
                    continue;
                }
                
                // State == 1 (success): validate order between chunks
                if (r.min > 0 && r.min < currMax) return false; // Order violation
                if (r.min > 0) currMax = r.max; // Update max for next chunk
            }
            
            if (allDone) break; // All threads completed successfully
            Thread.sleep(1); // Small delay to avoid busy waiting
        }
        
        return true;
    }
    
    /**
     * Extract substring for a specific thread to process
     * 
     * Chunk Distribution Strategy:
     * - Divide text length by number of threads for base chunk size
     * - Each thread gets equal-sized chunk except the last one
     * - Last thread gets any remainder characters to ensure complete coverage
     * 
     * Example: text.length=102, numThreads=4
     * - Base chunk size = 102/4 = 25
     * - Thread 0: chars 0-24 (25 chars)
     * - Thread 1: chars 25-49 (25 chars) 
     * - Thread 2: chars 50-74 (25 chars)
     * - Thread 3: chars 75-101 (27 chars) ← gets remainder
     * 
     * @param text Full text to be divided
     * @param index Thread index (0 to numThreads-1)
     * @return Substring for this thread to process
     */
    private String getChunk(String text, int index) {
        int len = text.length() / numThreads;
        int start = index * len;
        int end = (index == numThreads - 1) ? text.length() : start + len;
        return text.substring(start, end);
    }
    
    /**
     * Properly shutdown the executor service
     * Call this when you're completely done with the Solution instance
     */
    public void shutdown() {
        executor.shutdown();
        try {
            if (!executor.awaitTermination(5, TimeUnit.SECONDS)) {
                executor.shutdownNow();
            }
        } catch (InterruptedException e) {
            executor.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }
}

public class VerifyLongTextPartB {
    public static void main(String[] args) throws InterruptedException {
        Solution sol = new Solution();
        
        try {
            // Test cases
            String order = "abcd";
            System.out.println("Test 1: " + sol.isOrdered("axubxcxd", order));        // true
            System.out.println("Test 2: " + sol.isOrdered("axubbxcxbxd", order));     // false
            System.out.println("Test 3: " + sol.isOrdered("aaabbbbccccdddd", order)); // true
            System.out.println("Test 4: " + sol.isOrdered("abcddcba", order));        // false
        } finally {
            // Always shutdown the executor when done
            sol.shutdown();
        }
    }
}

/*
Concise Multi-threaded Order Verification Solution
=================================================

This implementation provides a complete multi-threaded solution for the order 
verification problem that can be written in a 30-minute interview.

Key Features:
- Multi-threading with ExecutorService
- Optimization: Start from first unfinished result  
- Early failure detection
- Proper resource management with shutdown()
- Comprehensive documentation

Algorithm:
1. Split text into numThreads chunks
2. Each thread processes its chunk to find min/max order positions
3. Main thread validates that chunk results maintain overall order
4. Early termination on any failure or order violation

Thread Safety:
- volatile fields in Result class for lock-free communication
- ExecutorService manages thread lifecycle
- Proper shutdown handling with timeout

Time Complexity: O(n/k) where n=text length, k=number of threads
Space Complexity: O(k) for results array

This solution demonstrates understanding of:
- Core algorithm and optimizations
- Modern Java concurrency (ExecutorService)
- Thread safety and resource management
- Clean, documented code under time pressure
*/