#include <iostream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <stdexcept>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>

/**
 * RevenueCalculatorPartA - Original problem: 1-level referral revenue tracking
 * 
 * Key Features:
 * - Auto-incrementing customer IDs starting from 0
 * - Tracks direct revenue per customer
 * - Updates referrer's total revenue when they refer new customers (1 level only)
 * - Efficient queries for lowest K customers by minimum revenue threshold
 */
class RevenueCalculatorPartA {
private:
    int currentCustomerId;
    
    // Maps customer ID to their total revenue (direct + from direct referrals)
    std::unordered_map<int, double> customerRevenueMap;
    
    // Maps revenue to set of customers with that revenue (sorted by revenue)
    std::map<double, std::unordered_set<int>> customerRevenuesSorted;

    /**
     * Adds a customer to the sorted revenue structure
     * Time Complexity: O(log n)
     */
    void addCustomerToSortedRevenue(int customerId, double revenue) {
        customerRevenuesSorted[revenue].insert(customerId);
    }

    /**
     * Removes a customer from the sorted revenue structure
     * Time Complexity: O(log n)
     */
    void removeCustomerFromSortedRevenue(int customerId, double revenue) {
        auto it = customerRevenuesSorted.find(revenue);
        if (it != customerRevenuesSorted.end()) {
            it->second.erase(customerId);
            if (it->second.empty()) {
                customerRevenuesSorted.erase(it);
            }
        }
    }

    /**
     * Updates a customer's total revenue (used when they get referral revenue)
     * Time Complexity: O(log n)
     */
    void increaseCustomerRevenue(int customerId, double additionalRevenue) {
        double currentRevenue = customerRevenueMap[customerId];
        double newRevenue = currentRevenue + additionalRevenue;
        
        customerRevenueMap[customerId] = newRevenue;
        removeCustomerFromSortedRevenue(customerId, currentRevenue);
        addCustomerToSortedRevenue(customerId, newRevenue);
    }

public:
    /**
     * Constructor
     * Time Complexity: O(1)
     */
    RevenueCalculatorPartA() : currentCustomerId(-1) {}

    /**
     * Inserts a new customer without referrer
     * Time Complexity: O(log n)
     */
    int insertNewCustomer(double revenue) {
        currentCustomerId++;
        customerRevenueMap[currentCustomerId] = revenue;
        addCustomerToSortedRevenue(currentCustomerId, revenue);
        return currentCustomerId;
    }

    /**
     * Inserts a new customer with a referrer
     * Time Complexity: O(log n)
     */
    int insertNewCustomer(double revenue, int referrerId) {
        if (customerRevenueMap.find(referrerId) == customerRevenueMap.end()) {
            throw std::invalid_argument("Referrer ID " + std::to_string(referrerId) + " not found");
        }
        
        int newCustomerId = insertNewCustomer(revenue);
        increaseCustomerRevenue(referrerId, revenue);
        return newCustomerId;
    }

    /**
     * Gets the K customers with lowest total revenue that meet minimum threshold
     * Time Complexity: O(log n + k)
     */
    std::unordered_set<int> getLowestKCustomersByMinTotalRevenue(int k, double minTotalRevenue) {
        std::unordered_set<int> result;
        
        if (k == 0) {
            return result;
        }
        
        auto it = customerRevenuesSorted.lower_bound(minTotalRevenue);
        
        for (; it != customerRevenuesSorted.end(); ++it) {
            const auto& customers = it->second;
            
            for (int customerId : customers) {
                result.insert(customerId);
                if (result.size() == k) {
                    return result;
                }
            }
        }
        
        return result;
    }
};

/**
 * RevenueCalculatorPartB - Follow-up: Multi-level referral revenue tracking using BFS
 * 
 * Key Features:
 * - Support for calculating revenue up to X levels deep
 * - Graph-based referral tracking
 * - BFS traversal with level control
 * - Cycle detection for robust graph traversal
 */
class RevenueCalculatorPartB {
private:
    int currentCustomerId;
    
    // Maps customer ID to their direct revenue only
    std::unordered_map<int, double> customerDirectRevenue;
    
