/*
================================================================================
                         HCV MINIMUM TRANSFERS PROBLEM
================================================================================

MAIN PROBLEM: Minimum HCV Transfers
===================================
We have Uber HCV (High Capacity Vehicles) operating on fixed circular routes.
Find the minimum number of transfers needed to travel from source to destination.

Input: 
- routes: 2D array where routes[i] = [stop1, stop2, ...] represents route i
- source: starting stop number
- destination: target stop number

Output: Minimum number of HCV transfers needed, or -1 if impossible

Example:
Routes: [[1,2,7], [3,6,7]], Source: 1, Destination: 6
- Board HCV 0 at stop 1 → ride to stop 7 → transfer to HCV 1 → ride to stop 6
- Answer: 1 transfer

FOLLOW-UP 1: Weighted HCV Boarding Costs
========================================
Each HCV has a boarding cost. Find minimum total boarding cost.

Additional Input: routeCosts[i] = cost to board HCV i
Goal: Minimize total boarding cost instead of number of transfers

Example: 
Routes: [[1,2,7], [3,6,7]], Costs: [1,5], Source: 1, Destination: 6
- Board HCV 0 (cost 1) → transfer → board HCV 1 (cost 5) = Total: 6

FOLLOW-UP 2: Per-Segment Ticket Costs  
=====================================
Each stop-to-stop segment has a ticket price. Find minimum travel cost.

Additional Input: ticketCosts[{A,B}] = cost to travel from stop A to stop B
Goal: Minimize total ticket cost for entire journey

Example:
Routes: [[1,2,7], [3,6,7]], Tickets: 1→2:3, 2→7:4, 1→7:10
Source: 1, Destination: 7
- Direct: 1→7 costs 10
- Via: 1→2→7 costs 3+4=7 (cheaper)
================================================================================
*/

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <map>
using namespace std;

class Solution {
public:
    /**
     * Original Problem: Find minimum number of HCV transfers
     * 
     * @param routes: 2D vector where routes[i] represents stops on route i
     * @param source: starting stop
     * @param destination: target stop
     * @return: minimum number of transfers needed, -1 if impossible
     * 
     * Algorithm: BFS on stops
     * - Each BFS level represents number of transfers
     * - Explore all reachable stops on same route with same transfer count
     * - Move to next level when switching to different route
     * 
     * Time Complexity: O(N * M) where N = total stops, M = total routes
     * Space Complexity: O(N * M) for stopToRoutes mapping + O(N) for BFS
     */
    int findMinTransfers(vector<vector<int>>& routes, int source, int destination) {
        if (source == destination) return 0;
        
        // Build mapping: stop -> list of route indices
        unordered_map<int, vector<int>> stopToRoutes;
        for (int i = 0; i < routes.size(); i++) {
            for (int stop : routes[i]) {
                stopToRoutes[stop].push_back(i);
            }
        }
        
        // Check if source and destination exist
        if (stopToRoutes.find(source) == stopToRoutes.end() || 
            stopToRoutes.find(destination) == stopToRoutes.end()) {
            return -1;
        }
        
        // BFS
        queue<pair<int, int>> q;  // (stop, transfers)
        unordered_set<int> visited;
        
        q.push({source, 0});
        visited.insert(source);
        
        while (!q.empty()) {
            int currentStop = q.front().first;
            int transfers = q.front().second;
            q.pop();
            
            // Try all routes passing through current stop
            for (int routeIdx : stopToRoutes[currentStop]) {
                for (int nextStop : routes[routeIdx]) {
                    if (nextStop == destination) {
                        return transfers;
                    }
                    
                    if (visited.find(nextStop) == visited.end()) {
                        visited.insert(nextStop);
                        q.push({nextStop, transfers + 1});
                    }
                }
            }
        }
        
        return -1;
    }
    
