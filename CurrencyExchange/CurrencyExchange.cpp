/*
PROBLEM DESCRIPTION:
===================

We are a currency exchange that maintains current exchange rates between currencies. 
A user can come to us with some amount in one currency and request the equivalent 
amount in a different currency. Given a list of exchange rates between currencies, 
write functions that:

1. Calculate currency rate between any 2 currencies (using DFS)
2. Calculate OPTIMAL currency rate between any 2 currencies (using Dijkstra)

Example Exchange Rates:
- (GBP, EUR, 10) means "1 GBP equals 10 EUR"
- (EUR, USD, 1.1) means "1 EUR equals 1.1 USD"

For optimal path finding:
- (GBP, EUR, 10) and (EUR, JPY, 900) and (EUR, USD, 1.1) and (USD, JPY, 108.3)
- Query: (10 GBP, JPY) => ?
- Path 1: GBP → EUR → USD → JPY = 10 × 10 × 1.1 × 108.3 = 11,913 JPY
- Path 2: GBP → EUR → JPY = 10 × 10 × 900 = 90,000 JPY (OPTIMAL)

The optimal solution should return the path that maximizes target currency.

APPROACH:
=========
1. Model as weighted directed graph where currencies are nodes and rates are edges
2. DFS: Find any valid conversion path (may not be optimal)
3. Dijkstra: Find optimal conversion path (maximizes target currency)

KEY INSIGHT: We modify Dijkstra to find MAXIMUM paths instead of minimum paths
by using a max-heap and tracking maximum rates instead of minimum distances.
*/

#include <bits/stdc++.h>

using namespace std;

class CurrencyExchange {
private:
    // Adjacency list: currency -> [(neighbor_currency, exchange_rate)]
    // Space Complexity: O(V + E) where V = currencies, E = exchange pairs
    unordered_map<string, vector<pair<string, double>>> adj;

    /*
    DFS Algorithm to find any valid conversion path
    Time Complexity: O(V + E) in worst case (visits all nodes and edges)
    Space Complexity: O(V) for recursion stack and visited set
    
    @param currentCurrency: Current currency in the path
    @param destinationCurrency: Target currency to reach
    @param visited: Set to track visited currencies (avoid cycles)
    @param currentRate: Accumulated conversion rate so far
    @return: Conversion rate if path found, -1 if no path exists
    */
    double dfs(string currentCurrency, string destinationCurrency, set<string> &visited, double currentRate) {
        // Base case: reached destination
        if (currentCurrency == destinationCurrency) {
            return currentRate;
        }
        
        // Mark current currency as visited to avoid cycles
        visited.insert(currentCurrency);

        // Explore all adjacent currencies
        for (auto &edge : adj[currentCurrency]) {
            string nextCurrency = edge.first;
            double rate = edge.second;

            // Only visit unvisited currencies
            if (visited.find(nextCurrency) == visited.end()) {
                // Recursively explore this path
                double result = dfs(nextCurrency, destinationCurrency, visited, currentRate * rate);
                if (result != -1) {
                    return result; // Found a valid path, return immediately
                }
            }
        }

        // Backtrack: remove current currency from visited set
        visited.erase(currentCurrency);
        return -1; // No path found from this currency
    }

    /*
    Dijkstra's Algorithm modified to find MAXIMUM conversion rate path
    Time Complexity: O((V + E) log V) where V = currencies, E = exchange pairs
    Space Complexity: O(V) for priority queue and maxRates map
    
    Key Modification: Uses max-heap instead of min-heap to find maximum rates
    
    @param sourceCurrency: Starting currency
    @param destinationCurrency: Target currency
    @return: Maximum conversion rate if path exists, -1 otherwise
    */
    double dijkstraMaxRate(string sourceCurrency, string destinationCurrency) {
        // Max-heap: stores (conversion_rate, currency)
        // C++ priority_queue is max-heap by default for pairs
        priority_queue<pair<double, string>> maxHeap;
        
        // Track the maximum conversion rate found to reach each currency
        unordered_map<string, double> maxRates;
        
        // Initialize: source currency has rate 1.0 to itself
        maxHeap.push({1.0, sourceCurrency});
        maxRates[sourceCurrency] = 1.0;
        
        while (!maxHeap.empty()) {
            // Always process currency with highest conversion rate first
            auto current = maxHeap.top();
            maxHeap.pop();
            
            double currentRate = current.first;
            string currentCurrency = current.second;
            
            // If we reached destination, this is the optimal rate
            // (guaranteed by max-heap property)
            if (currentCurrency == destinationCurrency) {
                return currentRate;
            }
            
            // Skip if we've already found a better rate to this currency
            // This handles duplicate entries in the priority queue
            if (maxRates.find(currentCurrency) != maxRates.end() && 
                    maxRates[currentCurrency] > currentRate) {
                continue;
            }
            
            // Explore all adjacent currencies
            for (auto &edge : adj[currentCurrency]) {
                string nextCurrency = edge.first;
                double rate = edge.second;
                double newRate = currentRate * rate; // Calculate new conversion rate
                
                // Update if we found a better rate to nextCurrency
                if (maxRates.find(nextCurrency) == maxRates.end() || newRate > maxRates[nextCurrency]) {
                    maxRates[nextCurrency] = newRate;
                    maxHeap.push({newRate, nextCurrency});
                }
            }
        }
        
        return -1; // No path found
    }

public:
    CurrencyExchange() {}

