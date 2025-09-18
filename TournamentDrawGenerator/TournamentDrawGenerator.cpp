/**
 * Tennis Tournament Simulator
 * 
 * This program simulates elimination tournaments with proper seeding algorithms.
 * It includes two main functionalities:
 * 1. Basic tournament simulation with given player rankings
 * 2. Seeded draw generation to ensure fair matchups
 * 
 * Author: Tournament Simulator
 * Date: 2025
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

/**
 * Part 1: Predict winners of each round in an elimination tournament
 * 
 * This function simulates one round of matches where adjacent players compete.
 * The player with lower rank number (higher actual rank) always wins.
 * 
 * Example:
 * Input: [1, 2, 3, 4, 5, 6, 7, 8]
 * Pairs: (1 vs 2), (3 vs 4), (5 vs 6), (7 vs 8)
 * Winners: 1 < 2 → 1 wins, 3 < 4 → 3 wins, 5 < 6 → 5 wins, 7 < 8 → 7 wins
 * Output: [1, 3, 5, 7]
 * 
 * BYE Example:
 * Input: [1, 5, 3, -1] where -1 is BYE
 * Pairs: (1 vs 5), (3 vs BYE)
 * Winners: 1 wins against 5, 3 gets automatic advancement against BYE
 * Output: [1, 3]
 * 
 * Algorithm:
 * 1. Pair adjacent players in the current round
 * 2. Compare ranks and select winner (lower number wins)
 * 3. Handle BYEs when odd number of players
 * 4. Return winners for next round
 * 
 * Time Complexity: O(n) where n is number of players in current round
 * Space Complexity: O(n/2) for storing winners of current round
 * 
 * @param candidates Vector of player ranks in current round (-1 represents BYE)
 * @return Vector of winners advancing to next round
 */
vector<int> predictWinners(const vector<int>& candidates) {
    if (candidates.size() <= 1) {
        return vector<int>();
    }
    
    vector<int> winners;
    
    for (size_t i = 0; i < candidates.size(); i += 2) {
        int left = candidates[i];
        int right = (i + 1 < candidates.size()) ? candidates[i + 1] : -1;
        
        // Handle BYE cases
        if (left == -1) {
            winners.push_back(right);
        } else if (right == -1) {
            winners.push_back(left);
        } else {
            // Lower number = higher rank = winner
            winners.push_back(min(left, right));
        }
    }
    return winners;
}

/**
 * Expand tournament bracket to next power of 2
 * 
 * This is the core seeding algorithm that maintains proper separation
 * between seeds. Each existing player gets paired with a new player
 * at maximum distance in the bracket.
 * 
 * Example:
 * Input: [1, 2] (current bracket size = 2)
 * Process:
 *   i=0: expandedDraw[0] = baseDraw[0] = 1, expandedDraw[1] = 1 + 2 = 3
 *   i=1: expandedDraw[2] = baseDraw[1] = 2, expandedDraw[3] = 2 + 2 = 4
 * Output: [1, 3, 2, 4]
 * 
 * Intuition: We're placing players so that the best players (1,2) are 
 * maximally separated and can only meet in the final.
 * 
 * Pattern:
 * - Existing players stay at even indices (0, 2, 4, 6, ...)
 * - New players go to odd indices (1, 3, 5, 7, ...)
 * - New player rank = existing_player_rank + current_bracket_size
 * 
 * Time Complexity: O(n) where n is current bracket size
 * Space Complexity: O(2n) for expanded bracket
 * 
 * @param baseDraw Current tournament bracket
 * @return Expanded bracket with double the size
 */
vector<int> expandDraw(const vector<int>& baseDraw) {
    vector<int> expandedDraw(baseDraw.size() * 2);
    
    for (size_t i = 0; i < baseDraw.size(); i++) {
        // Keep existing player at even index
        expandedDraw[i * 2] = baseDraw[i];
        // Add new player at odd index with calculated rank
        expandedDraw[i * 2 + 1] = baseDraw[i] + (int)baseDraw.size();
    }
    return expandedDraw;
}

