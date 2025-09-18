#include<bits/stdc++.h>

using namespace std;

class OrganisationHierrachyBruteForce {
  private:
    unordered_map<string, vector<string>>adjacenyList; // manager -> reportees
    unordered_map<string, string> managerOf; // employee -> manger

    void dfs(string employee, int& count) {
        for(string reportee : adjacenyList[employee]) {
            count++;
            dfs(reportee, count);
        }
    }

  public:
    OrganisationHierrachyBruteForce() {}

    void addNewReportee(string manager, string reportee) {
        this->adjacenyList[manager].push_back(reportee);
        this->managerOf[reportee] = manager;
    }

    int countDirectAndIndirectReportee(string manager) {
        int count = 0;
        dfs(manager, count);
        return count;
    }

    void moveReportee(string reportee, string newManager) {
        string currentManager = this->managerOf[reportee];
        this->managerOf[reportee] = newManager;
        this->adjacenyList[newManager].push_back(reportee);

        auto it = find(this->adjacenyList[currentManager].begin(), this->adjacenyList[currentManager].end(), reportee);
        if(it != this->adjacenyList[currentManager].end()) {
            this->adjacenyList[currentManager].erase(it);
        }
    }
};

int main() {
    OrganisationHierrachyBruteForce OrganisationHierrachy;
    OrganisationHierrachy.addNewReportee("A", "B");
    OrganisationHierrachy.addNewReportee("A", "C");
    OrganisationHierrachy.addNewReportee("B", "D");
    OrganisationHierrachy.addNewReportee("B", "E");
    OrganisationHierrachy.addNewReportee("C", "F");
    OrganisationHierrachy.addNewReportee("C", "H");
    OrganisationHierrachy.addNewReportee("C", "G");
    OrganisationHierrachy.addNewReportee("F", "I");

    cout<<"A -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("A")<<endl;
    cout<<"B -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("B")<<endl;
    cout<<"C -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("C")<<endl;
    cout<<"D -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("D")<<endl;
    cout<<"E -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("E")<<endl;
    cout<<"F -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("F")<<endl;
    cout<<"G -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("G")<<endl;
    cout<<"H -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("H")<<endl;
    cout<<"I -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("I")<<endl;

    cout<<"Updating the new reportee!!"<<endl;

    OrganisationHierrachy.moveReportee("I", "B");
    OrganisationHierrachy.addNewReportee("I", "T");

    cout<<"A -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("A")<<endl;
    cout<<"B -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("B")<<endl;
    cout<<"C -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("C")<<endl;
    cout<<"D -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("D")<<endl;
    cout<<"E -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("E")<<endl;
    cout<<"F -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("F")<<endl;
    cout<<"G -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("G")<<endl;
    cout<<"H -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("H")<<endl;
    cout<<"I -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("I")<<endl;
    cout<<"T -> "<<OrganisationHierrachy.countDirectAndIndirectReportee("T")<<endl;
    return 0;
}

/*
PROBLEM STATEMENT:
Given a list of Managers and Reportees, write the function given below:

Given the name of the manager, return count of all the direct and indirect reportees
Add a new reportee to a manager
Change the manager of given person

Input Example:
[("A", "B"), ("A", "C"), ("B", "D"), ("B", "E"), ("C", "F"), ("C", "H"), ("C", "G"), ("F", "I")]

                   
                               A
                  B                       C

           D            E            F     H     G
                         
                                  I
                       
Expected Behavior:
ReturnReporteeCount(A) = 8    // A has all 8 people under them (direct + indirect)
ReturnReporteeCount(C) = 4    // C has F, H, G, I under them

AddReporteeToManager(F, J)    // Add J as reportee to F
ReturnReporteeCount(A) = 9    // Now A has 9 people
ReturnReporteeCount(C) = 5    // Now C has 5 people
ReturnReporteeCount(B) = 2    // B still has D, E

MoveReportee(B, F)           // Move B to report to F instead of A
ReturnReporteeCount(B) = 2    // B still manages D, E
ReturnReporteeCount(C) = 2    // C now only has H, G (F moved to B's subtree)
ReturnReporteeCount(A) = 9    // A still has 9 total (same people, different structure)
*/