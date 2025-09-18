/**
 * ================================================================================
 * Schedule Creator - Complete Solution for Parts 1 & 2
 * ================================================================================
 * 
 * Part 1: Generate ON intervals from schedule settings
 * Part 2: Query if entity is ON/OFF at any given timestamp (Linear + Binary Search)
 * 
 * Author: Assistant
 * Date: 2025
 * ================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

/**
 * @brief Schedule configuration parameters
 */
struct ScheduleSettings {
    std::string start_date;              // "YYYY-MM-DD" format
    std::string end_date;                // "YYYY-MM-DD" format
    int interval_window_size_hours;      // Hours per ON/OFF cycle
};

/**
 * @brief Time interval representation (hour-based)
 */
struct TimeInterval {
    int start_hour;  // Hours from start_date
    int end_hour;    // Hours from start_date
    
    TimeInterval(int start, int end) : start_hour(start), end_hour(end) {}
};

/**
 * ================================================================================
 * PART 1: SCHEDULE GENERATION
 * ================================================================================
 */

/**
 * @brief Calculate days between two dates (simplified same-month)
 * 
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
int calculateDays(const std::string& start_date, const std::string& end_date) {
    int start_day = std::stoi(start_date.substr(8, 2));
    int end_day = std::stoi(end_date.substr(8, 2));
    return end_day - start_day + 1;
}

/**
 * @brief Generate schedule of ON intervals
 * 
 * Creates alternating ON/OFF schedule, returns only ON intervals.
 * Always starts with ON state.
 * 
 * @param settings Schedule configuration
 * @return Vector of ON intervals (chronologically sorted)
 * 
 * Time Complexity: O(N) where N = total_hours / interval_size
 * Space Complexity: O(K) where K = number of ON intervals
 */
std::vector<TimeInterval> createSchedule(const ScheduleSettings& settings) {
    std::vector<TimeInterval> onIntervals;
    
    // Calculate total time span
    int totalDays = calculateDays(settings.start_date, settings.end_date);
    int totalHours = totalDays * 24;
    
    int currentHour = 0;
    bool isOn = true;  // Always start with ON
    
    // Generate intervals
    while (currentHour < totalHours) {
        int intervalEnd = currentHour + settings.interval_window_size_hours;
        
        if (isOn) {
            // This is an ON interval - add it
            int actualEnd = (intervalEnd > totalHours) ? totalHours : intervalEnd;
            onIntervals.push_back(TimeInterval(currentHour, actualEnd));
        }
        
        currentHour = intervalEnd;
        isOn = !isOn;  // Toggle ON/OFF
        
        if (currentHour >= totalHours) break;
    }
    
    return onIntervals;
}

/**
 * ================================================================================
 * PART 2: TIMESTAMP QUERY FUNCTIONS
 * ================================================================================
 */

/**
 * @brief Convert "YYYY-MM-DD HH:MM:SS" to hour offset from start_date
 * 
 * Uses simplified calendar (30 days per month)
 * 
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
int dateTimeToHour(const std::string& datetime, const std::string& start_date) {
    // Parse datetime components
    int dt_year = std::stoi(datetime.substr(0, 4));
    int dt_month = std::stoi(datetime.substr(5, 2));
    int dt_day = std::stoi(datetime.substr(8, 2));
    int dt_hour = std::stoi(datetime.substr(11, 2));
    
    // Parse start_date components
    int start_year = std::stoi(start_date.substr(0, 4));
    int start_month = std::stoi(start_date.substr(5, 2));
    int start_day = std::stoi(start_date.substr(8, 2));
    
    // Calculate differences
    int year_diff = dt_year - start_year;
    int month_diff = dt_month - start_month;
    int day_diff = dt_day - start_day;
    
    // Convert to total day offset (simplified: 30 days per month, 365 days per year)
    int total_day_offset = year_diff * 365 + month_diff * 30 + day_diff;
    
    // Convert to hour offset
    return total_day_offset * 24 + dt_hour;
}

/**
 * @brief LINEAR SEARCH: Check if timestamp is in any ON interval
 * 
 * Iterates through all intervals sequentially.
 * 
 * @param scheduleIntervals List of ON intervals from Part 1
 * @param timestamp_hour Hour offset to check
 * @return True if entity is ON at given timestamp
 * 
 * Time Complexity: O(N) where N = number of ON intervals
 * Space Complexity: O(1)
 * 
 * Algorithm:
 * 1. Loop through each interval
 * 2. Check if timestamp falls in [start, end) range
 * 3. Return True if found, False if not found
 */
