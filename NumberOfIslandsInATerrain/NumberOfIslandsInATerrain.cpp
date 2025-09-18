#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
using namespace std;

// Simple type alias for coordinates - much cleaner than custom struct!
using Point = pair<int, int>;

// =============================================================================
// SOLUTION 1A: DFS APPROACH WITH MATRIX REPRESENTATION
// Time Complexity: addLand() = O(1), isLand() = O(1), getIslands() = O(width * height)
// Space Complexity: O(width * height)
// =============================================================================
class TerrainDFSMatrix {
private:
    vector<vector<bool>> grid;
    int width, height;
    
    // 4-directional neighbors
    vector<Point> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    
    // Check if coordinates are within bounds
    bool isValid(int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
    
    // DFS to mark all connected land cells as visited
    void dfs(int x, int y, vector<vector<bool>>& visited) {
        if (!isValid(x, y) || visited[x][y] || !grid[x][y]) {
            return;
        }
        
        visited[x][y] = true;
        
        // Visit all 4 neighbors
        for (auto& dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;
            dfs(nx, ny, visited);
        }
    }
    
public:
    // Constructor with fixed grid size
    TerrainDFSMatrix(int w = 100, int h = 100) : width(w), height(h) {
        grid = vector<vector<bool>>(width, vector<bool>(height, false));
    }
    
    // Add a land point - O(1)
    void addLand(int x, int y) {
        if (isValid(x, y)) {
            grid[x][y] = true;
        }
    }
    
    // Check if point is land - O(1)
    bool isLand(int x, int y) {
        return isValid(x, y) && grid[x][y];
    }
    
    // Count number of islands using DFS - O(width * height)
    int getIslands() {
        vector<vector<bool>> visited(width, vector<bool>(height, false));
        int islandCount = 0;
        
        // Iterate through entire grid
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (grid[i][j] && !visited[i][j]) {
                    // Found a new island, start DFS
                    islandCount++;
                    dfs(i, j, visited);
                }
            }
        }
        
        return islandCount;
    }
    
    void printLands() {
        cout << "DFS Matrix - Land cells: ";
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (grid[i][j]) {
                    cout << "(" << i << "," << j << ") ";
                }
            }
        }
        cout << endl;
    }
    
    void printGrid(int maxX = 10, int maxY = 10) {
        cout << "Grid visualization (showing " << maxX << "x" << maxY << "):" << endl;
        for (int j = 0; j < min(maxY, height); j++) {
            for (int i = 0; i < min(maxX, width); i++) {
                cout << (grid[i][j] ? "X" : ".") << " ";
            }
            cout << endl;
        }
    }
};

// =============================================================================
// SOLUTION 1B: DFS APPROACH WITH SPARSE SET REPRESENTATION
// Time Complexity: addLand() = O(log n), isLand() = O(log n), getIslands() = O(number_of_lands)
// Space Complexity: O(number_of_lands)
// =============================================================================
class TerrainDFSSet {
private:
    set<Point> landCells;  // Only store land points (sparse representation)
    
    // 4-directional neighbors
    vector<Point> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    
    // DFS to mark all connected land cells as visited
    void dfs(Point start, set<Point>& visited) {
        if (visited.count(start)) return;
        
        visited.insert(start);
        
        // Check all 4 neighbors
        for (auto& dir : directions) {
            Point neighbor = {start.first + dir.first, start.second + dir.second};
            
            // If neighbor is land and not visited, continue DFS
            if (landCells.count(neighbor) && !visited.count(neighbor)) {
                dfs(neighbor, visited);
            }
        }
    }
    
public:
    // Add a land point - O(log n) due to set insertion
    void addLand(int x, int y) {
        landCells.insert({x, y});
    }
    
    // Check if point is land - O(log n) due to set lookup
    bool isLand(int x, int y) {
        return landCells.count({x, y});
    }
    
    // Count number of islands using DFS - O(number_of_lands)
    int getIslands() {
        if (landCells.empty()) return 0;
        
        set<Point> visited;
        int islandCount = 0;
        
        // Iterate through all land cells only
        for (const auto& land : landCells) {
            if (!visited.count(land)) {
                // Found a new island, start DFS
                islandCount++;
                dfs(land, visited);
            }
        }
        
        return islandCount;
    }
    