    // Graph structure: customer ID -> set of direct referees
    std::unordered_map<int, std::unordered_set<int>> referralGraph;

public:
    /**
     * Constructor
     * Time Complexity: O(1)
     */
    RevenueCalculatorPartB() : currentCustomerId(-1) {}

    /**
     * Inserts a new customer without referrer
     * Time Complexity: O(1)
     */
    int insertNewCustomer(double revenue) {
        currentCustomerId++;
        customerDirectRevenue[currentCustomerId] = revenue;
        referralGraph[currentCustomerId] = std::unordered_set<int>();
        return currentCustomerId;
    }

    /**
     * Inserts a new customer with a referrer
     * Time Complexity: O(1)
     */
    int insertNewCustomer(double revenue, int referrerId) {
        if (customerDirectRevenue.find(referrerId) == customerDirectRevenue.end()) {
            throw std::invalid_argument("Referrer ID " + std::to_string(referrerId) + " not found");
        }
        
        int newCustomerId = insertNewCustomer(revenue);
        referralGraph[referrerId].insert(newCustomerId);
        return newCustomerId;
    }

    /**
     * Calculates total revenue for a customer up to X levels deep using BFS
     * Time Complexity: O(V + E) where V = vertices within X levels, E = edges within X levels
     * Space Complexity: O(V) for queue and visited set
     * 
     * @param customerId Customer whose revenue to calculate
     * @param maxLevels Maximum depth of referral levels to include (0 = direct revenue only)
     * @return Total revenue including referrals up to maxLevels deep
     */
    double calculateRevenueUpToXLevels(int customerId, int maxLevels) {
        if (customerDirectRevenue.find(customerId) == customerDirectRevenue.end()) {
            throw std::invalid_argument("Customer ID " + std::to_string(customerId) + " not found");
        }
        
        double totalRevenue = customerDirectRevenue[customerId];
        
        if (maxLevels == 0) {
            return totalRevenue;
        }
        
        // BFS with level tracking
        std::queue<std::pair<int, int>> queue; // (customerId, currentLevel)
        std::unordered_set<int> visited;
        
        queue.push({customerId, 0});
        visited.insert(customerId);
        
        while (!queue.empty()) {
            auto [currentId, currentLevel] = queue.front();
            queue.pop();
            
            // Stop if we've reached the maximum level
            if (currentLevel >= maxLevels) {
                continue;
            }
            
            // Process all direct referees
            for (int refereeId : referralGraph[currentId]) {
                if (visited.find(refereeId) == visited.end()) {
                    totalRevenue += customerDirectRevenue[refereeId];
                    visited.insert(refereeId);
                    queue.push({refereeId, currentLevel + 1});
                }
            }
        }
        
        return totalRevenue;
    }

    /**
     * Gets the K customers with lowest total revenue (X levels) that meet minimum threshold
     * Time Complexity: O(n * (V + E) + n log n) where n = total customers
     */
    std::unordered_set<int> getLowestKCustomersByMinTotalRevenue(int k, double minTotalRevenue, int maxLevels) {
        std::vector<std::pair<double, int>> customerRevenues;
        
        // Calculate total revenue for each customer up to X levels
        for (const auto& entry : customerDirectRevenue) {
            int customerId = entry.first;
            double totalRevenue = calculateRevenueUpToXLevels(customerId, maxLevels);
            
            if (totalRevenue >= minTotalRevenue) {
                customerRevenues.push_back({totalRevenue, customerId});
            }
        }
        
        // Sort by revenue (ascending)
        std::sort(customerRevenues.begin(), customerRevenues.end());
        
        // Take first k customers
        std::unordered_set<int> result;
        for (int i = 0; i < std::min(k, (int)customerRevenues.size()); i++) {
            result.insert(customerRevenues[i].second);
        }
        
        return result;
    }
};

