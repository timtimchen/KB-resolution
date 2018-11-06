//
//  CNFprover.h
//  4470resolution
//
//  Worked by Group 7: Peng Sophy Hok & Jinghong Chen on 11/1/18.
//

#ifndef CNFprover_h
#define CNFprover_h
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>

class CNFprover {
private:
    const std::string WHITESPACE = " \t\n\r\v\f";
    std::vector<std::vector<std::string>> clauses;
    std::map<int, std::pair<int, int>> resolvePath;
    bool isProved = false;
    
public:
    void clearData() {
        isProved = false;
        for (auto c : clauses) {
            c.clear();
        }
        clauses.clear();
        resolvePath.clear();
    }
    
    char * removeSpaces(char *str)
    {
        int i = 0, j = 0;
        while (str[i])
        {
            if (!std::isspace(str[i]))
                str[j++] = str[i];
            i++;
        }
        str[j] = '\0';
        return str;
    }
    
    void printAclause(const std::vector<std::string>& clause) {
        for (int i = 0; i < clause.size(); i++) {
            if (i != 0) {
                std::cout << " | ";
            }
            std::cout << clause[i];
        }
    }
    
    void printAllClauses() {
        for (int i = 0; i < clauses.size(); i++) {
            std::cout << "[" << i << "] : ";
            printAclause(clauses[i]);
            std::cout << std::endl;
        }
    }
    
    std::string negation(std::string literal) {
        if (literal.empty()) return literal;
        if (literal[0] == '~') {
            return literal.substr(1);
        } else {
            return "~" + literal;
        }
    }
    
    bool readCNFfile(std::string fileName) {
        std::ifstream inputFile(fileName);
        if (inputFile) {
            std::string line;
            int lineCounter = 0;
            //ignore all the whitespace
            while (std::getline(inputFile, line)) {
                lineCounter++;
                size_t firstCharPos = line.find_first_not_of(WHITESPACE);
                size_t lastCharPos = line.find_last_not_of(WHITESPACE);
                if (firstCharPos != std::string::npos && lastCharPos != std::string::npos) { // if not an empty lines
                    if (line.at(lastCharPos) == '.') {
                        std::replace(line.begin(), line.end(), '.' , ' ');
                        std::string temp;
                        std::vector<std::string> segments;
                        // split a line into segments
                        std::istringstream iss(line);
                        while (std::getline(iss, temp, '|')) {
                            segments.push_back(removeSpaces(const_cast<char*>(temp.c_str())));
                        }
                        if (!segments.empty()) {
                            clauses.push_back(segments);
                        }
                    } else if (line.at(lastCharPos) == '?') {
                        std::replace(line.begin(), line.end(), '?' , ' ');
                        // split a line into segments
                        std::istringstream iss(line);
                        std::string temp;
                        while (std::getline(iss, temp, '|')) {
                            std::vector<std::string> segments;
                            segments.push_back(negation(removeSpaces(const_cast<char*>(temp.c_str()))));
                            if (!segments.empty()) {
                                clauses.push_back(segments);
                            }
                        }
                    } else {
                        std::cout << "Unfinished line in: " << fileName << "(line: " << lineCounter << ")\n";
                        inputFile.close();
                        return false;
                    }
                }
                // ignore empty lines and continue
            }
            inputFile.close();
            return true;
        } else {
            std::cout << "Cannot open the file: " << fileName << std::endl;
            return false;
        }
    }
    
    bool isAlreadyInKB(const std::vector<std::string>& newClause) {
        for (auto clause : clauses) {
            if (newClause.size() == clause.size()) {
                
            }
        }
        return false;
    }
    
    void resolve(const std::vector<std::string>& c1, const std::vector<std::string>& c2) {
        for (int i = 0; i < c1.size(); i++) {
            for (int j = 0; j < c2.size(); j++) {
                if (c1[i] == negation(c2[j])) {
                    std::vector<std::string> newClause;
                    for (int k = 0; k < c1.size(); k++) {
                        if (k != i) {
                            newClause.push_back(c1[i]);
                        }
                    }
                    for (int k = 0; k < c2.size(); k++) {
                        if (k != j) {
                            newClause.push_back(c2[j]);
                        }
                    }
                    if (isAlreadyInKB(newClause)) {
                        return;
                    } else {
                        clauses.push_back(newClause);
                        resolvePath[static_cast<int>(clauses.size()) - 1] = std::make_pair(i, j);
                    }
                }
            }
        }
    }

    bool resolutionProve() {
        size_t startIndex = 0;
        size_t endIndex = clauses.size();
        while (startIndex < endIndex) {
            size_t tempIndex = startIndex;
            startIndex = endIndex;
            for (size_t i = 0; i < endIndex; i++) {
                for (size_t j = (i + 1 > tempIndex ? i + 1 : tempIndex); j < endIndex; j++) {
                    resolve(clauses[i], clauses[j]);
                    if (isProved) return true;
                }
            }
        }
        return false;
    }
};

#endif /* CNFprover_h */