    void printLands() {
        cout << "DFS Set - Land cells: ";
        for (const auto& p : landCells) {
            cout << "(" << p.first << "," << p.second << ") ";
        }
        cout << endl;
    }
};

// =============================================================================
// SOLUTION 2: UNION-FIND APPROACH WITH INCREMENTAL ISLAND COUNTING
// Time Complexity: addLand() = O(α(n)), isLand() = O(1), getIslands() = O(1)
// Space Complexity: O(number_of_lands)
// Note: α(n) is inverse Ackermann function, practically constant
// =============================================================================
class TerrainUnionFind {
private:
    map<Point, Point> parent;  // Point -> Parent mapping
    map<Point, int> rank;      // For union by rank optimization
    int islandCount;
    
    // 4-directional neighbors
    vector<Point> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    
    // Find root with path compression - O(α(n))
    Point find(Point p) {
        if (parent[p] == p) {
            return p;
        }
        // Path compression: make all nodes point directly to root
        return parent[p] = find(parent[p]);
    }
    
    // Union two components by rank - O(α(n))
    bool unionSets(Point p1, Point p2) {
        Point root1 = find(p1);
        Point root2 = find(p2);
        
        if (root1 == root2) return false;  // Already in same set
        
        // Union by rank: attach smaller tree under bigger tree
        if (rank[root1] < rank[root2]) {
            parent[root1] = root2;
        } else if (rank[root1] > rank[root2]) {
            parent[root2] = root1;
        } else {
            parent[root2] = root1;
            rank[root1]++;
        }
        
        return true;  // Successfully merged two sets
    }
    
public:
    TerrainUnionFind() : islandCount(0) {}
    
    // Add land and incrementally maintain island count - O(α(n))
    void addLand(int x, int y) {
        Point newPoint = {x, y};
        
        // If already exists, do nothing
        if (parent.count(newPoint)) return;
        
        // Initialize new point as its own parent
        parent[newPoint] = newPoint;
        rank[newPoint] = 0;
        islandCount++;  // Initially, new land is its own island
        
        // Check all 4 neighbors and union if they are land
        for (auto& dir : directions) {
            Point neighbor = {newPoint.first + dir.first, newPoint.second + dir.second};
            
            if (parent.count(neighbor)) {  // If neighbor is land
                if (unionSets(newPoint, neighbor)) {
                    islandCount--;  // Two islands merged into one
                }
            }
        }
    }
    
    // Check if point is land - O(1)
    bool isLand(int x, int y) {
        return parent.count({x, y});
    }
    
    // Get island count in constant time - O(1)
    int getIslands() {
        return islandCount;
    }
    
    void printLands() {
        cout << "UnionFind - Land cells: ";
        for (const auto& pair : parent) {
            cout << "(" << pair.first.first << "," << pair.first.second << ") ";
        }
        cout << endl;
    }
};