int main() {
    // Test Part A - Original 1-level referral problem
    {
        std::cout << "=== Part A: 1-Level Referral Revenue ===" << std::endl;
        RevenueCalculatorPartA calculator;
        
        int customer0 = calculator.insertNewCustomer(10);
        int customer1 = calculator.insertNewCustomer(30, customer0);
        int customer2 = calculator.insertNewCustomer(50, customer1);
        
        auto result1 = calculator.getLowestKCustomersByMinTotalRevenue(1, 45);
        std::cout << "Query(1, 45): {";
        bool first = true;
        for (int id : result1) {
            if (!first) std::cout << ", ";
            std::cout << id;
            first = false;
        }
        std::cout << "}" << std::endl;
        
        auto result2 = calculator.getLowestKCustomersByMinTotalRevenue(2, 45);
        std::cout << "Query(2, 45): {";
        first = true;
        for (int id : result2) {
            if (!first) std::cout << ", ";
            std::cout << id;
            first = false;
        }
        std::cout << "}" << std::endl;
    }
    
    // Test Part B - Multi-level referral problem
    {
        std::cout << "\n=== Part B: Multi-Level Referral Revenue ===" << std::endl;
        RevenueCalculatorPartB calculator;
        
        // Create referral chain: A -> B -> D, A -> C -> F
        int customerA = calculator.insertNewCustomer(100);
        int customerB = calculator.insertNewCustomer(50, customerA);
        int customerC = calculator.insertNewCustomer(40, customerA);
        int customerD = calculator.insertNewCustomer(20, customerB);
        int customerF = calculator.insertNewCustomer(15, customerC);
        
        // Test different level calculations for customer A
        std::cout << "Customer A revenue (Level 0): " << calculator.calculateRevenueUpToXLevels(customerA, 0) << std::endl;
        std::cout << "Customer A revenue (Level 1): " << calculator.calculateRevenueUpToXLevels(customerA, 1) << std::endl;
        std::cout << "Customer A revenue (Level 2): " << calculator.calculateRevenueUpToXLevels(customerA, 2) << std::endl;
        
        // Test query with different levels
        auto result3 = calculator.getLowestKCustomersByMinTotalRevenue(2, 150, 1);
        std::cout << "Query(2, 150, Level 1): {";
        bool first = true;
        for (int id : result3) {
            if (!first) std::cout << ", ";
            std::cout << id;
            first = false;
        }
        std::cout << "}" << std::endl;
    }
    
    return 0;
}

/*
PROBLEM STATEMENT:
==================

Part A - Original Problem:
We own a company that acquires customers. Each customer brings a certain revenue to the company 
and each customer can also refer more customers which in return add additional revenue. 
We want to implement an algorithm that calculates the revenue of the company.

There are 3 methods to implement:
1. int insertNewCustomer(double revenue) 
2. int insertNewCustomer(double revenue, int referrerID) 
3. Set<Integer> getLowestKCustomersByMinTotalRevenue(int k, double minTotalRevenue) 

The total revenue consists of the revenue that this customer brings initially 
as well as the revenue of the directly referred customers (1 level only).

Part B - Follow-up Problem:
What if you want to calculate the revenue up to X levels?
- This becomes a graph problem
- Use BFS traversal and stop at X levels
- Graph: Map<Integer, Set<Integer>> representing customer -> direct referees
- Revenue calculation includes all referrals up to X levels deep

COMPLEXITY ANALYSIS:
===================

Part A - Time Complexities:
• insertNewCustomer(revenue): O(log n)
• insertNewCustomer(revenue, referrerId): O(log n)
• getLowestKCustomersByMinTotalRevenue(k, minRevenue): O(log n + k)

Part B - Time Complexities:
• insertNewCustomer(revenue): O(1)
• insertNewCustomer(revenue, referrerId): O(1)
• calculateRevenueUpToXLevels(customerId, X): O(V + E) where V = vertices within X levels
• getLowestKCustomersByMinTotalRevenue(k, minRevenue, X): O(n * (V + E) + n log n)

Space Complexities:
• Part A: O(n) for storing customer data and sorted structure
• Part B: O(n + E) where E = total referral edges in graph

Key Optimizations:
• Part A: Binary search using lower_bound() for efficient range queries
• Part B: BFS with level control and cycle detection for robust graph traversal
*/