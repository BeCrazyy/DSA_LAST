/**
 * QUADTREE IMPLEMENTATION WITH DETAILED DRY RUN EXAMPLE
 * 
 * This code implements a quadtree for image compression and includes
 * a complete step-by-step dry run to show exactly how the algorithm works.
 */

#include <iostream>
#include <vector>
using namespace std;

/**
 * Simple QuadTree Node Structure
 */
struct QuadTreeNode {
    int value;                    // Pixel value (only used for leaves)
    bool isLeaf;                  // true = leaf, false = internal node
    QuadTreeNode* topLeft;        // Pointer to top-left child
    QuadTreeNode* topRight;       // Pointer to top-right child
    QuadTreeNode* bottomLeft;     // Pointer to bottom-left child
    QuadTreeNode* bottomRight;    // Pointer to bottom-right child
    
    // Constructor for leaf node
    QuadTreeNode(int val) {
        value = val;
        isLeaf = true;
        topLeft = topRight = bottomLeft = bottomRight = nullptr;
    }
    
    // Constructor for internal node
    QuadTreeNode(QuadTreeNode* tl, QuadTreeNode* tr, QuadTreeNode* bl, QuadTreeNode* br) {
        value = 0;  // Not used for internal nodes
        isLeaf = false;
        topLeft = tl;
        topRight = tr;
        bottomLeft = bl;
        bottomRight = br;
    }
};

/**
 * Build quadtree for a rectangular region
 */
QuadTreeNode* buildQuadTree(vector<vector<int>>& img, int x1, int x2, int y1, int y2) {
    // Base case: single pixel
    if (x1 == x2 && y1 == y2) {
        return new QuadTreeNode(img[x1][y1]);
    }
    
    // Calculate midpoints to split the region
    int midX = (x1 + x2) / 2;
    int midY = (y1 + y2) / 2;
    
    // Recursively build 4 child quadrants
    QuadTreeNode* tl = buildQuadTree(img, x1, midX, y1, midY);
    QuadTreeNode* tr = buildQuadTree(img, x1, midX, midY + 1, y2);
    QuadTreeNode* bl = buildQuadTree(img, midX + 1, x2, y1, midY);
    QuadTreeNode* br = buildQuadTree(img, midX + 1, x2, midY + 1, y2);
    
    // Check if all 4 children are leaves with the same value
    if (tl->isLeaf && tr->isLeaf && bl->isLeaf && br->isLeaf &&
        tl->value == tr->value && tl->value == bl->value && tl->value == br->value) {
        
        // All children are identical - merge them into one leaf
        int commonValue = tl->value;
        delete tl;
        delete tr;
        delete bl;
        delete br;
        return new QuadTreeNode(commonValue);
    }
    
    // Children are different - create internal node
    return new QuadTreeNode(tl, tr, bl, br);
}

/**
 * Wrapper function to start the quadtree building
 */
QuadTreeNode* makeQuadTree(vector<vector<int>>& img) {
    if (img.empty() || img[0].empty()) {
        return nullptr;
    }
    return buildQuadTree(img, 0, img.size() - 1, 0, img[0].size() - 1);
}

/**
 * Print the quadtree structure
 */
void printTree(QuadTreeNode* node, string prefix = "") {
    if (!node) {
        cout << prefix << "NULL" << endl;
        return;
    }
    
    if (node->isLeaf) {
        cout << prefix << "LEAF(" << node->value << ")" << endl;
    } else {
        cout << prefix << "INTERNAL" << endl;
        cout << prefix << "├─topLeft: ";
        printTree(node->topLeft, prefix + "│ ");
        cout << prefix << "├─topRight: ";
        printTree(node->topRight, prefix + "│ ");
        cout << prefix << "├─bottomLeft: ";
        printTree(node->bottomLeft, prefix + "│ ");
        cout << prefix << "└─bottomRight: ";
        printTree(node->bottomRight, prefix + "  ");
    }
}

/**
 * Clean up memory
 */
void deleteTree(QuadTreeNode* node) {
    if (!node) return;
    if (!node->isLeaf) {
        deleteTree(node->topLeft);
        deleteTree(node->topRight);
        deleteTree(node->bottomLeft);
        deleteTree(node->bottomRight);
    }
    delete node;
}