/**
 * Part 2: Generate seeded tournament draw
 * 
 * Creates a proper tournament bracket where higher-ranked players are separated
 * to avoid early elimination by lower-ranked players. Uses recursive expansion
 * starting from the top seed.
 * 
 * Example for n=6 (non-power of 2):
 * Step 1: base = [1]
 * Step 2: base = expandDraw([1]) = [1, 2]
 * Step 3: base = expandDraw([1, 2]) = [1, 3, 2, 4]
 * Step 4: base = expandDraw([1, 3, 2, 4]) = [1, 5, 3, 7, 2, 6, 4, 8]
 * Step 5: Replace players > 6 with BYEs: [1, 5, 3, -1, 2, 6, 4, -1]
 *         (players 7 and 8 don't exist, so they become BYEs)
 * 
 * Result: [1, 5, 3, -1, 2, 6, 4, -1]
 * Notice: Players 3 and 4 get automatic advancement (BYE) in round 1
 * 
 * WHY BYEs ARE NEEDED:
 * Tournament brackets must be powers of 2 (2, 4, 8, 16...) for elimination to work.
 * If you have 6 players, you need an 8-slot bracket, so 2 slots become BYEs.
 * BYEs give some players automatic advancement to balance the bracket.
 * 
 * Algorithm:
 * 1. Start with top seed [1]
 * 2. Recursively expand bracket size by powers of 2
 * 3. Place new seeds to maximize separation from existing seeds
 * 4. Fill remaining slots with BYEs if needed
 * 
 * Time Complexity: O(n log n) where n is final bracket size
 *   - log n iterations of expansion
 *   - Each expansion takes O(current_size) time
 * Space Complexity: O(n) for final bracket
 * 
 * @param n Number of actual players in tournament
 * @return Vector representing seeded tournament bracket
 */
vector<int> generateDraw(int n) {
    vector<int> base = {1}; // Start with top seed
    
    // Expand bracket until it can accommodate n players
    while ((int)base.size() < n) {
        base = expandDraw(base);
    }
    
    // Replace players beyond n with BYEs
    // Example: if n=6 but bracket is [1,5,3,7,2,6,4,8], then players 7,8 don't exist
    // So replace them with BYE (-1): [1,5,3,-1,2,6,4,-1]
    // This means player 3 faces BYE (automatic win) and player 4 faces BYE (automatic win)
    for (int i = 0; i < (int)base.size(); i++) {
        if (base[i] > n) {
            base[i] = -1; // -1 represents BYE (no player, automatic advancement)
        }
    }
    return base;
}

/**
 * Main function demonstrating both parts of the tournament system
 * 
 * Shows the difference between basic sequential brackets and properly
 * seeded brackets, including handling of non-power-of-2 tournaments.
 * 
 * Demonstrates:
 * 1. Basic tournament simulation with [1,2,3,4,5,6,7,8] arrangement
 *    - Shows how adjacent pairing works in simple elimination
 *    - Player 1 and 2 meet in round 2 (too early for top seeds)
 * 
 * 2. Seeded tournament with 8 players showing proper separation
 *    - Shows how seeding keeps top players apart until finals
 *    - Player 1 and 2 are in different bracket halves
 * 
 * 3. Seeded tournament with 6 players showing BYE handling
 *    - Shows how non-power-of-2 tournaments work with BYEs
 *    - Some players get automatic advancement in round 1
 * 
 * The BYE example illustrates how tournaments handle non-power-of-2 
 * participant counts by giving some players automatic advancement.
 * This is exactly how real tournaments (like tennis, basketball) work.
 */
int main() {
    cout << "=== Part 1: Basic Tournament Simulation ===" << endl;
    vector<int> draw = {1, 2, 3, 4, 5, 6, 7, 8};
    
    while (draw.size() >= 1) {
        for (int player : draw) {
            cout << (player == -1 ? "BYE" : to_string(player)) << " ";
        }
        cout << endl;
        draw = predictWinners(draw);
    }
    
    cout << "\n=== Part 2: Seeded Tournament Draw (8 players) ===" << endl;
    draw = generateDraw(8);
    
    while (draw.size() >= 1) {
        for (int player : draw) {
            cout << (player == -1 ? "BYE" : to_string(player)) << " ";
        }
        cout << endl;
        draw = predictWinners(draw);
    }
    
    cout << "\n=== Part 2: Non-power of 2 example (6 players) ===" << endl;
    draw = generateDraw(6);
    
    while (draw.size() >= 1) {
        for (int player : draw) {
            cout << (player == -1 ? "BYE" : to_string(player)) << " ";
        }
        cout << endl;
        draw = predictWinners(draw);
    }
    
    return 0;
}

