import java.util.*;
import java.time.Instant;

/**
 * Expiring Counter - maintains counts of elements that expire after a time window
 */
class ExpiringCounter {
    private Deque<Pair<Instant, String>> operations;  // Queue of (timestamp, element)
    private Map<String, Integer> counts;               // Current count per element
    private int windowSeconds;                         // Expiration window
    
    /**
     * Helper class to store timestamp-element pairs
     */
    private static class Pair<T, U> {
        public final T first;
        public final U second;
        
        public Pair(T first, U second) {
            this.first = first;
            this.second = second;
        }
    }
    
    /**
     * Get current timestamp
     */
    private Instant currentTime() {
        return Instant.now();
    }
    
    /**
     * Remove expired entries from front of queue
     * Updates counts map accordingly
     */
    private void cleanup() {
        Instant now = currentTime();
        
        while (!operations.isEmpty()) {
            Pair<Instant, String> front = operations.peekFirst();
            Instant timestamp = front.first;
            String element = front.second;
            
            if (timestamp.plusSeconds(windowSeconds).isAfter(now)) {
                break;  // Not expired yet
            }
            
            operations.pollFirst();
            counts.put(element, counts.get(element) - 1);
            
            if (counts.get(element) == 0) {
                counts.remove(element);
            }
        }
    }
    
    /**
     * Constructor
     * @param seconds - expiration time window in seconds
     */
    public ExpiringCounter(int seconds) {
        this.windowSeconds = seconds;
        this.operations = new ArrayDeque<>();
        this.counts = new HashMap<>();
    }
    
    /**
     * Add an element to the counter
     * @param element - element to add
     */
    public void put(String element) {
        cleanup();
        operations.addLast(new Pair<>(currentTime(), element));
        counts.put(element, counts.getOrDefault(element, 0) + 1);
    }
    
    /**
     * Get count of a specific element within time window
     * @param element - element to count
     * @return count of element (0 if not found)
     */
    public int getCount(String element) {
        cleanup();
        return counts.getOrDefault(element, 0);
    }
    
    /**
     * Get total count of all elements within time window
     * @return total number of active elements
     */
    public int getTotalCount() {
        cleanup();
        return operations.size();
    }
}

public class TimeLimitedCounter {
    // Test the implementation
    public static void main(String[] args) {
        ExpiringCounter counter = new ExpiringCounter(300);  // 5 minute window
        
        counter.put("a");
        counter.put("a");
        counter.put("b");
        
        System.out.println("Count of 'a': " + counter.getCount("a"));        // 2
        System.out.println("Total count: " + counter.getTotalCount());       // 3
        System.out.println("Count of 'x': " + counter.getCount("x"));        // 0
    }
}

/*
PROBLEM:
Write a Counter class that holds expiring counts for elements.
- Constructor takes expiration time window (seconds)
- put(element): add element 
- get_count(element): return count for element
- get_total_count(): return total count

Elements expire after time window passes.

APPROACH:
- Store (timestamp, element) pairs in deque
- Keep element counts in hash map
- On each operation, remove expired entries from front
- Time: O(expired_items) per call, amortized O(1)
- Space: O(active_elements)
*/