    /*
    Add bidirectional exchange rate between two currencies
    Time Complexity: O(1)
    Space Complexity: O(1)
    
    @param firstCurrency: First currency
    @param secondCurrency: Second currency  
    @param rate: Exchange rate from first to second currency
    */
    void addCurrencyExchangeRate(string firstCurrency, string secondCurrency, double rate) {
        // Add edge: firstCurrency -> secondCurrency with given rate
        adj[firstCurrency].push_back({secondCurrency, rate});
        
        // Add reverse edge: secondCurrency -> firstCurrency with inverse rate
        adj[secondCurrency].push_back({firstCurrency, 1.0 / rate});
    }

    /*
    Calculate exchange rate using DFS (finds any valid path)
    Time Complexity: O(V + E)
    Space Complexity: O(V)
    
    @param count: Amount of source currency
    @param sourceCurrency: Source currency
    @param destinationCurrency: Target currency
    @return: Equivalent amount in target currency, -1 if no path
    */
    double calculateExhangeRate(int count, string sourceCurrency, string destinationCurrency) {
        // Same currency conversion
        if(sourceCurrency == destinationCurrency) {
            return count;
        }
        
        set<string> visited; // Track visited currencies
        double rate = dfs(sourceCurrency, destinationCurrency, visited, 1.0);
        return rate == -1 ? -1 : count * rate;
    }

    /*
    Calculate OPTIMAL exchange rate using Dijkstra (finds maximum rate path)
    Time Complexity: O((V + E) log V)
    Space Complexity: O(V)
    
    @param count: Amount of source currency
    @param sourceCurrency: Source currency
    @param destinationCurrency: Target currency
    @return: Maximum equivalent amount in target currency, -1 if no path
    */
    double calculateOptimalExchangerate(int count, string sourceCurrency, string destinationCurrency) {
        // Same currency conversion
        if(sourceCurrency == destinationCurrency) {
            return count;
        }
        
        double rate = dijkstraMaxRate(sourceCurrency, destinationCurrency);
        return rate == -1 ? -1 : count * rate;
    }
    
    /*
    Clear all exchange rates
    Time Complexity: O(1)
    Space Complexity: O(1)
    */
    void clearRates() {
        adj.clear();
    }
};

