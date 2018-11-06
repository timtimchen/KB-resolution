//
//  main.cpp
//  4470resolution
//
//  Worked by Group 7: Peng Sophy Hok & Jinghong Chen on 11/1/18.
//

#include <iostream>
#include "CNFprover.h"
using namespace std;

int main(int argc, const char * argv[]) {
    string fileName = "";
    string dir = "/Users/jinghongchen/uvu/";
    CNFprover prover;
    cout << "Please input a source file name: (input 'Q' to quit)\n";
    cin >> fileName;
    while (fileName != "q" && fileName != "Q") {
        prover.clearData();
        if (prover.readCNFfile(dir + fileName)) {
            prover.printAllClauses();
        }
        cout << "\nPlease input a source file name: (input 'Q' to quit)\n";
        cin >> fileName;
    }
    return 0;
}