    /**
     * Follow-up 1: Find minimum HCV boarding cost
     * 
     * @param routes: 2D vector where routes[i] represents stops on route i
     * @param routeCosts: vector where routeCosts[i] is cost to board HCV i
     * @param source: starting stop
     * @param destination: target stop
     * @return: minimum total boarding cost, -1 if impossible
     * 
     * Algorithm: Dijkstra's algorithm on stops
     * - Priority queue ensures we explore lowest cost paths first
     * - Cost increases by routeCosts[i] when boarding route i
     * - Each route boarding adds its specific cost to total
     * 
     * Time Complexity: O((N + M) * log(N)) where N = stops, M = routes
     * Space Complexity: O(N + M) for data structures
     */
    int findMinCostWithWeights(vector<vector<int>>& routes, vector<int>& routeCosts, 
                               int source, int destination) {
        if (source == destination) return 0;
        if (routes.size() != routeCosts.size()) return -1;
        
        // Build mapping: stop -> list of route indices
        unordered_map<int, vector<int>> stopToRoutes;
        for (int i = 0; i < routes.size(); i++) {
            for (int stop : routes[i]) {
                stopToRoutes[stop].push_back(i);
            }
        }
        
        // Check if source and destination exist
        if (stopToRoutes.find(source) == stopToRoutes.end() || 
            stopToRoutes.find(destination) == stopToRoutes.end()) {
            return -1;
        }
        
        // Dijkstra
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;  // (cost, stop)
        unordered_map<int, int> minCost;
        
        pq.push({0, source});
        minCost[source] = 0;
        
        while (!pq.empty()) {
            int currentCost = pq.top().first;
            int currentStop = pq.top().second;
            pq.pop();
            
            if (currentStop == destination) {
                return currentCost;
            }
            
            if (minCost.find(currentStop) != minCost.end() && 
                currentCost > minCost[currentStop]) {
                continue;
            }
            
            // Try all routes passing through current stop
            for (int routeIdx : stopToRoutes[currentStop]) {
                int newCost = currentCost + routeCosts[routeIdx];
                
                for (int nextStop : routes[routeIdx]) {
                    if (minCost.find(nextStop) == minCost.end() || newCost < minCost[nextStop]) {
                        minCost[nextStop] = newCost;
                        pq.push({newCost, nextStop});
                    }
                }
            }
        }
        
        return -1;
    }
    
    /**
     * Follow-up 2: Find minimum travel cost between stops
     * 
     * @param routes: 2D vector where routes[i] represents stops on route i
     * @param ticketCosts: map where ticketCosts[{A,B}] is cost to travel from stop A to B
     * @param source: starting stop
     * @param destination: target stop
     * @return: minimum total travel cost, -1 if impossible
     * 
     * Algorithm: Dijkstra's algorithm on stops
     * - Cost increases by ticket price for each stop-to-stop segment
     * - Only considers segments where tickets are available
     * - Models real-world scenario with per-segment pricing
     * 
     * Time Complexity: O((N + M) * log(N)) where N = stops, M = routes
     * Space Complexity: O(N + M + T) where T = number of ticket combinations
     */
    int findMinTravelCost(vector<vector<int>>& routes, map<pair<int, int>, int>& ticketCosts, 
                          int source, int destination) {
        if (source == destination) return 0;
        
        // Build mapping: stop -> list of route indices
        unordered_map<int, vector<int>> stopToRoutes;
        for (int i = 0; i < routes.size(); i++) {
            for (int stop : routes[i]) {
                stopToRoutes[stop].push_back(i);
            }
        }
        
        // Check if source and destination exist
        if (stopToRoutes.find(source) == stopToRoutes.end() || 
            stopToRoutes.find(destination) == stopToRoutes.end()) {
            return -1;
        }
        
        // Dijkstra
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;  // (cost, stop)
        unordered_map<int, int> minCost;
        
        pq.push({0, source});
        minCost[source] = 0;
        
        while (!pq.empty()) {
            int currentCost = pq.top().first;
            int currentStop = pq.top().second;
            pq.pop();
            
            if (currentStop == destination) {
                return currentCost;
            }
            
            if (minCost.find(currentStop) != minCost.end() && 
                currentCost > minCost[currentStop]) {
                continue;
            }
            
            // Try all routes passing through current stop
            for (int routeIdx : stopToRoutes[currentStop]) {
                for (int nextStop : routes[routeIdx]) {
                    if (nextStop == currentStop) continue;
                    
                    // Check if ticket exists from currentStop to nextStop
                    pair<int, int> ticket = {currentStop, nextStop};
                    if (ticketCosts.find(ticket) == ticketCosts.end()) {
                        continue;
                    }
                    
                    int travelCost = ticketCosts[ticket];
                    int newCost = currentCost + travelCost;
                    
                    if (minCost.find(nextStop) == minCost.end() || newCost < minCost[nextStop]) {
                        minCost[nextStop] = newCost;
                        pq.push({newCost, nextStop});
                    }
                }
            }
        }
        
        return -1;
    }
};

