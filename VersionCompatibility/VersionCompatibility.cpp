#include<bits/stdc++.h>
using namespace std;

/**
 * Version Compatibility Management System
 * 
 * Three different implementations:
 * 1. HashMap: O(n) time, stores direct compatibility flags
 * 2. DSU: O(α(n)) time, uses Union-Find for grouping compatible versions
 * 3. PrefixSum: O(1) time, uses group IDs for instant compatibility checks
 */

/**
 * Disjoint Set Union (Union-Find) Data Structure
 * 
 * Time Complexity:
 * - find(): O(α(n)) amortized (inverse Ackermann function - practically constant)
 * - unionBySize(): O(α(n)) amortized
 * - connected(): O(α(n)) amortized
 * 
 * Space Complexity: O(number of elements)
 * 
 * Features:
 * - Path compression: flattens tree during find operations
 * - Union by size: attaches smaller tree under root of larger tree
 */
class DSU {
private:
    unordered_map<int, int> parent; // parent[x] = parent of element x
    unordered_map<int, int> size;   // size[x] = size of tree rooted at x
    
public:
    /**
     * Finds the root of the set containing x with path compression
     * @param x: element to find root for
     * @return: root of the set containing x
     */
    int find(int x) {
        if (parent.find(x) == parent.end()) {
            parent[x] = x;
            size[x] = 1;
            return x;
        }
        if (parent[x] != x) {
            parent[x] = find(parent[x]); // path compression
        }
        return parent[x];
    }
    
    /**
     * Unites the sets containing x and y using union by size
     * @param x: first element
     * @param y: second element
     */
    void unionBySize(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        
        if (rootX != rootY) {
            if (size[rootX] < size[rootY]) {
                parent[rootX] = rootY;
                size[rootY] += size[rootX];
            } else {
                parent[rootY] = rootX;
                size[rootX] += size[rootY];
            }
        }
    }
    
    /**
     * Checks if x and y are in the same connected component
     * @param x: first element
     * @param y: second element
     * @return: true if x and y are connected, false otherwise
     */
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

/**
 * HashMap-based Version Management
 * 
 * Time Complexity:
 * - addNewVersion(): O(1)
 * - isCompatible(): O(targetVer - srcVer) - linear scan
 * 
 * Space Complexity: O(number of versions)
 * 
 * Description: Stores compatibility flag for each version with its predecessor.
 * For compatibility check, iterates through all intermediate versions.
 */
class VersionManagementHashMap {
private:
    unordered_map<int, bool> compatibilityMap; // stores compatibility with previous version
    
public:
    /**
     * Adds a new version with its compatibility status
     * @param ver: version number
     * @param isCompatibleWithPrev: whether this version is compatible with previous one
     */
    void addNewVersion(int ver, bool isCompatibleWithPrev) {
        compatibilityMap[ver] = isCompatibleWithPrev;
    }
    
    /**
     * Checks if upgrade from srcVer to targetVer is possible
     * @param srcVer: source version
     * @param targetVer: target version
     * @return: true if compatible upgrade possible, false otherwise
     */
    bool isCompatible(int srcVer, int targetVer) {
        // Same version is always compatible
        if (srcVer == targetVer) {
            return true;
        }
        
        // Downgrade is not supported
        if (srcVer > targetVer) {
            return false;
        }
        
        // Iterate through all versions from srcVer+1 to targetVer
        // All must be compatible with their previous version
        for (int ver = srcVer + 1; ver <= targetVer; ver++) {
            if (compatibilityMap.find(ver) == compatibilityMap.end() || !compatibilityMap[ver]) {
                return false;
            }
        }
        
        return true;
    }
};

/**
 * DSU-based Version Management
 * 
 * Time Complexity:
 * - addNewVersion(): O(α(n)) amortized
 * - isCompatible(): O(n + α(n)) where n is number of versions (for position finding + DSU check)
 * 
 * Space Complexity: O(number of versions)
 * 
 * Description: Groups compatible versions using Union-Find. When a version is compatible
 * with previous, it joins the current compatibility group. When incompatible, it starts
 * a new group. This approach handles non-sequential version numbers naturally.
 * 
 * Advantages:
 * - Works with any version numbering scheme (not just 1,2,3...)
 * - Clear grouping semantics
 * - Extensible for complex compatibility graphs
 */
class VersionManagementDSU {
private:
    DSU dsu;                    // Union-Find structure for grouping
    vector<int> versionOrder;   // maintains addition order for downgrade checking
    int currentGroupRoot;       // root of current compatibility group
    
public:
    VersionManagementDSU() : currentGroupRoot(-1) {}
    