/*
===============================================================================
PROBLEM STATEMENT
===============================================================================

You are hosting a tennis tournament with many players participating. All your 
players are pre-ranked based on their historical performance. In an elimination 
tournament, the loser of each match-up is immediately eliminated from the 
tournament. Each winner will play another in the next round, until the final 
match-up, whose winner becomes the tournament champion.

PART 1:
Can you write a program to predict the players in each round? Your program 
should naively assume that a higher ranked player always wins against a lower 
ranked player.

Input: [1, 2, 3, 4, 5, 6, 7, 8] 
// ranks of each player and the match ups. The smaller the number, the higher the rank.

Output:
1, 2, 3, 4, 5, 6, 7, 8  // round 1
1, 3, 5, 7              // round 2  
1, 5                    // round 3
1                       // round 4 (champion)

Bracket visualization:
Round1  Round2  Round3  Round4
  1  ----\
           1 ----\
  2  ----/        \
                   1 ----\
  3  ----\        /       \
           3 ----/         1 (Champion)
  4  ----/                /
                         /
  5  ----\              /
           5 ----------/
  6  ----/
  
  7  ----\
           7
  8  ----/

PART 2:
Can you create a draw generator that makes sure the better players do not meet 
in the earlier rounds? To be more specific, a higher-ranked player should never 
be eliminated in an earlier round than someone ranked lower according to your 
prediction in part 1.

Input: 8 (total number of players)
Output: A possible seeded draw arrangement

POTENTIAL FOLLOW-UP QUESTIONS:
• How do we make the generator work when participants count is not a power of 2?
• How will you modify your solution to output all possible draws?

SOLUTION GUIDANCE:
Part 1 is quite basic and shouldn't need any guidance.
Part 2 can be solved easily if working backwards from later rounds to earlier rounds.

ALGORITHM EXPLANATION WITH STEP-BY-STEP EXAMPLE:
The key insight for Part 2 is that we want top seeds to be maximally separated.
Starting with [1], we recursively expand:

DETAILED SEEDING PROCESS FOR 8 PLAYERS:

Step 1: Start with top seed
base = [1]

Step 2: Expand to accommodate 2 players
expandDraw([1]):
- i=0: expandedDraw[0] = 1, expandedDraw[1] = 1 + 1 = 2
base = [1, 2]

Step 3: Expand to accommodate 4 players  
expandDraw([1, 2]):
- i=0: expandedDraw[0] = 1, expandedDraw[1] = 1 + 2 = 3
- i=1: expandedDraw[2] = 2, expandedDraw[3] = 2 + 2 = 4
base = [1, 3, 2, 4]

Step 4: Expand to accommodate 8 players
expandDraw([1, 3, 2, 4]):
- i=0: expandedDraw[0] = 1, expandedDraw[1] = 1 + 4 = 5
- i=1: expandedDraw[2] = 3, expandedDraw[3] = 3 + 4 = 7  
- i=2: expandedDraw[4] = 2, expandedDraw[5] = 2 + 4 = 6
- i=3: expandedDraw[6] = 4, expandedDraw[7] = 4 + 4 = 8
base = [1, 5, 3, 7, 2, 6, 4, 8]

FINAL SEEDED BRACKET: [1, 5, 3, 7, 2, 6, 4, 8]

WHY THIS WORKS:
- Seeds 1 and 2 are at positions 0 and 4 (different halves) → can only meet in finals
- Seeds 3 and 4 are at positions 2 and 6 (different halves) → can only meet in finals  
- Seeds 1 vs 3 and 2 vs 4 can only meet in semifinals
- Higher seeds are protected from early elimination

BRACKET VISUALIZATION:
Round 1: [1, 5, 3, 7, 2, 6, 4, 8]
Matches: (1 vs 5), (3 vs 7), (2 vs 6), (4 vs 8)
Winners: [1, 3, 2, 4]

Round 2: [1, 3, 2, 4] 
Matches: (1 vs 3), (2 vs 4)
Winners: [1, 2]

Round 3: [1, 2]
Match: (1 vs 2)
Champion: 1

BYE HANDLING EXAMPLE (6 PLAYERS):
When n=6, we need an 8-slot bracket, so 2 become BYEs:
Generated bracket: [1, 5, 3, -1, 2, 6, 4, -1]
Round 1 matches: (1 vs 5), (3 vs BYE), (2 vs 6), (4 vs BYE)
Winners: [1, 3, 2, 4] (players 3 and 4 advance automatically)

This is exactly how real tournaments work - some players get "byes" in early rounds
when the number of participants isn't a perfect power of 2.

TIME/SPACE COMPLEXITY SUMMARY:
- Tournament Simulation: O(n) time, O(n) space
- Seeded Draw Generation: O(n log n) time, O(n) space
- Overall System: O(n log n) time, O(n) space

===============================================================================
*/