int main() {
    Solution solution;
    
    // ========== ORIGINAL TESTS ==========
    cout << "========== ORIGINAL: MIN TRANSFERS ==========" << endl;
    
    vector<vector<int>> routes1 = {{1, 2, 7}, {3, 6, 7}};
    cout << "Routes: [[1,2,7], [3,6,7]]" << endl;
    cout << "Source 1 -> Dest 7: " << solution.findMinTransfers(routes1, 1, 7) << " (expect 0)" << endl;
    cout << "Source 1 -> Dest 6: " << solution.findMinTransfers(routes1, 1, 6) << " (expect 1)" << endl;
    
    vector<vector<int>> routes2 = {{1,2,3,4,5,6,7,8,9,10}, {2,7}};
    cout << "Source 1 -> Dest 7: " << solution.findMinTransfers(routes2, 1, 7) << " (expect 0)" << endl;
    
    vector<vector<int>> routes3 = {{1, 2, 3}, {4, 5, 6}};
    cout << "Source 1 -> Dest 5: " << solution.findMinTransfers(routes3, 1, 5) << " (expect -1)" << endl;
    
    cout << "Source 2 -> Dest 2: " << solution.findMinTransfers(routes1, 2, 2) << " (expect 0)" << endl;
    
    // ========== FOLLOW-UP 1 TESTS ==========
    cout << "\n========== FOLLOW-UP 1: MIN BOARDING COST ==========" << endl;
    
    vector<int> costs1 = {1, 5};
    cout << "Routes: [[1,2,7], [3,6,7]], Costs: [1,5]" << endl;
    cout << "Source 1 -> Dest 7: " << solution.findMinCostWithWeights(routes1, costs1, 1, 7) << " (expect 1)" << endl;
    cout << "Source 1 -> Dest 6: " << solution.findMinCostWithWeights(routes1, costs1, 1, 6) << " (expect 6)" << endl;
    
    vector<int> costs2 = {10, 2};
    cout << "Costs: [10,2]" << endl;
    cout << "Source 1 -> Dest 7: " << solution.findMinCostWithWeights(routes1, costs2, 1, 7) << " (expect 10)" << endl;
    
    vector<vector<int>> routes4 = {{1, 2}, {2, 3}, {1, 3}};
    vector<int> costs4 = {1, 1, 10};
    cout << "Trade-off: Routes [[1,2], [2,3], [1,3]], Costs [1,1,10]" << endl;
    cout << "Min transfers 1->3: " << solution.findMinTransfers(routes4, 1, 3) << " transfers" << endl;
    cout << "Min cost 1->3: " << solution.findMinCostWithWeights(routes4, costs4, 1, 3) << " cost" << endl;
    
    // ========== FOLLOW-UP 2 TESTS ==========
    cout << "\n========== FOLLOW-UP 2: MIN TRAVEL COST ==========" << endl;
    
    map<pair<int, int>, int> ticketCosts;
    ticketCosts[{1, 2}] = 3;
    ticketCosts[{2, 7}] = 4;
    ticketCosts[{1, 7}] = 10;
    ticketCosts[{7, 6}] = 2;
    ticketCosts[{7, 3}] = 1;
    ticketCosts[{3, 6}] = 5;
    
    cout << "Routes: [[1,2,7], [3,6,7]]" << endl;
    cout << "Ticket costs: 1->2:3, 2->7:4, 1->7:10, 7->6:2, 7->3:1, 3->6:5" << endl;
    
    cout << "Source 1 -> Dest 7:" << endl;
    cout << "  Direct 1->7 = cost 10" << endl;
    cout << "  Via 1->2->7 = cost 3+4 = 7" << endl;
    int result1 = solution.findMinTravelCost(routes1, ticketCosts, 1, 7);
    cout << "  Result: " << result1 << " (expect 7)" << endl;
    
    cout << "Source 1 -> Dest 6:" << endl;
    cout << "  Via 1->2->7->6 = cost 3+4+2 = 9" << endl;
    cout << "  Via 1->7->6 = cost 10+2 = 12" << endl;
    int result2 = solution.findMinTravelCost(routes1, ticketCosts, 1, 6);
    cout << "  Result: " << result2 << " (expect 9)" << endl;
    
    map<pair<int, int>, int> limitedCosts;
    limitedCosts[{1, 2}] = 5;
    cout << "Limited tickets (only 1->2:5):" << endl;
    int result3 = solution.findMinTravelCost(routes1, limitedCosts, 1, 7);
    cout << "Source 1 -> Dest 7: " << result3 << " (expect -1)" << endl;
    
    return 0;
}

