/*
 * ===============================================================================
 * DEPENDENCY RESOLUTION USING KAHN'S ALGORITHM (TOPOLOGICAL SORT)
 * ===============================================================================
 * 
 * ALGORITHM OVERVIEW:
 * ------------------
 * This implementation uses Kahn's Algorithm for topological sorting to solve
 * the dependency resolution problem. The algorithm works as follows:
 * 
 * 1. INDEGREE CALCULATION:
 *    - For each package, count how many other packages depend on it
 *    - Indegree[X] = number of packages that have X as a dependency
 *    - Packages with indegree = 0 have no dependencies (can be built first)
 * 
 * 2. KAHN'S ALGORITHM STEPS:
 *    a) Initialize a queue with all packages having indegree = 0
 *    b) While queue is not empty:
 *       - Remove a package from queue and add to result
 *       - For all packages that depend on this package:
 *         * Decrease their indegree by 1
 *         * If indegree becomes 0, add to queue
 *    c) If result contains all packages → valid topological order
 *    d) If result is incomplete → cycle detected
 * 
 * 3. CYCLE DETECTION:
 *    - If we cannot process all packages, a cycle exists
 *    - This happens when remaining packages all have indegree > 0
 *    - Indicates circular dependencies that cannot be resolved
 * 
 * TIME COMPLEXITY: O(V + E)
 * - V = number of packages (vertices)
 * - E = number of dependency relationships (edges)
 * - Each package is processed exactly once
 * - Each dependency edge is examined exactly once
 * 
 * SPACE COMPLEXITY: O(V + E)
 * - Graph storage: O(V + E) for adjacency lists
 * - Indegree map: O(V) for storing indegree of each package
 * - Queue: O(V) in worst case (all packages have indegree 0 initially)
 * - Result vector: O(V) for storing topological order
 * - Visited set: O(V) for DFS traversal to find needed packages
 * 
 * ADVANTAGES OF KAHN'S ALGORITHM:
 * - Efficient: Single pass through the graph
 * - Intuitive: Mimics actual build process (build what has no dependencies first)
 * - Natural cycle detection: Incomplete result indicates cycles
 * - Iterative: No recursion, no stack overflow risk
 * 
 * ===============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <stdexcept>

using namespace std;

/**
 * DependencyResolver - Resolves package build dependencies using topological sort
 * 
 * This class maintains a directed graph where:
 * - Nodes represent packages
 * - Directed edge A → B means "A depends on B" (B must be built before A)
 * 
 * The class provides functionality to:
 * 1. Add packages with their dependencies
 * 2. Compute valid build order for any target package
 * 3. Detect circular dependencies and report errors
 */
class DependencyResolver {
private:
    /**
     * Graph representation using adjacency list
     * Key: Package name
     * Value: List of packages this package depends on
     * 
     * Example: graph["Service"] = ["Adapters", "Core", "Utils"]
     * Means: Service depends on Adapters, Core, and Utils
     * 
     * Space Complexity: O(V + E) where V = packages, E = dependencies
     */
    unordered_map<string, vector<string>> graph;
    
    /**
     * Validates and retrieves dependencies for a given package
     * 
     * @param package The package name to get dependencies for
     * @return Vector of dependency package names
     * @throws runtime_error if package doesn't exist, has self-dependency,
     *         or references non-existent dependencies
     * 
     * Time Complexity: O(D) where D = number of dependencies for this package
     * Space Complexity: O(D) for returning copy of dependencies vector
     */
    vector<string> getPackageDependencies(const string& package) {
        // Check if package exists in graph
        if (graph.find(package) == graph.end()) {
            throw runtime_error("Package '" + package + "' does not exist");
        }
        
        const auto& dependencies = graph[package];
        
        // Validate each dependency
        for (const auto& dep : dependencies) {
            // Check for self-dependency (A depends on A)
            if (dep == package) {
                throw runtime_error("Self-dependency detected in package '" + package + "'");
            }
            // Check if dependency package exists
            if (graph.find(dep) == graph.end()) {
                throw runtime_error("Dependency '" + dep + "' required by '" + package + "' does not exist");
            }
        }
        
        return dependencies;
    }

public:
    /**
     * Adds a package to the dependency graph
     * 
     * @param package The name of the package to add
     * @param dependencies List of packages this package depends on (default: empty)
     * 
     * Time Complexity: O(1) - Just inserting into hash map
     * Space Complexity: O(D) where D = number of dependencies
     * 
     * Note: This doesn't validate dependencies immediately - validation happens
     * during getBuildOrder() to allow adding packages in any order
     */
    void addPackage(const string& package, const vector<string>& dependencies = {}) {
        graph[package] = dependencies;
    }
    