int main() {
    CurrencyExchange currencyExchange;
    
    // Test Case 1: Example 1 - (10 GBP, USD) => 110
    currencyExchange.addCurrencyExchangeRate("GBP", "EUR", 10);
    currencyExchange.addCurrencyExchangeRate("EUR", "USD", 1.1);
    double result1 = currencyExchange.calculateExhangeRate(10, "GBP", "USD");
    cout << "Test 1 - 10 GBP to USD: " << result1 << " (expected: 110)" << endl;
    assert(result1 == 110.0);
    
    // Test Case 2: Example 2 - (10 GBP, JPY) => 11913
    currencyExchange.addCurrencyExchangeRate("USD", "JPY", 108.3);
    double result2 = currencyExchange.calculateExhangeRate(10, "GBP", "JPY");
    cout << "Test 2 - 10 GBP to JPY: " << result2 << " (expected: 11913)" << endl;
    assert(result2 == 11913.0);
    
    currencyExchange.clearRates();
    
    // Test Case 3: (1 EUR, INR) => 78.91
    currencyExchange.addCurrencyExchangeRate("GBP", "CAD", 1.671);
    currencyExchange.addCurrencyExchangeRate("INR", "CAD", 0.0185);
    currencyExchange.addCurrencyExchangeRate("CAD", "EUR", 0.685);
    double result3 = currencyExchange.calculateExhangeRate(1, "EUR", "INR");
    cout << "Test 3 - 1 EUR to INR: " << result3 << " (expected: 78.91)" << endl;
    assert(abs(result3 - 78.91) < 0.1);
    
    currencyExchange.clearRates();
    
    // Test Case 4: (10 EUR, JPY) => 1155.0
    currencyExchange.addCurrencyExchangeRate("EUR", "USD", 1.1);
    currencyExchange.addCurrencyExchangeRate("USD", "INR", 70.0);
    currencyExchange.addCurrencyExchangeRate("INR", "JPY", 1.5);
    double result4 = currencyExchange.calculateExhangeRate(10, "EUR", "JPY");
    cout << "Test 4 - 10 EUR to JPY: " << result4 << " (expected: 1155)" << endl;
    assert(result4 == 1155.0);
    
    currencyExchange.clearRates();
    
    // Test Case 5: (10 EUR, JPY) => 1178.57 (with INR->USD conversion)
    currencyExchange.addCurrencyExchangeRate("EUR", "USD", 1.1);
    currencyExchange.addCurrencyExchangeRate("INR", "USD", 0.014);
    currencyExchange.addCurrencyExchangeRate("INR", "JPY", 1.5);
    double result5 = currencyExchange.calculateExhangeRate(10, "EUR", "JPY");
    cout << "Test 5 - 10 EUR to JPY: " << result5 << " (expected: 1178.57)" << endl;
    assert(abs(result5 - 1178.57) < 1.0);
    
    currencyExchange.clearRates();
    
    // Test Case 6: Multiple paths (10 EUR, JPY) => 1155.0
    currencyExchange.addCurrencyExchangeRate("EUR", "USD", 1.1);
    currencyExchange.addCurrencyExchangeRate("USD", "INR", 70.0);
    currencyExchange.addCurrencyExchangeRate("INR", "JPY", 1.5);
    currencyExchange.addCurrencyExchangeRate("EUR", "GBP", 0.9);
    currencyExchange.addCurrencyExchangeRate("GBP", "MP", 27.3);
    currencyExchange.addCurrencyExchangeRate("MP", "JPY", 4.7);
    double result6 = currencyExchange.calculateExhangeRate(10, "EUR", "JPY");
    cout << "Test 6 - 10 EUR to JPY (multiple paths): " << result6 << " (expected: 1155)" << endl;
    assert(abs(result6 - 1155.0) < 1.0);
    
    currencyExchange.clearRates();
    
    // Test Case 7: No conversion exists (1 EUR, INR) => -1
    currencyExchange.addCurrencyExchangeRate("EUR", "USD", 1.1);
    currencyExchange.addCurrencyExchangeRate("CAD", "INR", 53.9);
    double result7 = currencyExchange.calculateExhangeRate(1, "EUR", "INR");
    cout << "Test 7 - 1 EUR to INR (no path): " << result7 << " (expected: -1)" << endl;
    assert(result7 == -1);
    
    currencyExchange.clearRates();
    
    // Test Case 8: Multiple paths with DFS (10 EUR, JPY) => 1155.0
    currencyExchange.addCurrencyExchangeRate("EUR", "GBP", 0.91);
    currencyExchange.addCurrencyExchangeRate("GBP", "CHF", 1.18);
    currencyExchange.addCurrencyExchangeRate("USD", "INR", 70.0);
    currencyExchange.addCurrencyExchangeRate("EUR", "USD", 1.1);
    currencyExchange.addCurrencyExchangeRate("INR", "JPY", 1.5);
    double result8 = currencyExchange.calculateExhangeRate(10, "EUR", "JPY");
    cout << "Test 8 - 10 EUR to JPY (DFS): " << result8 << " (expected: 1155)" << endl;
    assert(result8 == 1155.0);
    
    cout << "All test cases passed!" << endl;
    
    // Test optimal path finding
    cout << "\n=== Testing Optimal Exchange Rate ===" << endl;
    
    currencyExchange.clearRates();
    currencyExchange.addCurrencyExchangeRate("GBP", "EUR", 10);
    currencyExchange.addCurrencyExchangeRate("EUR", "USD", 1.1);
    currencyExchange.addCurrencyExchangeRate("EUR", "JPY", 900);
    currencyExchange.addCurrencyExchangeRate("USD", "JPY", 108.3);
    
    double regularResult = currencyExchange.calculateExhangeRate(10, "GBP", "JPY");
    double optimalResult = currencyExchange.calculateOptimalExchangerate(10, "GBP", "JPY");
    
    cout << "Regular path: " << regularResult << " JPY" << endl;
    cout << "Optimal path: " << optimalResult << " JPY" << endl;
    cout << "Improvement: " << (optimalResult - regularResult) << " JPY" << endl;
    
    assert(optimalResult == 90000.0);
    cout << "Optimal exchange rate test passed!" << endl;
    
    return 0;
}

/*
COMPLEXITY ANALYSIS:
===================

Overall Time Complexity:
- DFS approach: O(V + E) where V = number of currencies, E = number of exchange pairs
- Dijkstra approach: O((V + E) log V)

Overall Space Complexity: 
- Graph storage: O(V + E)
- DFS: O(V) for recursion stack and visited set
- Dijkstra: O(V) for priority queue and maxRates map

TRADE-OFFS:
===========
DFS: Faster, but may not find optimal path
Dijkstra: Slower, but guarantees optimal (maximum rate) path

For production systems with frequent queries, consider:
1. Caching optimal rates between major currency pairs
2. Using hub currencies (USD, EUR) to reduce graph complexity
3. Periodic recalculation vs real-time optimization based on use case
*/