/**
 * DETAILED DRY RUN EXAMPLE
 * 
 * Let's trace through this 2x2 image step by step:
 * 
 * IMAGE:
 * [A, B]
 * [A, A]
 * 
 * Where A=1, B=2
 * So the actual image is:
 * [1, 2]
 * [1, 1]
 */
void dryRunExample() {
    cout << "\n" << string(80, '=') << endl;
    cout << "                    DETAILED DRY RUN EXAMPLE" << endl;
    cout << string(80, '=') << endl;
    
    vector<vector<int>> img = {
        {1, 2},
        {1, 1}
    };
    
    cout << "\nInput Image (2x2):" << endl;
    cout << "┌───┬───┐" << endl;
    cout << "│ " << img[0][0] << " │ " << img[0][1] << " │" << endl;
    cout << "├───┼───┤" << endl;
    cout << "│ " << img[1][0] << " │ " << img[1][1] << " │" << endl;
    cout << "└───┴───┘" << endl;
    
    cout << "\nSTEP-BY-STEP EXECUTION:" << endl;
    cout << "=======================" << endl;
    
    cout << "\nCALL 1: buildQuadTree(img, x1=0, x2=1, y1=0, y2=1)" << endl;
    cout << "  - Processing entire 2x2 image" << endl;
    cout << "  - Check: Is it 1x1? (0==1 && 0==1) → NO" << endl;
    cout << "  - Calculate midpoints: midX = (0+1)/2 = 0, midY = (0+1)/2 = 0" << endl;
    cout << "  - Split into 4 quadrants:" << endl;
    
    cout << "\n  CALL 2: topLeft = buildQuadTree(img, 0, 0, 0, 0)" << endl;
    cout << "    - Processing img[0][0] = " << img[0][0] << endl;
    cout << "    - Check: Is it 1x1? (0==0 && 0==0) → YES" << endl;
    cout << "    - Return: LEAF(" << img[0][0] << ")" << endl;
    
    cout << "\n  CALL 3: topRight = buildQuadTree(img, 0, 0, 1, 1)" << endl;
    cout << "    - Processing img[0][1] = " << img[0][1] << endl;
    cout << "    - Check: Is it 1x1? (0==0 && 1==1) → YES" << endl;
    cout << "    - Return: LEAF(" << img[0][1] << ")" << endl;
    
    cout << "\n  CALL 4: bottomLeft = buildQuadTree(img, 1, 1, 0, 0)" << endl;
    cout << "    - Processing img[1][0] = " << img[1][0] << endl;
    cout << "    - Check: Is it 1x1? (1==1 && 0==0) → YES" << endl;
    cout << "    - Return: LEAF(" << img[1][0] << ")" << endl;
    
    cout << "\n  CALL 5: bottomRight = buildQuadTree(img, 1, 1, 1, 1)" << endl;
    cout << "    - Processing img[1][1] = " << img[1][1] << endl;
    cout << "    - Check: Is it 1x1? (1==1 && 1==1) → YES" << endl;
    cout << "    - Return: LEAF(" << img[1][1] << ")" << endl;
    
    cout << "\nBack to CALL 1: Check if we can merge the 4 children" << endl;
    cout << "  - topLeft = LEAF(" << img[0][0] << ")" << endl;
    cout << "  - topRight = LEAF(" << img[0][1] << ")" << endl;
    cout << "  - bottomLeft = LEAF(" << img[1][0] << ")" << endl;
    cout << "  - bottomRight = LEAF(" << img[1][1] << ")" << endl;
    
    cout << "\n  Merge check:" << endl;
    cout << "  - Are all leaves? " << img[0][0] << "✓, " << img[0][1] << "✓, " << img[1][0] << "✓, " << img[1][1] << "✓ → YES" << endl;
    cout << "  - Same values? " << img[0][0] << "==" << img[0][1] << " && " << img[0][0] << "==" << img[1][0] << " && " << img[0][0] << "==" << img[1][1] << endl;
    cout << "  - " << (img[0][0] == img[0][1] ? "TRUE" : "FALSE") << " && " << (img[0][0] == img[1][0] ? "TRUE" : "FALSE") << " && " << (img[0][0] == img[1][1] ? "TRUE" : "FALSE") << endl;
    
    bool canMerge = (img[0][0] == img[0][1] && img[0][0] == img[1][0] && img[0][0] == img[1][1]);
    
    if (canMerge) {
        cout << "  - RESULT: Can merge! Delete 4 children, return LEAF(" << img[0][0] << ")" << endl;
    } else {
        cout << "  - RESULT: Cannot merge! Return INTERNAL node with 4 children" << endl;
    }
    
    cout << "\nFINAL TREE STRUCTURE:" << endl;
    cout << "=====================" << endl;
    QuadTreeNode* root = makeQuadTree(img);
    printTree(root);
    
    cout << "\nEXPLANATION:" << endl;
    if (canMerge) {
        cout << "All pixels have the same value, so the entire image is compressed into one leaf node!" << endl;
    } else {
        cout << "The pixels have different values, so we need an internal node with 4 children." << endl;
        cout << "This shows how the quadtree adapts to the image content." << endl;
    }
    
    deleteTree(root);
}