bool isTimestampIncluded_Linear(const std::vector<TimeInterval>& scheduleIntervals, int timestamp_hour) {
    for (const auto& interval : scheduleIntervals) {
        if (timestamp_hour >= interval.start_hour && timestamp_hour < interval.end_hour) {
            return true;  // Found - entity is ON
        }
    }
    return false;  // Not found - entity is OFF
}

/**
 * @brief BINARY SEARCH: Check if timestamp is in any ON interval
 * 
 * Uses binary search on sorted intervals for efficiency.
 * 
 * @param scheduleIntervals List of ON intervals from Part 1 (must be sorted)
 * @param timestamp_hour Hour offset to check
 * @return True if entity is ON at given timestamp
 * 
 * Time Complexity: O(log N) where N = number of ON intervals
 * Space Complexity: O(1)
 * 
 * Algorithm:
 * 1. Set left=0, right=size-1
 * 2. While left <= right:
 *    a. Calculate mid = (left + right) / 2
 *    b. If timestamp in middle interval, return True
 *    c. If timestamp < middle.start, search left half
 *    d. If timestamp >= middle.end, search right half
 * 3. Return False if not found
 */
bool isTimestampIncluded_Binary(const std::vector<TimeInterval>& scheduleIntervals, int timestamp_hour) {
    int left = 0;
    int right = scheduleIntervals.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        const auto& interval = scheduleIntervals[mid];
        
        if (timestamp_hour >= interval.start_hour && timestamp_hour < interval.end_hour) {
            return true;  // Found - entity is ON
        }
        else if (timestamp_hour < interval.start_hour) {
            right = mid - 1;  // Search left half
        }
        else {
            left = mid + 1;   // Search right half
        }
    }
    return false;  // Not found - entity is OFF
}

/**
 * @brief LINEAR SEARCH wrapper for string timestamps
 */
bool isTimestampIncluded_Linear(const std::vector<TimeInterval>& scheduleIntervals, 
                               const std::string& datetime_str, 
                               const std::string& start_date) {
    int timestamp_hour = dateTimeToHour(datetime_str, start_date);
    return isTimestampIncluded_Linear(scheduleIntervals, timestamp_hour);
}

/**
 * @brief BINARY SEARCH wrapper for string timestamps
 */
bool isTimestampIncluded_Binary(const std::vector<TimeInterval>& scheduleIntervals,
                               const std::string& datetime_str,
                               const std::string& start_date) {
    int timestamp_hour = dateTimeToHour(datetime_str, start_date);
    return isTimestampIncluded_Binary(scheduleIntervals, timestamp_hour);
}

/**
 * ================================================================================
 * UTILITY FUNCTIONS
 * ================================================================================
 */

/**
 * @brief Convert hour offset to readable date-time string
 */
std::string hourToDateTime(int hour, const std::string& start_date) {
    int day_offset = hour / 24;
    int hour_in_day = hour % 24;
    
    // Parse start date
    int year = std::stoi(start_date.substr(0, 4));
    int month = std::stoi(start_date.substr(5, 2));
    int day = std::stoi(start_date.substr(8, 2));
    
    // Add day offset
    day += day_offset;
    
    // Handle month overflow (simplified: 30 days per month)
    while (day > 30) {
        day -= 30;
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }
    
    // Format string
    std::string result;
    result += std::to_string(year) + "-";
    result += (month < 10 ? "0" : "") + std::to_string(month) + "-";
    result += (day < 10 ? "0" : "") + std::to_string(day) + " ";
    result += (hour_in_day < 10 ? "0" : "") + std::to_string(hour_in_day) + ":00:00";
    
    return result;
}

/**
 * @brief Print interval in readable format
 */
void printInterval(const TimeInterval& interval, int index, const std::string& start_date) {
    std::cout << "Interval " << index << ": " 
              << hourToDateTime(interval.start_hour, start_date)
              << " to " 
              << hourToDateTime(interval.end_hour, start_date) 
              << std::endl;
}