    /**
     * Computes the build order for a target package using Kahn's Algorithm
     * 
     * @param targetPackage The package for which to compute build order
     * @return Vector containing packages in the order they should be built
     * @throws runtime_error for invalid input, missing packages, or cycles
     * 
     * ALGORITHM PHASES:
     * 
     * Phase 1: Dependency Discovery (DFS Traversal)
     * - Time: O(V + E), Space: O(V)
     * - Finds all packages needed to build the target
     * - Uses DFS to traverse dependency graph
     * 
     * Phase 2: Indegree Calculation  
     * - Time: O(V + E), Space: O(V)
     * - Counts incoming edges (dependencies) for each package
     * - Package with indegree 0 = no dependencies = can build immediately
     * 
     * Phase 3: Kahn's Algorithm (Topological Sort)
     * - Time: O(V + E), Space: O(V) 
     * - Processes packages in dependency order
     * - Uses queue to maintain packages ready to build (indegree = 0)
     * 
     * Phase 4: Cycle Detection
     * - Time: O(1), Space: O(1)
     * - If not all packages processed → cycle exists
     * 
     * TOTAL TIME COMPLEXITY: O(V + E)
     * TOTAL SPACE COMPLEXITY: O(V + E)
     */
    vector<string> getBuildOrder(const string& targetPackage) {
        // Input validation
        if (targetPackage.empty()) {
            throw runtime_error("Package name cannot be empty");
        }
        
        if (graph.find(targetPackage) == graph.end()) {
            throw runtime_error("Target package '" + targetPackage + "' does not exist");
        }
        
        // ===================================================================
        // PHASE 1: DEPENDENCY DISCOVERY (DFS TRAVERSAL)
        // ===================================================================
        // Find all packages needed to build the target package
        // Uses DFS to traverse the dependency graph starting from target
        //
        // Time Complexity: O(V + E) - visits each node and edge once
        // Space Complexity: O(V) - for visited set and stack
        
        unordered_set<string> neededPackages;  // All packages in subgraph
        stack<string> stack;                   // DFS stack
        stack.push(targetPackage);
        
        while (!stack.empty()) {
            string current = stack.top();
            stack.pop();
            
            // Skip if already processed
            if (neededPackages.find(current) != neededPackages.end()) {
                continue;
            }
            neededPackages.insert(current);
            
            // Add all dependencies to stack for processing
            vector<string> deps = getPackageDependencies(current);
            for (const auto& dep : deps) {
                stack.push(dep);
            }
        }
        
        // ===================================================================
        // PHASE 2: INDEGREE CALCULATION
        // ===================================================================
        // Calculate indegree for each package in the subgraph
        // Indegree[X] = number of packages that depend on X
        //
        // Time Complexity: O(V + E) - iterate through all packages and their deps
        // Space Complexity: O(V) - indegree map stores one entry per package
        
        unordered_map<string, int> indegree;
        
        // Initialize all indegrees to 0
        for (const auto& pkg : neededPackages) {
            indegree[pkg] = 0;
        }
        
        // Count incoming edges (how many packages depend on each package)
        for (const auto& pkg : neededPackages) {
            vector<string> deps = getPackageDependencies(pkg);
            for (const auto& dep : deps) {
                if (neededPackages.find(dep) != neededPackages.end()) {
                    indegree[pkg]++;  // pkg depends on dep, so pkg's indegree increases
                }
            }
        }
        
        // ===================================================================
        // PHASE 3: KAHN'S ALGORITHM (TOPOLOGICAL SORT)
        // ===================================================================
        // Process packages in topological order using BFS with queue
        //
        // Time Complexity: O(V + E) - each package processed once, each edge examined once
        // Space Complexity: O(V) - queue can hold up to V packages
        
        // Initialize queue with packages that have no dependencies (indegree = 0)
        queue<string> zeroIndegreeQueue;
        for (const auto& pair : indegree) {
            if (pair.second == 0) {
                zeroIndegreeQueue.push(pair.first);
            }
        }
        
        vector<string> result;  // Topological order result
        
        // Process packages in topological order
        while (!zeroIndegreeQueue.empty()) {
            // Remove a package with no dependencies
            string current = zeroIndegreeQueue.front();
            zeroIndegreeQueue.pop();
            result.push_back(current);  // Add to build order
            
            // Update indegrees of packages that depend on current package
            // When we "build" current package, it's no longer a dependency
            for (const auto& pkg : neededPackages) {
                vector<string> deps = getPackageDependencies(pkg);
                for (const auto& dep : deps) {
                    if (dep == current) {
                        indegree[pkg]--;  // Remove dependency
                        // If package now has no dependencies, it can be built
                        if (indegree[pkg] == 0) {
                            zeroIndegreeQueue.push(pkg);
                        }
                    }
                }
            }
        }
        
        // ===================================================================
        // PHASE 4: CYCLE DETECTION
        // ===================================================================
        // If we couldn't process all packages, there must be a cycle
        // Remaining packages all have indegree > 0, forming a cycle
        //
        // Time Complexity: O(1) - simple size comparison
        // Space Complexity: O(1) - no additional space needed
        
        if (result.size() != neededPackages.size()) {
            throw runtime_error("Circular dependency detected involving package '" + targetPackage + "'");
        }
        
        return result;
    }
};