/*
================================================================================
                            PROBLEM DESCRIPTIONS
================================================================================

MAIN PROBLEM: Minimum HCV Transfers
===================================
We have Uber HCV (High Capacity Vehicles) operating on fixed circular routes.
Each route is represented as an array of stops, e.g., [[1,2,7], [3,6,7]]

Route Operation:
- Route [1,2,7]: HCV travels 1 → 2 → 7 → 1 → 2 → 7 → ... (continuous loop)
- Route [3,6,7]: HCV travels 3 → 6 → 7 → 3 → 6 → 7 → ... (continuous loop)

Goal: Find minimum number of HCV transfers to get from source to destination.

Examples:
1. Routes: [[1,2,7], [3,6,7]], Source: 1, Destination: 7
   Output: 0 (board first HCV at stop 1, ride directly to stop 7)

2. Routes: [[1,2,7], [3,6,7]], Source: 1, Destination: 6  
   Output: 1 (board first HCV at stop 1, transfer at stop 7 to second HCV, 
           ride to stop 6)

3. Routes: [[1,2,3,4,5,6,7,8,9,10], [2,7]], Source: 1, Destination: 7
   Output: 0 (take first HCV directly, even though second route is shorter)

Return -1 if destination is unreachable from source.
Priority: Minimize transfers, NOT minimize number of stops traveled.

FOLLOW-UP 1: Weighted HCV Boarding Costs
========================================
Extension: Each HCV has a boarding cost (e.g., premium vs economy HCVs).

Input: Additional vector routeCosts where routeCosts[i] = cost to board HCV i
Goal: Find minimum total boarding cost instead of minimum transfers.

Example:
Routes: [[1,2,7], [3,6,7]], Costs: [1, 5], Source: 1, Destination: 6
- Path: Board HCV 0 (cost 1) → transfer at stop 7 → board HCV 1 (cost 5)
- Total cost: 1 + 5 = 6

Trade-off: Sometimes more transfers with cheaper HCVs cost less than fewer 
transfers with expensive HCVs.

FOLLOW-UP 2: Per-Segment Ticket Costs
=====================================
Extension: Instead of boarding costs, each stop-to-stop segment has a ticket price.

Input: Map ticketCosts where ticketCosts[{A,B}] = cost to travel from stop A to B
Goal: Find minimum total travel cost for the entire journey.

Example:
Routes: [[1,2,7], [3,6,7]]
Tickets: 1→2:3, 2→7:4, 1→7:10, 7→6:2

Source 1 → Destination 7:
- Option 1: Direct ticket 1→7 = cost 10
- Option 2: Multi-hop 1→2→7 = cost 3+4 = 7
- Choose cheaper: cost 7

Real-world application: Models airline/train pricing where direct routes 
might be expensive but connecting flights/trains could be cheaper.

================================================================================
                            SOLUTION APPROACHES
================================================================================

ALGORITHM COMPARISON:
| Problem    | Algorithm | Data Structure | Optimization Target    | Edge Weight |
|------------|-----------|----------------|------------------------|-------------|
| Original   | BFS       | Queue          | # Transfers           | +1          |
| Follow-up 1| Dijkstra  | Priority Queue | Boarding Cost         | +cost[i]    |
| Follow-up 2| Dijkstra  | Priority Queue | Travel Cost           | +ticket[A,B]|

COMPLEXITY ANALYSIS:
===================
Let N = total number of unique stops, M = number of routes

Original Problem:
- Time: O(N * M) - each stop visited once, check all routes per stop
- Space: O(N * M) - stopToRoutes mapping + BFS queue

Follow-up 1 & 2:
- Time: O((N + M) * log(N)) - Dijkstra with priority queue operations
- Space: O(N + M) - data structures for Dijkstra + route mappings

DESIGN DECISIONS:
================
1. Stop-centric model: Treats stops as graph nodes, routes as connections
2. BFS for unweighted (original), Dijkstra for weighted (follow-ups)
3. Consistent find() != end() pattern for all existence checks
4. Simple map<pair<int,int>, int> for ticket costs (no custom hash needed)
5. Comprehensive test cases covering edge cases and trade-offs

WHY THIS PROBLEM IS EXCELLENT:
=============================
1. Tests graph modeling skills (choosing right abstraction)
2. Algorithm selection (BFS vs Dijkstra based on problem constraints)
3. Data structure proficiency (maps, sets, queues, priority queues)
4. Time/space complexity analysis
5. Real-world application understanding (transportation optimization)
6. Trade-off analysis (transfers vs cost optimization)
*/