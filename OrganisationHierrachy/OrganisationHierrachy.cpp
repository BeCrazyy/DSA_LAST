#include<bits/stdc++.h>

using namespace std;

class OrganisationHierrachy {
  private:
    unordered_map<string, vector<string>>adjacenyList; // manager -> [reportees]
    unordered_map<string, string> managerOf; // reportee -> manager
    unordered_map<string, int> directAndIndirectCount; // manager -> count (direct or indirect count)

    void updateCount(string employee, int sign) {
        if(managerOf.find(employee) == managerOf.end()) {
            return;
        }
        string manager = managerOf[employee];
        directAndIndirectCount[manager] += sign;
        updateCount(manager, sign);
    }

  public:
    OrganisationHierrachy() {}

    void addNewReportee(string manager, string reportee) {
        this->adjacenyList[manager].push_back(reportee);
        this->managerOf[reportee] = manager;
        updateCount(reportee, 1);
    }

    int directOrIndirectCount(string manager) {
        return this->directAndIndirectCount[manager];
    }

    void moveReportee(string reportee, string newManager) {
        
        updateCount(reportee, -1);

        string currentManager = this->managerOf[reportee];
        this->managerOf[reportee] = newManager;
        this->adjacenyList[newManager].push_back(reportee);

        auto it = find(this->adjacenyList[currentManager].begin(), this->adjacenyList[currentManager].end(), reportee);
        if(it != this->adjacenyList[currentManager].end()) {
            this->adjacenyList[currentManager].erase(it);
        }
        updateCount(reportee, 1);
    }
};

int main() {
    OrganisationHierrachy OrganisationHierrachy;
    OrganisationHierrachy.addNewReportee("A", "B");
    OrganisationHierrachy.addNewReportee("A", "C");
    OrganisationHierrachy.addNewReportee("B", "D");
    OrganisationHierrachy.addNewReportee("B", "E");
    OrganisationHierrachy.addNewReportee("C", "F");
    OrganisationHierrachy.addNewReportee("C", "H");
    OrganisationHierrachy.addNewReportee("C", "G");
    OrganisationHierrachy.addNewReportee("F", "I");

    cout<<"A -> "<<OrganisationHierrachy.directOrIndirectCount("A")<<endl;
    cout<<"B -> "<<OrganisationHierrachy.directOrIndirectCount("B")<<endl;
    cout<<"C -> "<<OrganisationHierrachy.directOrIndirectCount("C")<<endl;
    cout<<"D -> "<<OrganisationHierrachy.directOrIndirectCount("D")<<endl;
    cout<<"E -> "<<OrganisationHierrachy.directOrIndirectCount("E")<<endl;
    cout<<"F -> "<<OrganisationHierrachy.directOrIndirectCount("F")<<endl;
    cout<<"G -> "<<OrganisationHierrachy.directOrIndirectCount("G")<<endl;
    cout<<"H -> "<<OrganisationHierrachy.directOrIndirectCount("H")<<endl;
    cout<<"I -> "<<OrganisationHierrachy.directOrIndirectCount("I")<<endl;

    cout<<"Updating the new reportee!!"<<endl;

    OrganisationHierrachy.moveReportee("I", "B");
    OrganisationHierrachy.addNewReportee("I", "T");

    cout<<"A -> "<<OrganisationHierrachy.directOrIndirectCount("A")<<endl;
    cout<<"B -> "<<OrganisationHierrachy.directOrIndirectCount("B")<<endl;
    cout<<"C -> "<<OrganisationHierrachy.directOrIndirectCount("C")<<endl;
    cout<<"D -> "<<OrganisationHierrachy.directOrIndirectCount("D")<<endl;
    cout<<"E -> "<<OrganisationHierrachy.directOrIndirectCount("E")<<endl;
    cout<<"F -> "<<OrganisationHierrachy.directOrIndirectCount("F")<<endl;
    cout<<"G -> "<<OrganisationHierrachy.directOrIndirectCount("G")<<endl;
    cout<<"H -> "<<OrganisationHierrachy.directOrIndirectCount("H")<<endl;
    cout<<"I -> "<<OrganisationHierrachy.directOrIndirectCount("I")<<endl;
    cout<<"I -> "<<OrganisationHierrachy.directOrIndirectCount("T")<<endl;
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