/**
 * Main function demonstrating the dependency resolver
 * 
 * Tests both normal dependency resolution and cycle detection
 * Shows example usage and error handling
 */
int main() {
    try {
        // ===============================================================
        // TEST 1: NORMAL DEPENDENCY RESOLUTION
        // ===============================================================
        
        cout << "=== Testing Normal Dependency Resolution ===\n";
        DependencyResolver resolver;
        
        // Build the example dependency graph:
        //
        //     Service
        //    /   |   \
        // Adapters Core Utils
        //    |     |     |
        // Interfaces Types Types
        //
        // Expected build order: Types, Interfaces, then any valid combination
        // of Adapters/Core/Utils, finally Service
        
        resolver.addPackage("Service", {"Adapters", "Core", "Utils"});
        resolver.addPackage("Adapters", {"Interfaces"});
        resolver.addPackage("Core", {"Types"});
        resolver.addPackage("Utils", {"Types"});
        resolver.addPackage("Types");        // No dependencies
        resolver.addPackage("Interfaces");   // No dependencies
        
        // Get and display build order
        vector<string> buildOrder = resolver.getBuildOrder("Service");
        cout << "Build order for Service: ";
        for (size_t i = 0; i < buildOrder.size(); ++i) {
            cout << buildOrder[i];
            if (i < buildOrder.size() - 1) cout << " -> ";
        }
        cout << "\n\n";
        
        // ===============================================================
        // TEST 2: CYCLE DETECTION
        // ===============================================================
        
        cout << "=== Testing Cycle Detection ===\n";
        DependencyResolver cyclicResolver;
        
        // Create a circular dependency: A -> B -> C -> A
        cyclicResolver.addPackage("A", {"B"});
        cyclicResolver.addPackage("B", {"C"});
        cyclicResolver.addPackage("C", {"A"});  // Creates cycle
        
        try {
            cyclicResolver.getBuildOrder("A");
            cout << "ERROR: Cycle should have been detected!\n";
        } catch (const exception& e) {
            cout << "✓ Cycle correctly detected: " << e.what() << "\n\n";
        }
        
        // ===============================================================
        // TEST 3: EDGE CASES
        // ===============================================================
        
        cout << "=== Testing Edge Cases ===\n";
        
        // Test empty package name
        try {
            resolver.getBuildOrder("");
        } catch (const exception& e) {
            cout << "✓ Empty package name handled: " << e.what() << "\n";
        }
        
        // Test non-existent package
        try {
            resolver.getBuildOrder("NonExistent");
        } catch (const exception& e) {
            cout << "✓ Non-existent package handled: " << e.what() << "\n";
        }
        
        // Test self-dependency
        DependencyResolver selfDepResolver;
        selfDepResolver.addPackage("SelfDep", {"SelfDep"});
        try {
            selfDepResolver.getBuildOrder("SelfDep");
        } catch (const exception& e) {
            cout << "✓ Self-dependency handled: " << e.what() << "\n";
        }
        
    } catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

/*
 * ===============================================================================
 * PROBLEM STATEMENT
 * ===============================================================================
 * 
 * DEPENDENCY RESOLUTION AND BUILD ORDERING
 * 
 * Given a package name, return the build order of its package dependencies.
 * A dependency from package A to package B (A→B) means that package B must be 
 * built before package A.
 * 
 * EXAMPLE DEPENDENCY GRAPH:
 * ------------------------
 *                 --------
 *         +----------- Service -----------+
 *         |           --------           |
 *         |               |              |
 *         |               |              |
 *     ----V----      -----V-----     ----V----
 *     Adapters <---    Core       Utils
 *     --------      -----------   ----------
 *         |               |              |
 *         |               |              |
 *         |               |              |
 *     ----V------         +->   Types   <-+
 *     Interfaces              ----------
 *     ------------
 * 
 * REQUIREMENTS:
 * 1. Find valid build order for any target package
 * 2. Detect and report circular dependencies
 * 3. Handle edge cases (missing packages, self-dependencies, etc.)
 * 4. Efficient algorithm suitable for large dependency graphs
 * 
 * EXAMPLE INPUT/OUTPUT:
 * 
 * Input: "Service"
 * Output: ["Types", "Interfaces", "Adapters", "Utils", "Core", "Service"]
 * (Note: Multiple valid orders exist, e.g., "Utils" and "Core" can be swapped)
 * 
 * Input: Package with circular dependency
 * Output: Error indicating circular dependency detected
 * 
 * CONSTRAINTS:
 * - Package names are non-empty strings
 * - No self-dependencies allowed
 * - All referenced dependencies must exist
 * - Build order must satisfy all dependency constraints
 * 
 * REAL-WORLD APPLICATIONS:
 * - Build systems (Maven, Gradle, Make)
 * - Package managers (npm, pip, apt-get)
 * - Task scheduling with prerequisites
 * - Module loading in programming languages
 * - Database migration ordering
 * 
 * ===============================================================================
 */