#include <bits/stdc++.h>
using namespace std;

/**
 * @brief Find next timestamp value for backfilling
 * 
 * @param series The time series data (sorted by timestamp)
 * @param timestamp Current timestamp to find next value for
 * @return int Value from next available timestamp, or last known value
 * 
 * Time Complexity: O(log N)
 */
int findNextValue(const vector<pair<int, int>>& series, int timestamp) {
    // Binary search for first element > timestamp
    auto it = upper_bound(series.begin(), series.end(), 
                         make_pair(timestamp, INT_MAX));
    
    // If found next timestamp, return its value
    if (it != series.end()) {
        return it->second;
    }
    
    // No future timestamp, use last known value
    return series.empty() ? 0 : series.back().second;
}

/**
 * @brief Aggregate two time series using backfilling and summation
 * 
 * Steps:
 * 1. Collect all unique timestamps from both series
 * 2. For each timestamp, get value (direct or backfilled) from each series
 * 3. Sum the values and add to result
 * 
 * @param first_series First time series data
 * @param second_series Second time series data
 * @return Aggregated time series with all unique timestamps
 * 
 * Time Complexity: O((X + Y) * log(max(X, Y)))
 * Space Complexity: O(X + Y)
 */
vector<pair<int, int>> aggregateTimeSeries(
    const vector<pair<int, int>>& first_series,
    const vector<pair<int, int>>& second_series) {
    
    // Step 1: Collect all unique timestamps
    set<int> all_timestamps;
    for (const auto& point : first_series) {
        all_timestamps.insert(point.first);
    }
    for (const auto& point : second_series) {
        all_timestamps.insert(point.first);
    }
    
    // Step 2: Create lookup maps for direct timestamp access
    map<int, int> first_map, second_map;
    for (const auto& point : first_series) {
        first_map[point.first] = point.second;
    }
    for (const auto& point : second_series) {
        second_map[point.first] = point.second;
    }
    
    // Step 3: Process each timestamp and aggregate values
    vector<pair<int, int>> result;
    
    for (int timestamp : all_timestamps) {
        int value1 = 0, value2 = 0;
        
        // Get value from first series (direct or backfilled)
        if (first_map.find(timestamp) != first_map.end()) {
            value1 = first_map[timestamp];
        } else {
            value1 = findNextValue(first_series, timestamp);
        }
        
        // Get value from second series (direct or backfilled)
        if (second_map.find(timestamp) != second_map.end()) {
            value2 = second_map[timestamp];
        } else {
            value2 = findNextValue(second_series, timestamp);
        }
        
        // Aggregate and store
        result.push_back({timestamp, value1 + value2});
    }
    
    return result;
}

/**
 * @brief Utility function to print time series in readable format
 */
void printTimeSeries(const vector<pair<int, int>>& series, const string& name) {
    cout << name << ": [";
    for (size_t i = 0; i < series.size(); i++) {
        cout << "(" << series[i].first << ", " << series[i].second << ")";
        if (i < series.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

int main() {
    // Test data from problem statement
    vector<pair<int, int>> first_data = {{1, 3}, {3, 1}, {5, 3}, {6, 4}, {10, 1}};
    vector<pair<int, int>> second_data = {{2, 3}, {6, 3}, {11, 2}};
    
    cout << "=== Time Series Aggregation with Binary Search ===" << endl;
    cout << endl;
    
    // Display input
    printTimeSeries(first_data, "First Series ");
    printTimeSeries(second_data, "Second Series");
    cout << endl;
    
    // Aggregate and display result
    vector<pair<int, int>> aggregated = aggregateTimeSeries(first_data, second_data);
    printTimeSeries(aggregated, "Aggregated   ");
    cout << endl;
    
    // Verify expected output
    vector<pair<int, int>> expected = {{1, 6}, {2, 4}, {3, 4}, {5, 6}, {6, 7}, {10, 3}, {11, 3}};
    printTimeSeries(expected, "Expected     ");
    
    // Check if result matches expected
    bool matches = (aggregated == expected);
    cout << endl << "Result matches expected: " << (matches ? "✓ YES" : "✗ NO") << endl;
    
    return 0;
}

/*
===============================================================================
PROBLEM STATEMENT: Time Series Aggregation with Backward Extrapolation
===============================================================================

Given two time series data, write a function to aggregate them into one.

INPUT:
  First Series:  [(1, 3), (3, 1), (5, 3), (6, 4), (10, 1)]
  Second Series: [(2, 3), (6, 3), (11, 2)]

RULE:
  Missing/Undefined values at any timestamp are extrapolated backwards 
  from the next available timestamp.
  
  Example: Value at timestamp 2 in first series = 1 (from timestamp 3)

ALGORITHM:
  1. Collect all unique timestamps from both series
  2. For each timestamp:
     - Get direct value if exists, otherwise backfill from next timestamp
     - Do this for both series
     - Sum the two values
  3. Return aggregated series

EXPECTED OUTPUT:
  [(1, 6), (2, 4), (3, 4), (5, 6), (6, 7), (10, 3), (11, 3)]

COMPLEXITY:
  Time:  O((X + Y) * log(max(X, Y))) where X, Y are input series sizes
  Space: O(X + Y) for storing timestamps and results

OPTIMIZATION:
  Uses binary search for backfilling instead of linear scan
  Improves from O(N) to O(log N) per lookup operation
===============================================================================
*/