// =============================================================================
// TESTING ALL THREE APPROACHES
// =============================================================================
void testAllApproaches() {
    cout << "=== Testing All Three Approaches ===" << endl;
    
    TerrainDFSMatrix terrainMatrix(20, 20);  // 20x20 grid
    TerrainDFSSet terrainSet;
    TerrainUnionFind terrainUF;
    
    // Test case 1: Single connected island
    cout << "\n--- Test 1: Single Connected Island ---" << endl;
    vector<Point> testLands1 = {{0,0}, {0,1}, {1,0}, {1,1}};
    
    for (auto& land : testLands1) {
        terrainMatrix.addLand(land.first, land.second);
        terrainSet.addLand(land.first, land.second);
        terrainUF.addLand(land.first, land.second);
    }
    
    terrainMatrix.printLands();
    terrainSet.printLands();
    terrainUF.printLands();
    cout << "Matrix DFS Islands: " << terrainMatrix.getIslands() << endl;
    cout << "Set DFS Islands: " << terrainSet.getIslands() << endl;
    cout << "UnionFind Islands: " << terrainUF.getIslands() << endl;
    
    terrainMatrix.printGrid(5, 5);
    
    // Test case 2: Multiple separate islands
    cout << "\n--- Test 2: Multiple Separate Islands ---" << endl;
    TerrainDFSMatrix terrainMatrix2(20, 20);
    TerrainDFSSet terrainSet2;
    TerrainUnionFind terrainUF2;
    
    // Island 1: (0,0)
    terrainMatrix2.addLand(0, 0);
    terrainSet2.addLand(0, 0);
    terrainUF2.addLand(0, 0);
    
    // Island 2: (2,2), (2,3)
    terrainMatrix2.addLand(2, 2);
    terrainMatrix2.addLand(2, 3);
    terrainSet2.addLand(2, 2);
    terrainSet2.addLand(2, 3);
    terrainUF2.addLand(2, 2);
    terrainUF2.addLand(2, 3);
    
    // Island 3: (5,5)
    terrainMatrix2.addLand(5, 5);
    terrainSet2.addLand(5, 5);
    terrainUF2.addLand(5, 5);
    
    cout << "Matrix DFS Islands: " << terrainMatrix2.getIslands() << endl;
    cout << "Set DFS Islands: " << terrainSet2.getIslands() << endl;
    cout << "UnionFind Islands: " << terrainUF2.getIslands() << endl;
    
    terrainMatrix2.printGrid(8, 8);
    
    // Test case 3: Dynamic merging demonstration
    cout << "\n--- Test 3: Dynamic Island Merging ---" << endl;
    TerrainDFSMatrix terrainMatrix3(20, 20);
    TerrainDFSSet terrainSet3;
    TerrainUnionFind terrainUF3;
    
    // Initially separate islands
    terrainMatrix3.addLand(0, 0);
    terrainMatrix3.addLand(0, 2);
    terrainSet3.addLand(0, 0);
    terrainSet3.addLand(0, 2);
    terrainUF3.addLand(0, 0);
    terrainUF3.addLand(0, 2);
    
    cout << "Before bridge:" << endl;
    cout << "Matrix DFS Islands: " << terrainMatrix3.getIslands() << endl;
    cout << "Set DFS Islands: " << terrainSet3.getIslands() << endl;
    cout << "UnionFind Islands: " << terrainUF3.getIslands() << endl;
    terrainMatrix3.printGrid(5, 5);
    
    // Add bridge to connect them
    terrainMatrix3.addLand(0, 1);
    terrainSet3.addLand(0, 1);
    terrainUF3.addLand(0, 1);
    
    cout << "\nAfter bridge:" << endl;
    cout << "Matrix DFS Islands: " << terrainMatrix3.getIslands() << endl;
    cout << "Set DFS Islands: " << terrainSet3.getIslands() << endl;
    cout << "UnionFind Islands: " << terrainUF3.getIslands() << endl;
    terrainMatrix3.printGrid(5, 5);
}

// Performance comparison demonstration
void performanceDemo() {
    cout << "\n=== Performance Comparison Demo ===" << endl;
    
    // Scenario: Sparse terrain (few lands in large space)
    cout << "\n--- Sparse Terrain Scenario ---" << endl;
    TerrainDFSMatrix terrainMatrix(1000, 1000);  // Large grid
    TerrainDFSSet terrainSet;                    // Sparse representation
    
    // Add only 5 scattered land points
    vector<Point> sparseLands = {
        {10, 10}, {100, 200}, {300, 400}, {500, 600}, {700, 800}
    };
    
    for (auto& land : sparseLands) {
        terrainMatrix.addLand(land.first, land.second);
        terrainSet.addLand(land.first, land.second);
    }
    
    cout << "Sparse lands added: " << sparseLands.size() << endl;
    cout << "Matrix approach scans: 1000x1000 = 1,000,000 cells" << endl;
    cout << "Set approach visits: " << sparseLands.size() << " land cells only" << endl;
    cout << "Matrix Islands: " << terrainMatrix.getIslands() << endl;
    cout << "Set Islands: " << terrainSet.getIslands() << endl;
}

int main() {
    testAllApproaches();
    performanceDemo();
    return 0;
}