/**
 * ================================================================================
 * MAIN FUNCTION - COMPREHENSIVE TESTING
 * ================================================================================
 */
int main() {
    std::cout << "================================================================================\n";
    std::cout << "                SCHEDULE CREATOR - COMPLETE SOLUTION TEST\n";
    std::cout << "================================================================================\n\n";
    
    // ========================================================================
    // PART 1: SCHEDULE GENERATION TEST
    // ========================================================================
    std::cout << "PART 1: SCHEDULE GENERATION\n";
    std::cout << std::string(50, '=') << "\n\n";
    
    ScheduleSettings settings = {"2023-10-01", "2023-10-03", 9};
    
    std::cout << "Settings: start=" << settings.start_date 
              << ", end=" << settings.end_date 
              << ", interval=" << settings.interval_window_size_hours << " hours\n\n";
    
    auto schedule = createSchedule(settings);
    
    std::cout << "Generated " << schedule.size() << " ON intervals:\n";
    for (size_t i = 0; i < schedule.size(); ++i) {
        printInterval(schedule[i], i + 1, settings.start_date);
    }
    
    std::cout << "\n" << std::string(70, '-') << "\n\n";
    
    // ========================================================================
    // PART 2: TIMESTAMP QUERY TESTING
    // ========================================================================
    std::cout << "PART 2: TIMESTAMP QUERY TESTING\n";
    std::cout << std::string(50, '=') << "\n\n";
    
    // Test timestamps
    std::vector<std::string> test_timestamps = {
        "2023-10-01 05:00:00",  // Should be ON  (in interval 1)
        "2023-10-01 12:00:00",  // Should be OFF (gap)
        "2023-10-01 20:00:00",  // Should be ON  (in interval 2)
        "2023-10-02 01:00:00",  // Should be ON  (in interval 2)
        "2023-10-02 15:00:00",  // Should be ON  (in interval 3)
        "2023-10-02 22:00:00",  // Should be OFF (gap)
        "2023-10-03 08:00:00",  // Should be ON  (in interval 4)
        "2023-10-03 20:00:00",  // Should be OFF (after schedule)
        "2023-09-30 12:00:00",  // Should be OFF (before schedule)
        "2023-10-05 12:00:00"   // Should be OFF (after schedule)
    };
    
    // Expected results
    std::vector<bool> expected = {
        true,   // 2023-10-01 05:00:00 - in interval 1 [0,9)
        false,  // 2023-10-01 12:00:00 - gap between intervals
        true,   // 2023-10-01 20:00:00 - in interval 2 [18,27)
        true,   // 2023-10-02 01:00:00 - in interval 2 [18,27)
        true,   // 2023-10-02 15:00:00 - in interval 3 [36,45)
        false,  // 2023-10-02 22:00:00 - gap between intervals
        true,   // 2023-10-03 08:00:00 - in interval 4 [54,63)
        false,  // 2023-10-03 20:00:00 - after last interval
        false,  // 2023-09-30 12:00:00 - before schedule
        false   // 2023-10-05 12:00:00 - after schedule
    };
    
    std::cout << "Testing both Linear Search O(N) and Binary Search O(log N):\n\n";
    std::cout << "Timestamp            | Expected | Linear | Binary | Match | Correct\n";
    std::cout << std::string(75, '-') << "\n";
    
    bool all_tests_passed = true;
    
    for (size_t i = 0; i < test_timestamps.size(); ++i) {
        const std::string& timestamp = test_timestamps[i];
        bool expected_result = expected[i];
        
        // Test both approaches
        bool linear_result = isTimestampIncluded_Linear(schedule, timestamp, settings.start_date);
        bool binary_result = isTimestampIncluded_Binary(schedule, timestamp, settings.start_date);
        
        // Check if both approaches match
        bool approaches_match = (linear_result == binary_result);
        
        // Check if results are correct
        bool results_correct = (linear_result == expected_result && binary_result == expected_result);
        
        if (!approaches_match || !results_correct) {
            all_tests_passed = false;
        }
        
        std::cout << timestamp << " |    " << (expected_result ? "ON " : "OFF") 
                  << "    |   " << (linear_result ? "ON " : "OFF")
                  << "  |   " << (binary_result ? "ON " : "OFF")
                  << "  |   " << (approaches_match ? "✓" : "✗")
                  << "   |    " << (results_correct ? "✓" : "✗") << "\n";
    }
    
    std::cout << "\n";
    std::cout << "Overall Test Result: " << (all_tests_passed ? "✓ ALL TESTS PASSED" : "✗ SOME TESTS FAILED") << "\n";
    
    std::cout << "\n" << std::string(70, '-') << "\n\n";
    
    // ========================================================================
    // ADDITIONAL SCHEDULE TESTS
    // ========================================================================
    std::cout << "ADDITIONAL SCHEDULE TESTS\n";
    std::cout << std::string(30, '=') << "\n\n";
    
    // Test 24-hour intervals
    ScheduleSettings settings24 = {"2023-10-01", "2023-10-05", 24};
    auto schedule24 = createSchedule(settings24);
    
    std::cout << "24-hour interval schedule (" << schedule24.size() << " intervals):\n";
    for (size_t i = 0; i < schedule24.size(); ++i) {
        printInterval(schedule24[i], i + 1, settings24.start_date);
    }
    
    // Quick test
    bool test1 = isTimestampIncluded_Binary(schedule24, "2023-10-01 12:00:00", settings24.start_date);
    bool test2 = isTimestampIncluded_Binary(schedule24, "2023-10-02 12:00:00", settings24.start_date);
    std::cout << "Quick test: 2023-10-01 12:00:00 -> " << (test1 ? "ON" : "OFF") << " (should be ON)\n";
    std::cout << "Quick test: 2023-10-02 12:00:00 -> " << (test2 ? "ON" : "OFF") << " (should be OFF)\n";
    
    std::cout << "\n" << std::string(70, '-') << "\n\n";
    
    // Test 6-hour intervals
    ScheduleSettings settings6 = {"2023-10-01", "2023-10-02", 6};
    auto schedule6 = createSchedule(settings6);
    
    std::cout << "6-hour interval schedule (" << schedule6.size() << " intervals):\n";
    for (size_t i = 0; i < schedule6.size(); ++i) {
        printInterval(schedule6[i], i + 1, settings6.start_date);
    }
    
    // ========================================================================
    // PERFORMANCE ANALYSIS
    // ========================================================================
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "PERFORMANCE ANALYSIS\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << "Algorithm Comparison for " << schedule.size() << " intervals:\n";
    std::cout << "• Linear Search  O(N):     worst case = " << schedule.size() << " comparisons\n";
    std::cout << "• Binary Search  O(log N): worst case = " << (int)std::ceil(std::log2(schedule.size())) << " comparisons\n\n";
    std::cout << "Recommendations:\n";
    std::cout << "• Use Linear Search for small schedules (< 100 intervals)\n";
    std::cout << "• Use Binary Search for large schedules (> 100 intervals)\n";
    std::cout << "• Both have O(1) space complexity\n";
    std::cout << std::string(70, '=') << "\n";
    
    return 0;
}

/*
================================================================================
                               PROBLEM STATEMENT
================================================================================

PART 1: SCHEDULE GENERATION
Create a schedule of ON intervals from given settings. Entity alternates between
ON and OFF states with equal durations, always starting with ON.

Function: createSchedule(ScheduleSettings settings)
Returns: Sorted list of ON intervals only

PART 2: TIMESTAMP QUERY  
Given the ON intervals from Part 1, determine if entity is active at any timestamp.

Function: isTimestampIncluded(List<intervals>, Timestamp)
Returns: True if timestamp falls within any ON interval, False otherwise

Two approaches required:
1. O(N) Linear Search - iterate through all intervals
2. O(log N) Binary Search - leverage sorted order

ASSUMPTIONS:
• Interval size given in whole hours
• All times in UTC timezone  
• Start/end dates at 00:00
• Starting status is ON
• 30 days per month (simplified calendar)

COMPLEXITY ANALYSIS:
Part 1: O(N) time, O(K) space where N=total_intervals, K=ON_intervals
Part 2: Linear O(N), Binary O(log N), both O(1) space

================================================================================
*/