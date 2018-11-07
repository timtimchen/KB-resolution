//
//  main.cpp
//  4470resolution
//
//  Worked by Group 7: Peng Sophy Hok & Jinghong Chen on 11/1/18.
//

#include "CNFprover.h"
#include <iostream>
using namespace std;

int main(int argc, const char * argv[]) {
    string fileName = "";
    CNFprover prover;
    cout << "Please input a source file name: (input 'Q' to quit)\n";
    cin >> fileName;
    while (fileName != "q" && fileName != "Q") {
        cout << endl;
        prover.clearData();  // clear the data of last run
        if (prover.readCNFfile(fileName)) {
            if (prover.resolutionProve()) {
                // if the query is proved
                prover.printAllClauses();
                cout << "\nThe query has been proved! \n\n";
                prover.printResolvePath();
            } else {
                // if it is not proved
                prover.printAllClauses();
                cout << "\nThe query cannot be proved by the given KB. \n";
            }
        }
        cout << "Please input a source file name: (input 'Q' to quit)\n";
        cin >> fileName;
    }
    return 0;
}