    /**
     * Adds a new version and manages compatibility groups
     * @param ver: version number (can be non-sequential)
     * @param isCompatibleWithPrev: whether this version is compatible with previous one
     */
    void addNewVersion(int ver, bool isCompatibleWithPrev) {
        versionOrder.push_back(ver);
        
        if (versionOrder.size() == 1) {
            // First version starts its own group
            currentGroupRoot = ver;
        } else if (isCompatibleWithPrev) {
            // Compatible: add to current group using union by size
            dsu.unionBySize(ver, currentGroupRoot);
        } else {
            // Incompatible: start new group (breaks the compatibility chain)
            currentGroupRoot = ver;
        }
    }
    
    /**
     * Checks compatibility using DSU groups and position ordering
     * @param srcVer: source version
     * @param targetVer: target version
     * @return: true if compatible upgrade possible, false otherwise
     */
    bool isCompatible(int srcVer, int targetVer) {
        if (srcVer == targetVer) {
            return true;
        }
        
        // Find positions in addition order
        int srcPos = -1, targetPos = -1;
        for (int i = 0; i < versionOrder.size(); i++) {
            if (versionOrder[i] == srcVer) srcPos = i;
            if (versionOrder[i] == targetVer) targetPos = i;
        }
        
        if (srcPos == -1 || targetPos == -1) {
            return false; // Version not found
        }
        
        if (srcPos > targetPos) {
            return false; // No downgrade allowed
        }
        
        // Check if they are in the same compatibility group
        return dsu.connected(srcVer, targetVer);
    }
};

/**
 * PrefixSum-based Version Management (Group ID approach)
 * 
 * Time Complexity:
 * - addNewVersion(): O(1)
 * - isCompatible(): O(1) - constant time lookup!
 * 
 * Space Complexity: O(number of versions)
 * 
 * Description: Assigns group IDs to versions based on compatibility breaks.
 * Versions in the same group are compatible with each other.
 * Most efficient approach for sequential version management.
 * 
 * Advantages:
 * - Optimal O(1) time complexity for both operations
 * - Intuitive group-based approach
 * - Perfect for incremental version numbering
 */
class VersionManagementPrefixSum {
private:
    vector<int> groupID; // groupID[i] = compatibility group of version i+1
    int currentVersion = 0;
    int currentGroup = 1;
    
public:
    /**
     * Adds a new version with its compatibility status
     * @param ver: version number (should be sequential: 1, 2, 3, ...)
     * @param isCompatibleWithPrev: whether this version is compatible with previous one
     */
    void addNewVersion(int ver, bool isCompatibleWithPrev) {
        currentVersion++;
        
        if (currentVersion == 1) {
            // First version starts group 1
            groupID.push_back(1);
        } else if (isCompatibleWithPrev) {
            // Compatible: same group as previous version
            groupID.push_back(groupID.back());
        } else {
            // Incompatible: start new group
            currentGroup++;
            groupID.push_back(currentGroup);
        }
    }
    
    /**
     * Checks if upgrade from srcVer to targetVer is possible
     * @param srcVer: source version
     * @param targetVer: target version
     * @return: true if compatible upgrade possible, false otherwise
     */
    bool isCompatible(int srcVer, int targetVer) {
        // Same version is always compatible
        if (srcVer == targetVer) {
            return true;
        }
        
        // Downgrade is not supported
        if (srcVer > targetVer) {
            return false;
        }
        
        // Check bounds
        if (srcVer < 1 || targetVer > currentVersion) {
            return false;
        }
        
        // Compatible if both versions belong to the same group
        return groupID[srcVer - 1] == groupID[targetVer - 1];
    }
    
