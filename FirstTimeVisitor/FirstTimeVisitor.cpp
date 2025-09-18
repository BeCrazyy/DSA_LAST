#include <bits/stdc++.h>

using namespace std;

class CustomerTracker {
private:
    // Doubly linked list node to maintain order of OneTime Visitors
    struct Node {
        int id;
        Node* prev;
        Node* next;
        Node(int customerId) : id(customerId), prev(nullptr), next(nullptr) {}
    };
    
    // HashMap: customerId -> total visit count
    std::unordered_map<int, int> visitCount;
    
    // HashMap: customerId -> pointer to their node in linked list
    // Only OneTime Visitors have entries here
    std::unordered_map<int, Node*> nodeMap;
    
    // Linked list pointers: head = earliest OneTime Visitor
    Node* head = nullptr;
    Node* tail = nullptr;
    
    // Helper: Add new OneTime Visitor to end of list
    void addToList(int customerId) {
        Node* node = new Node(customerId);
        nodeMap[customerId] = node;  // Store reference for O(1) removal later
        
        // Add to tail of linked list
        if (!tail) {
            head = tail = node;  // First node
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
    }
    
    // Helper: Remove customer from OneTime Visitors list (they became Recurrent)
    void removeFromList(int customerId) {
        Node* node = nodeMap[customerId];
        
        // Update previous node's next pointer
        if (node->prev) node->prev->next = node->next;
        else head = node->next;  // Removing head
        
        // Update next node's prev pointer
        if (node->next) node->next->prev = node->prev;
        else tail = node->prev;  // Removing tail
        
        // Clean up
        delete node;
        nodeMap.erase(customerId);
    }
    
public:
    // Record a customer visit
    void postCustomerVisit(int customerId) {
        visitCount[customerId]++;
        
        if (visitCount[customerId] == 1) {
            // First visit: customer becomes OneTime Visitor
            addToList(customerId);
        } 
        else if (visitCount[customerId] == 2) {
            // Second visit: customer graduates to Recurrent Visitor
            removeFromList(customerId);
        }
        // Third+ visit: customer is already Recurrent, nothing to do
    }
    
    // Get the earliest OneTime Visitor (first in queue)
    int getFirstOneTimeVisitor() {
        return head ? head->id : -1;  // -1 if no OneTime Visitors exist
    }
};

int main() {
    CustomerTracker tracker;
    
    tracker.postCustomerVisit(2);
    tracker.postCustomerVisit(5);
    tracker.postCustomerVisit(2);
    tracker.postCustomerVisit(3);
    // cout << tracker.getFirstOneTimeVisitor() << std::endl;  // 5
    cout << tracker.getFirstOneTimeVisitor() << std::endl;  // 5
    assert(tracker.getFirstOneTimeVisitor() == 5);
    
    tracker.postCustomerVisit(2);
    tracker.postCustomerVisit(4);
    tracker.postCustomerVisit(5);
    cout << tracker.getFirstOneTimeVisitor() << std::endl;  // 3
    
    return 0;
}

/*
PROBLEM STATEMENT:
=================

We have millions of customers visiting our website daily. Each customer has a unique 
identifier that remains the same across visits. We classify customers into two types:

1. Recurrent Visitors: customers who have visited more than once
2. OneTime Visitors: customers who have visited exactly once (so far)

Implement a service with two functionalities:
• postCustomerVisit(customerId) - records a customer visit
• getFirstOneTimeVisitor() - returns the earliest OneTime Visitor

EXAMPLE:
--------
postCustomerVisit(2)           // Customer 2: first visit (OneTime)
postCustomerVisit(5)           // Customer 5: first visit (OneTime) 
postCustomerVisit(2)           // Customer 2: second visit (becomes Recurrent)
postCustomerVisit(3)           // Customer 3: first visit (OneTime)
getFirstOneTimeVisitor() → 5   // Returns 5 (earliest remaining OneTime)

postCustomerVisit(2)           // Customer 2: third visit (still Recurrent)
postCustomerVisit(4)           // Customer 4: first visit (OneTime)
postCustomerVisit(5)           // Customer 5: second visit (becomes Recurrent)
getFirstOneTimeVisitor() → 3   // Returns 3 (now earliest remaining OneTime)

REQUIREMENTS:
-------------
- Handle millions of daily customers efficiently
- postCustomerVisit should be fast (O(1))
- getFirstOneTimeVisitor should be very fast (O(1))
- Track order of when customers first became OneTime Visitors
- When OneTime Visitor gets second visit, they graduate to Recurrent

SOLUTION APPROACH:
------------------
HashMap + Doubly Linked List:
- HashMap tracks visit counts and node references for O(1) lookup
- Doubly Linked List maintains insertion order of OneTime Visitors
- Head points to earliest OneTime Visitor
- O(1) removal from middle of list when customer becomes Recurrent
*/