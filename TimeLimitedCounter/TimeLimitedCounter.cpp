#include <iostream>
#include <unordered_map>
#include <deque>
#include <chrono>
#include <string>

/**
 * Expiring Counter - maintains counts of elements that expire after a time window
 */
class ExpiringCounter {
private:
    std::deque<std::pair<long long, std::string>> operations;  // Queue of (timestamp, element)
    std::unordered_map<std::string, int> counts;              // Current count per element
    int window_seconds;                                       // Expiration window
    
    /**
     * Get current timestamp in seconds since epoch
     */
    long long current_time() {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    /**
     * Remove expired entries from front of queue
     * Updates counts map accordingly
     */
    void cleanup() {
        long long now = current_time();
        
        while (!operations.empty()) {
            auto [timestamp, element] = operations.front();
            
            if (now - timestamp <= window_seconds) {
                break;  // Not expired yet
            }
            
            operations.pop_front();
            counts[element]--;
            
            if (counts[element] == 0) {
                counts.erase(element);
            }
        }
    }
    
public:
    /**
     * Constructor
     * @param seconds - expiration time window in seconds
     */
    ExpiringCounter(int seconds) : window_seconds(seconds) {}
    
    /**
     * Add an element to the counter
     * @param element - element to add
     */
    void put(const std::string& element) {
        cleanup();
        operations.push_back({current_time(), element});
        counts[element]++;
    }
    
    /**
     * Get count of a specific element within time window
     * @param element - element to count
     * @return count of element (0 if not found)
     */
    int get_count(const std::string& element) {
        cleanup();
        return counts.count(element) ? counts[element] : 0;
    }
    
    /**
     * Get total count of all elements within time window
     * @return total number of active elements
     */
    int get_total_count() {
        cleanup();
        return operations.size();
    }
};

// Test the implementation
int main() {
    ExpiringCounter counter(300);  // 5 minute window
    
    counter.put("a");
    counter.put("a");
    counter.put("b");
    
    std::cout << "Count of 'a': " << counter.get_count("a") << std::endl;        // 2
    std::cout << "Total count: " << counter.get_total_count() << std::endl;      // 3
    std::cout << "Count of 'x': " << counter.get_count("x") << std::endl;        // 0
    
    return 0;
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