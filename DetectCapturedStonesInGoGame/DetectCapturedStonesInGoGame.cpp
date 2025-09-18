#include <iostream>
#include <vector>
#include <queue>
#include <set>
using namespace std;

enum FieldValue { WHITE, BLACK, EMPTY };

class Board {
private:
    vector<vector<FieldValue>> grid;
    int size;

public:
    Board(int boardSize) : size(boardSize) {
        grid = vector<vector<FieldValue>>(size, vector<FieldValue>(size, EMPTY));
    }
    
    FieldValue getValue(int x, int y) {
        if (x < 0 || y < 0 || x >= size || y >= size) {
            return EMPTY; // Borderless - treat out-of-bounds as empty
        }
        return grid[x][y];
    }
    
    void setValue(int x, int y, FieldValue value) {
        if (x >= 0 && y >= 0 && x < size && y < size) {
            grid[x][y] = value;
        }
    }
    
    void clear() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                grid[i][j] = EMPTY;
            }
        }
    }
};

class StoneCaptureChecker {
private:
    Board* board;
    
    // 4 directions: up, down, left, right
    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};

public:
    StoneCaptureChecker(Board* gameBoard) : board(gameBoard) {}
    
    // Main function to check if stone/group is captured
    bool isCaptured(int x, int y) {
        FieldValue stoneColor = board->getValue(x, y);
        
        // Empty position cannot be captured
        if (stoneColor == EMPTY) {
            return false;
        }
        
        // BFS to find all connected stones and check for escape
        queue<pair<int, int>> toVisit;
        set<pair<int, int>> visited;
        
        toVisit.push({x, y});
        
        while (!toVisit.empty()) {
            auto [currentX, currentY] = toVisit.front();
            toVisit.pop();
            
            // Skip if already visited
            if (visited.count({currentX, currentY})) {
                continue;
            }
            
            visited.insert({currentX, currentY});
            
            // Check all 4 neighbors
            for (int i = 0; i < 4; i++) {
                int newX = currentX + dx[i];
                int newY = currentY + dy[i];
                
                FieldValue neighborValue = board->getValue(newX, newY);
                
                // Found empty space - group can escape!
                if (neighborValue == EMPTY) {
                    return false;
                }
                
                // Found same color stone - add to queue if not visited
                if (neighborValue == stoneColor && !visited.count({newX, newY})) {
                    toVisit.push({newX, newY});
                }
                
                // Different color stones block escape (do nothing)
            }
        }
        
        // No empty space found = group is captured
        return true;
    }
};

// Test the implementation
int main() {
    Board gameBoard(10);
    StoneCaptureChecker checker(&gameBoard);
    
    cout << "=== Stone Capture Tests ===\n" << endl;
    
    // Test 1: Single captured stone
    cout << "Test 1: Single white stone surrounded by black" << endl;
    gameBoard.clear();
    gameBoard.setValue(5, 5, WHITE);  // Center stone
    gameBoard.setValue(4, 5, BLACK);  // Left
    gameBoard.setValue(6, 5, BLACK);  // Right
    gameBoard.setValue(5, 4, BLACK);  // Up
    gameBoard.setValue(5, 6, BLACK);  // Down
    
    cout << "Result: " << (checker.isCaptured(5, 5) ? "CAPTURED" : "FREE") << endl;
    cout << endl;
    
    // Test 2: Connected group with escape
    cout << "Test 2: White group with escape route" << endl;
    gameBoard.clear();
    gameBoard.setValue(5, 5, WHITE);
    gameBoard.setValue(5, 6, WHITE);  // Connected white stones
    gameBoard.setValue(4, 5, BLACK);
    gameBoard.setValue(6, 5, BLACK);
    gameBoard.setValue(5, 4, BLACK);
    gameBoard.setValue(4, 6, BLACK);
    gameBoard.setValue(6, 6, BLACK);
    // Position (5,7) is EMPTY - escape route exists
    
    cout << "Result: " << (checker.isCaptured(5, 5) ? "CAPTURED" : "FREE") << endl;
    cout << endl;
    
    // Test 3: Connected group fully captured
    cout << "Test 3: White group fully captured" << endl;
    gameBoard.clear();
    gameBoard.setValue(5, 5, WHITE);
    gameBoard.setValue(5, 6, WHITE);
    gameBoard.setValue(4, 5, BLACK);
    gameBoard.setValue(6, 5, BLACK);
    gameBoard.setValue(5, 4, BLACK);
    gameBoard.setValue(4, 6, BLACK);
    gameBoard.setValue(6, 6, BLACK);
    gameBoard.setValue(5, 7, BLACK);  // Block escape route
    
    cout << "Result: " << (checker.isCaptured(5, 5) ? "CAPTURED" : "FREE") << endl;
    
    return 0;
}

/*
=== PROBLEM STATEMENT ===

The goal of this interview question is to determine if a stone at a given position (x,y) 
on a borderless (and potentially very large) board is entirely captured by "enemy" stones. 

"Captured" in this case means directly surrounded by stones of the opposite color that 
share an edge. If there are any blank fields, the stone(s) can be considered free. 

The problem is almost identical to building a Paintbrush "fill" function.

In the most simple case (for a whiteboard question I would recommend starting with that), 
it would be a single stone surrounded by four stones of the opposite color.

Method to be implemented:
public boolean isCaptured(int x, int y) { ... }

You can assume that there is a board that enables you to check the field value as well 
as some Tri-State field value:
FieldValue val = Board.getValue(x,y);
public enum FieldValue { WHITE, BLACK, EMPTY; }

The candidate should be able to solve the question in three steps and levels of complexity:

1. Determine if a single stone is captured.
   - This is the most trivial example, no candidate should fail to solve this in code

2. Determine if a connected group of same-colored stones is captured.
   - Is the solution valid, does it always finish (circular references) and does it scale?
   - Does the candidate use the right data structures for the job (O(n) lookup for visited stones,...)

3. Given infinite computing power and a shared memory, how would you solve this problem 
   for billions of connected stones?

SOLUTION GUIDANCE:
1. Level 1: Simple hardcoded check of 4 adjacent cells (2-5 minutes)
2. Level 2: Use BFS (preferred) over DFS/recursion for better scalability
   - BFS avoids stack overflow, easier to parallelize, better cycle detection
   - Use Queue for "to check" and Set for "visited" stones
3. Level 3: Discuss threading, race conditions, distributed processing (no coding)

Key insight: This is essentially a flood-fill algorithm to find connected components
and check if any stone in the component can reach an empty space.
*/