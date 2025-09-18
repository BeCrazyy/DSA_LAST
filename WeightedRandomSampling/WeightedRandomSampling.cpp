#include<bits/stdc++.h>
#include<iomanip>

using namespace std;

class RandomSelection {
    public:
        RandomSelection() {}

        vector<string> weightedRandomSelection(vector<pair<string, int>> segments, int N) {
            vector<string> result;
            if(segments.size() == 0) {
                return result;
            }

            vector<int>weights = vector<int>({segments[0].second});
            for(int i=1; i<segments.size(); i++) {
                weights.push_back(weights.back() + segments[i].second);
            }
            int totalWeight = weights.back();

            for(int i=0; i<N; i++) {
                int randomIndex = rand() % totalWeight;
                auto it = upper_bound(weights.begin(), weights.end(), randomIndex);
                // Remove this line - upper_bound should never return end() for valid input
                if(it == weights.end()) it--;
                int index = it - weights.begin();
                result.push_back(segments[index].first);
            }
            return result;
        }
};

int main() {
    srand(time(0)); // Seed for random number generation
    
    RandomSelection rs;
    
    // Test case 1: From the problem example
    vector<pair<string, int>> segments1 = {
        {"rider_pool", 2}, 
        {"vip_rider", 3}, 
        {"premier", 5}
    };
    
    cout << "Test case 1:" << endl;
    cout << "Segments: [rider_pool(2), vip_rider(3), premier(5)]" << endl;
    vector<string> result1 = rs.weightedRandomSelection(segments1, 10);
    cout << "Result: [";
    for(int i = 0; i < result1.size(); i++) {
        cout << "\"" << result1[i] << "\"";
        if(i < result1.size() - 1) cout << ", ";
    }
    cout << "]" << endl << endl;
    
    // Test to verify distribution over many runs
    cout << "Distribution test (1000 runs):" << endl;
    map<string, int> counts;
    for(int run = 0; run < 1000; run++) {
        vector<string> sample = rs.weightedRandomSelection(segments1, 1);
        counts[sample[0]]++;
    }
    
    for(auto& p : counts) {
        cout << p.first << ": " << p.second << " (" << (p.second/10.0) << "%)" << endl;
    }
    
    return 0;
}

/*

Problem Statement:
You need to implement a weighted random selection algorithm for user segments. The problem involves:

Input: A list of pairs, where each pair consists of:

A user segment label (e.g., "new_rider", "loyal_driver", "vip_rider")
A weight, which indicates how likely that segment should be selected


Task: Write a function that randomly selects and returns n segment labels, 
such that the probability of selecting each label is proportional to its weight. 
Each selection should be independent, and sampling is done with replacement.

Example Input:
segments = [("rider_pool", 2), ("vip_rider", 3), ("premier", 5)]
n = 4
Expected Output:
A list of n randomly selected labels, such as:

["rider_pool", "vip_rider", "vip_rider", "rider_pool"]
["vip_rider", "vip_rider", "vip_rider", "rider_pool"]
["rider_pool", "rider_pool", "rider_pool", "rider_pool"]

Key Points:

Since both segments have equal weights in the example, each has a 50% chance of being selected
Over many runs, the output distribution should reflect the weight ratios
This kind of weighted sampling is useful when Uber wants to target or prioritize 
users based on how critical or valuable the group is to a given initiative

The problem is asking you to implement a weighted random sampling algorithm with replacement.
*/