/**
 * MAIN FUNCTION WITH EXAMPLES
 */
int main() {
    cout << "QUADTREE IMPLEMENTATION DEMO" << endl;
    cout << "============================" << endl;
    
    // Run the detailed dry run example
    dryRunExample();
    
    cout << "\n\n" << string(80, '=') << endl;
    cout << "                    ADDITIONAL TEST CASES" << endl;
    cout << string(80, '=') << endl;
    
    // Test Case 1: All same values (maximum compression)
    cout << "\nTest 1: Uniform Image (all 5's)" << endl;
    vector<vector<int>> uniform = {{5, 5}, {5, 5}};
    cout << "Input: [5,5] [5,5]" << endl;
    QuadTreeNode* root1 = makeQuadTree(uniform);
    cout << "Result: ";
    printTree(root1);
    deleteTree(root1);
    
    // Test Case 2: All different values (no compression)
    cout << "\nTest 2: All Different Values" << endl;
    vector<vector<int>> different = {{1, 2}, {3, 4}};
    cout << "Input: [1,2] [3,4]" << endl;
    QuadTreeNode* root2 = makeQuadTree(different);
    cout << "Result: ";
    printTree(root2);
    deleteTree(root2);
    
    // Test Case 3: Larger example
    cout << "\nTest 3: 4x4 Image with Mixed Regions" << endl;
    vector<vector<int>> large = {
        {1, 1, 2, 2},
        {1, 1, 2, 2},
        {3, 4, 5, 5},
        {6, 7, 5, 5}
    };
    cout << "Input:" << endl;
    for (auto& row : large) {
        cout << "[";
        for (int i = 0; i < row.size(); i++) {
            cout << row[i];
            if (i < row.size() - 1) cout << ",";
        }
        cout << "] ";
    }
    cout << endl;
    QuadTreeNode* root3 = makeQuadTree(large);
    cout << "Result: ";
    printTree(root3);
    deleteTree(root3);
    
    return 0;
}

/*
================================================================================
                              PROBLEM DESCRIPTION
================================================================================

QUADTREE DATA STRUCTURE FOR IMAGE COMPRESSION

A quadtree is a tree where each internal node has exactly 4 children, representing
the 4 quadrants of a 2D region: topLeft, topRight, bottomLeft, bottomRight.

ALGORITHM:
1. If the region is 1x1, create a leaf node with that pixel value
2. Otherwise, split the region into 4 equal quadrants
3. Recursively build subtrees for each quadrant
4. If all 4 subtrees are leaves with the same value, merge them into one leaf
5. Otherwise, create an internal node with the 4 subtrees as children

TIME COMPLEXITY: O(n) where n is the number of pixels
SPACE COMPLEXITY: O(1) to O(n) depending on image complexity

BENEFITS:
- Efficient compression for images with large uniform regions
- Adaptive representation (more detail where needed)
- Fast spatial queries

================================================================================
*/