/*
===============================================================================
PROBLEM STATEMENT:
===============================================================================

Build a 2-dimensional terrain with land and water parts.

PART 1: Basic Data Structure
- Implement a data structure to efficiently represent terrain in memory
- Required operations:
  * isLand(x, y): returns true if point (x,y) is land
  * addLand(x, y): sets point (x,y) to land
- Assumptions:
  * Everything is water at the beginning
  * Only 2 types of points exist: land or water

PART 2: Island Counting
- Extend data structure to add getIslands() method
- Returns number of unique connected components of land
- Assumptions:
  * Use 4-neighborhood (north, south, east, west)
  * Land cells are connected if they are adjacent horizontally or vertically

Example:
. . . . X . X . . .
X . = 3 islands
X X X . . . . . . .
. X

===============================================================================
COMPLEXITY ANALYSIS:
===============================================================================

DFS MATRIX APPROACH:
- addLand(): O(1) - direct array access
- isLand(): O(1) - direct array access
- getIslands(): O(width * height) - scan entire grid
- Space: O(width * height) - full grid storage

DFS SET APPROACH:
- addLand(): O(log n) - set insertion, where n = number of lands
- isLand(): O(log n) - set lookup
- getIslands(): O(number_of_lands) - only visit actual land cells
- Space: O(number_of_lands) - sparse representation

UNION-FIND APPROACH:
- addLand(): O(α(n)) - nearly constant with path compression
- isLand(): O(1) - map lookup
- getIslands(): O(1) - return cached count
- Space: O(number_of_lands) - sparse representation

Note: α(n) is the inverse Ackermann function, practically constant for all realistic inputs.

===============================================================================
MATRIX vs SET DFS COMPARISON:
===============================================================================

MATRIX DFS ADVANTAGES:
- Simple implementation with direct indexing
- O(1) addLand() and isLand() operations
- No complex data structures needed
- Cache-friendly for dense terrains

MATRIX DFS DISADVANTAGES:
- Always O(width * height) for getIslands(), even with few lands
- High memory usage for sparse terrains
- Fixed bounds limitation
- Poor performance for very sparse scenarios

SET DFS ADVANTAGES:
- Memory efficient for sparse terrains
- getIslands() complexity depends only on actual land count
- No bounds limitations (can handle any coordinates)
- Excellent for very sparse scenarios

SET DFS DISADVANTAGES:
- O(log n) operations due to set structure
- Slightly more complex implementation
- Less cache-friendly than matrix approach
- Higher constant factors for dense terrains

===============================================================================
KEY IMPROVEMENT WITH using Point = pair<int, int>:
===============================================================================

BENEFITS:
- Clean, readable code without custom struct complexity
- No need to define custom operators (< and ==)
- Built-in pair functionality works perfectly with STL containers
- Zero overhead compared to custom Point struct
- Interview-friendly - simple and universally understood

USAGE EXAMPLES:
- Point p = {x, y};                    // Simple creation
- set<Point> landCells;                // Clean container declaration
- if (landCells.count({x, y})) { }     // Easy lookup
- Point neighbor = {p.first + dx, p.second + dy};  // Coordinate arithmetic

===============================================================================
WHEN TO USE WHICH SOLUTION:
===============================================================================

USE MATRIX DFS WHEN:
- Terrain is dense (many land cells relative to total area)
- Grid size is small and known in advance
- Need fastest possible addLand()/isLand() operations
- Memory usage is not a primary concern
- Cache locality is important

USE SET DFS WHEN:
- Terrain is very sparse (few scattered land cells)
- Query pattern: Many addLand() calls, few getIslands() calls
- Unknown or unbounded coordinate space
- Memory efficiency is critical
- Coordinate space is very large

USE UNION-FIND WHEN:
- getIslands() called frequently (10x or more than addLand)
- Need real-time island count updates
- Can afford slightly more complex implementation
- Performance of getIslands() is critical

EXAMPLE SCENARIOS:
- Small game map (100x100, dense): Matrix DFS
- Large world map (10000x10000, sparse): Set DFS  
- Real-time strategy game with frequent queries: Union-Find
- Procedural generation with rare validation: Set DFS
- Memory-constrained embedded system: Set DFS

===============================================================================
*/