    /**
     * Debug function to print group assignments
     */
    void printGroups() {
        cout << "Version -> Group ID:\n";
        for (int i = 0; i < groupID.size(); i++) {
            cout << "v" << (i + 1) << " -> Group " << groupID[i] << "\n";
        }
        cout << "\n";
    }
};

int main() {
    cout << "Testing all three implementations...\n\n";
    
    // Test HashMap implementation
    cout << "Testing HashMap implementation (O(n) compatibility check):\n";
    VersionManagementHashMap hashMapVersion;
    hashMapVersion.addNewVersion(1, false);
    hashMapVersion.addNewVersion(2, true);
    hashMapVersion.addNewVersion(3, true);
    hashMapVersion.addNewVersion(4, false);
    hashMapVersion.addNewVersion(5, true);
    hashMapVersion.addNewVersion(6, true);
    
    assert(hashMapVersion.isCompatible(1, 3) == true);
    assert(hashMapVersion.isCompatible(3, 5) == false);
    assert(hashMapVersion.isCompatible(4, 2) == false);
    assert(hashMapVersion.isCompatible(3, 3) == true);
    cout << "HashMap tests passed!\n\n";

    // Test DSU implementation
    cout << "Testing DSU implementation (O(α(n)) compatibility check):\n";
    VersionManagementDSU dsuVersion;
    dsuVersion.addNewVersion(1, false);
    dsuVersion.addNewVersion(2, true);
    dsuVersion.addNewVersion(3, true);
    dsuVersion.addNewVersion(4, false);
    dsuVersion.addNewVersion(5, true);
    dsuVersion.addNewVersion(6, true);
    
    assert(dsuVersion.isCompatible(1, 3) == true);
    assert(dsuVersion.isCompatible(3, 5) == false);
    assert(dsuVersion.isCompatible(4, 2) == false);
    assert(dsuVersion.isCompatible(3, 3) == true);
    assert(dsuVersion.isCompatible(4, 6) == true);
    cout << "DSU tests passed!\n\n";
    
    // Test PrefixSum implementation
    cout << "Testing PrefixSum implementation (O(1) compatibility check):\n";
    VersionManagementPrefixSum prefixSumVersion;
    prefixSumVersion.addNewVersion(1, false);
    prefixSumVersion.addNewVersion(2, true);
    prefixSumVersion.addNewVersion(3, true);
    prefixSumVersion.addNewVersion(4, false);
    prefixSumVersion.addNewVersion(5, true);
    prefixSumVersion.addNewVersion(6, true);
    
    prefixSumVersion.printGroups();
    
    assert(prefixSumVersion.isCompatible(1, 3) == true);
    assert(prefixSumVersion.isCompatible(3, 5) == false);
    assert(prefixSumVersion.isCompatible(4, 2) == false);
    assert(prefixSumVersion.isCompatible(3, 3) == true);
    assert(prefixSumVersion.isCompatible(4, 6) == true);
    cout << "PrefixSum tests passed!\n\n";
    
    cout << "All three implementations work correctly!\n";
    cout << "\nComplexity Summary:\n";
    cout << "1. HashMap: O(n) compatibility check - simple but slower\n";
    cout << "2. DSU: O(α(n)) compatibility check - most flexible for any version scheme\n";
    cout << "3. PrefixSum: O(1) compatibility check - optimal for sequential versions\n";
    
    return 0;
}

/*
Problem Summary:
The storage team needs a version management system that tracks compatibility between versions.
Versions can be compatible (no data migration needed) or incompatible (data migration required).
Compatibility is transitive - if any step in the upgrade path is incompatible, the entire path is incompatible.

Test Case Analysis:
v1 -> v2 -> v3 -> v4 -> v5 -> v6
      T     T     F     T     T
1     1     1     2     2     2

This creates two compatibility groups:
- Group 1: {v1, v2, v3} - all can upgrade to each other
- Group 2: {v4, v5, v6} - all can upgrade to each other
- v3 cannot upgrade to v5 because v4 is incompatible with v3 (breaks the chain)

Expected Results:
- isCompatible(1, 3) = true  (same group)
- isCompatible(3, 5) = false (different groups due to incompatible v4)
- isCompatible(4, 2) = false (downgrade not allowed)
- isCompatible(3, 3) = true  (same version)
- isCompatible(4, 6) = true  (same group)

PrefixSum Approach Details:
For versions: v1 -> v2(T) -> v3(T) -> v4(F) -> v5(T) -> v6(T)

Group Assignment:
v1: Group 1 (first version)
v2: Group 1 (compatible with v1)
v3: Group 1 (compatible with v2)
v4: Group 2 (incompatible with v3, starts new group)
v5: Group 2 (compatible with v4)
v6: Group 2 (compatible with v5)

The PrefixSum approach provides O(1) compatibility checks by simply comparing group IDs!
*/