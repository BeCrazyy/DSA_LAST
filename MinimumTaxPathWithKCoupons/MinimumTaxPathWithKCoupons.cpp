#include <iostream>
#include <vector>
#include <climits>
using namespace std;

int solve(int node, int coupons_left, int dest, vector<vector<pair<int,int>>>& graph, 
          vector<vector<int>>& memo, vector<vector<bool>>& visiting) {
    // Base case: reached destination
    if (node == dest) return 0;
    
    // Check memoization
    if (memo[node][coupons_left] != -1) {
        return memo[node][coupons_left];
    }
    
    // Avoid infinite recursion in cycles
    if (visiting[node][coupons_left]) {
        return INT_MAX;
    }
    
    visiting[node][coupons_left] = true;
    int min_cost = INT_MAX;
    
    // Try all adjacent nodes
    for (auto& edge : graph[node]) {
        int next_node = edge.first;
        int tax = edge.second;
        
        // Option 1: Pay the tax
        int cost_with_payment = solve(next_node, coupons_left, dest, graph, memo, visiting);
        if (cost_with_payment != INT_MAX) {
            min_cost = min(min_cost, cost_with_payment + tax);
        }
        
        // Option 2: Use a coupon (if available)
        if (coupons_left > 0) {
            int cost_with_coupon = solve(next_node, coupons_left - 1, dest, graph, memo, visiting);
            if (cost_with_coupon != INT_MAX) {
                min_cost = min(min_cost, cost_with_coupon);
            }
        }
    }
    
    visiting[node][coupons_left] = false; // Backtrack
    return memo[node][coupons_left] = min_cost;
}

int minTax(int n, vector<vector<pair<int,int>>>& graph, int source, int dest, int k) {
    vector<vector<int>> memo(n, vector<int>(k + 1, -1));
    vector<vector<bool>> visiting(n, vector<bool>(k + 1, false));
    
    int result = solve(source, k, dest, graph, memo, visiting);
    return result == INT_MAX ? -1 : result;
}

void printTestCase(int testNum, int n, int source, int dest, int k, int result, int expected) {
    cout << "Test " << testNum << ": ";
    cout << "N=" << n << " S=" << source+1 << " D=" << dest+1 << " K=" << k;
    cout << " | Result=" << result << " Expected=" << expected;
    cout << (result == expected ? " ✓" : " ✗") << endl;
}

int main() {
    // Test 1: Basic example - use coupon optimally
    {
        int n = 3;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 50}); graph[1].push_back({0, 50});
        graph[1].push_back({2, 3});  graph[2].push_back({1, 3});
        
        int result = minTax(n, graph, 0, 2, 1);
        printTestCase(1, n, 0, 2, 1, result, 3);
    }
    
    // Test 2: Use multiple coupons
    {
        int n = 3;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 5}); graph[1].push_back({0, 5});
        graph[1].push_back({2, 3}); graph[2].push_back({1, 3});
        
        int result = minTax(n, graph, 0, 2, 2);
        printTestCase(2, n, 0, 2, 2, result, 0);
    }
    
    // Test 3: Choose best path with coupon
    {
        int n = 3;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 10}); graph[1].push_back({0, 10});
        graph[1].push_back({2, 5});  graph[2].push_back({1, 5});
        graph[0].push_back({2, 20}); graph[2].push_back({0, 20}); // Direct path
        
        int result = minTax(n, graph, 0, 2, 1);
        printTestCase(3, n, 0, 2, 1, result, 0);
    }
    
    // Test 4: No path exists
    {
        int n = 4;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 10}); graph[1].push_back({0, 10});
        graph[2].push_back({3, 10}); graph[3].push_back({2, 10});
        
        int result = minTax(n, graph, 0, 3, 5);
        printTestCase(4, n, 0, 3, 5, result, -1);
    }
    
    // Test 5: Already at destination
    {
        int n = 3;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 10}); graph[1].push_back({0, 10});
        
        int result = minTax(n, graph, 1, 1, 2);
        printTestCase(5, n, 1, 1, 2, result, 0);
    }
    
    // Test 6: Single edge without coupon
    {
        int n = 2;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 100}); graph[1].push_back({0, 100});
        
        int result = minTax(n, graph, 0, 1, 0);
        printTestCase(6, n, 0, 1, 0, result, 100);
    }
    
    // Test 7: Single edge with coupon
    {
        int n = 2;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 100}); graph[1].push_back({0, 100});
        
        int result = minTax(n, graph, 0, 1, 1);
        printTestCase(7, n, 0, 1, 1, result, 0);
    }
    
    // Test 8: Complex path selection
    {
        int n = 4;
        vector<vector<pair<int,int>>> graph(n);
        graph[0].push_back({1, 100}); graph[1].push_back({0, 100});
        graph[1].push_back({2, 200}); graph[2].push_back({1, 200});
        graph[2].push_back({3, 50});  graph[3].push_back({2, 50});
        
        int result = minTax(n, graph, 0, 3, 2);
        printTestCase(8, n, 0, 3, 2, result, 50);
    }
    
    return 0;
}