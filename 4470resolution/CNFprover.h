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
    std::map<size_t, std::pair<size_t, size_t>> resolvePath;
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
    
    bool isLiteralInClause(std::string literal, const std::vector<std::string>& clause) {
        return std::find(clause.begin(), clause.end(), literal) != clause.end();
    }

    bool isEqual(const std::vector<std::string>& c1, const std::vector<std::string>& c2) {
        if (c1.size() != c2.size()) return false;
        for (int i = 0; i < c1.size(); i++) {
            if (!isLiteralInClause(c1[i], c2)) {
                return false;
            }
        }
        return true;
    }

    bool isClauseInKB(const std::vector<std::string>& newClause) {
        for (auto clause : clauses) {
            if (isEqual(newClause, clause)) {
                return true;
            }
        }
        return false;
    }
    
    void resolve(size_t c1, size_t c2) {
        for (int i = 0; i < clauses[c1].size(); i++) {
            for (int j = 0; j < clauses[c2].size(); j++) {
                if (clauses[c1][i] == negation(clauses[c2][j])) {
                    std::vector<std::string> newClause;
                    for (int k = 0; k < clauses[c1].size(); k++) {
                        if (k != i && !isLiteralInClause(clauses[c1][k], newClause)) {
                            newClause.push_back(clauses[c1][k]);
                        }
                    }
                    for (int k = 0; k < clauses[c2].size(); k++) {
                        if (k != j && !isLiteralInClause(clauses[c2][k], newClause)) {
                            newClause.push_back(clauses[c2][k]);
                        }
                    }
                    if (newClause.empty()) {
                        isProved = true;
                        clauses.push_back(newClause);
                        resolvePath[clauses.size() - 1] = std::make_pair(c1, c2);
                        return;
                    }
                    if (isClauseInKB(newClause)) {
                        return;
                    } else {
                        clauses.push_back(newClause);
                        resolvePath[clauses.size() - 1] = std::make_pair(c1, c2);
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
                    resolve(i, j);
                    if (isProved) return true;
                }
            }
            endIndex = clauses.size();
        }
        return false;
    }
    
    std::string printPath(size_t n) {
        std::string result = "";
        if (resolvePath.find(n) != resolvePath.end()) {
            result = "[ " + printPath(resolvePath[n].first) + " , "
            + std::to_string(n)
            + " , " + printPath(resolvePath[n].second) + " ]";
        } else {
            result = std::to_string(n);
        }
        return result;
    }
    
    void printResolvePath() {
        std::cout << printPath(static_cast<int>(clauses.size()) - 1) << std::endl;
//        for (size_t i = 0; i < clauses.size(); i++) {
//            if (resolvePath.find(i) != resolvePath.end()) {
//                std::cout << "[" << resolvePath[i].first << ","
//                << resolvePath[i].second << "] => ["
//                << i << "]\n";
//            }
//        }
    }
};

#endif /* CNFprover_h */
