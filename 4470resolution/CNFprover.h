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
#include <iterator>
#include <cctype>

// a class using resolution method to prove CNF file of first order logic
class CNFprover {
private:
    const std::string WHITESPACE = " \t\n\r\v\f";
    // all clauses in the KB (might keep increasing
    std::vector<std::vector<std::string>> clauses;
    // store the resolution path
    std::map<size_t, std::pair<size_t, size_t>> resolvePath;
    // store temporary unification pair
    std::map<std::string, std::string> unification;
    bool isProved = false;
    int lineCounter = 0;
    
public:
    // to clear data of last run
    void clearData() {
        isProved = false;
        for (auto c : clauses) {
            c.clear();
        }
        clauses.clear();
        resolvePath.clear();
    }
    
    // remove white space in string
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
    
    // print one clause to the console
    void printAclause(const std::vector<std::string>& clause) {
        for (int i = 0; i < clause.size(); i++) {
            if (i != 0) {
                std::cout << " | ";
            }
            std::cout << clause[i];
        }
    }
    
    // print all clauses to the console
    void printAllClauses() {
        for (int i = 0; i < clauses.size(); i++) {
            std::cout << "[" << i << "] : ";
            printAclause(clauses[i]);
            std::cout << std::endl;
        }
    }
    
    // check if a literal is a variable
    bool isVariable(std::string literal) {
        if (literal.empty()) return false;
        if (literal[0] >= 'a' && literal[0] <= 'z') {
            return true;
        } else {
            return false;
        }
    }
    
    // parse each part of a literal and rename variable adding lineCounter for distinction
    std::string parseLiteral(std::string literal) {
        std::string returnLiteral = "";
        size_t firstPos = literal.find_first_of('(');
        size_t lastPos = literal.find_last_of(')');
        // validation check
        if (firstPos == std::string::npos || lastPos == std::string::npos || firstPos >= lastPos) {
            returnLiteral = literal;
        } else {
            returnLiteral = literal.substr(0, firstPos + 1);  // copy the first part (the predicate)
            // parse terms inside parentheses
            std::string tempStr = literal.substr(firstPos + 1, lastPos - firstPos - 1);
            std::replace(tempStr.begin(), tempStr.end(), ',', ' ');
            std::istringstream iss(tempStr);
            std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)),std::istream_iterator<std::string>());
            // check each token inside parentheses
            for (int i = 0; i < tokens.size(); i++) {
                if (i > 0) {
                    returnLiteral += ",";
                }
                if (!tokens[i].empty()) {
                    if (tokens[i].find_first_of('(') != std::string::npos) {
                        // if it is a compound term, recusively parse again
                        returnLiteral += parseLiteral(tokens[i]);
                    } else {
                        if (isVariable(tokens[i])) {
                            // if it's variable, rename it by adding lineCounter for distinction
                            returnLiteral += tokens[i] + std::to_string(lineCounter);
                        } else {
                            returnLiteral += tokens[i];
                        }
                    }
                }
            }
            returnLiteral += ")";
        }
        return returnLiteral;
    }
    
    // negate the literal
    std::string negation(std::string literal) {
        if (literal.empty()) return literal;
        if (literal[0] == '~') {
            return literal.substr(1);
        } else {
            return "~" + literal;
        }
    }
    
    // read a Knowledge Base of CNF from a txt file
    bool readCNFfile(std::string fileName) {
        std::ifstream inputFile(fileName);
        if (inputFile) {
            std::string line;
            lineCounter = 0;
            //ignore all the whitespace
            while (std::getline(inputFile, line)) {
                lineCounter++;
                size_t firstCharPos = line.find_first_not_of(WHITESPACE);
                size_t lastCharPos = line.find_last_not_of(WHITESPACE);
                if (firstCharPos != std::string::npos && lastCharPos != std::string::npos) { // if not an empty lines
                    if (line.at(lastCharPos) == '.') {
                        // normal clause ended with a period
                        std::replace(line.begin(), line.end(), '.' , ' ');
                        std::string temp;
                        std::vector<std::string> segments;
                        // split a line into segments
                        std::istringstream iss(line);
                        while (std::getline(iss, temp, '|')) {
                            segments.push_back(parseLiteral(removeSpaces(const_cast<char*>(temp.c_str()))));
                        }
                        if (!segments.empty()) {
                            clauses.push_back(segments);
                        }
                    } else if (line.at(lastCharPos) == '?') {
                        // handle the query, which ended with a question mark
                        std::replace(line.begin(), line.end(), '?' , ' ');
                        // split a line into segments
                        std::istringstream iss(line);
                        std::string temp;
                        while (std::getline(iss, temp, '|')) {
                            std::vector<std::string> segments;
                            // put it's negation to KB (prove by contradition)
                            segments.push_back(negation(parseLiteral(removeSpaces(const_cast<char*>(temp.c_str())))));
                            if (!segments.empty()) {
                                clauses.push_back(segments);
                            }
                        }
                    } else {
                        // handle unexpected ended lines
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
    
    // check if a literal is in a clause
    bool isLiteralInClause(std::string literal, const std::vector<std::string>& clause) {
        return std::find(clause.begin(), clause.end(), literal) != clause.end();
    }

    // check if the first clause contains the second clause
    bool isContained(const std::vector<std::string>& c1, const std::vector<std::string>& c2) {
        if (c1.size() < c2.size()) return false;
        for (int i = 0; i < c2.size(); i++) {
            if (!isLiteralInClause(c2[i], c1)) {
                return false;
            }
        }
        return true;
    }

    // check if a new clause has already include (logically) in a KB
    bool isClauseInKB(const std::vector<std::string>& newClause) {
        for (auto clause : clauses) {
            if (isContained(newClause, clause)) {
                return true;
            }
        }
        return false;
    }
    
    // This function return ture if two literal can be unified, and it also update the unification map
    bool unify(std::string l1, std::string l2) {
        unification.clear();
        std::string predicate1, predicate2;
        size_t startPos1 = l1.find_first_of('(');
        size_t startPos2 = l2.find_first_of('(');
        size_t endPos1 = l1.find_last_of(')');
        size_t endPos2 = l2.find_last_of(')');
        if (startPos1 == std::string::npos && startPos2 == std::string::npos && l1 == negation(l2)) {
            // if the literal is a ground term, check if they are equal
            return true;
        } else if (startPos1 == std::string::npos || startPos2 == std::string::npos) {
            // if not equal
            return false;
        }
        // get those predicates
        predicate1 = l1.substr(0, startPos1);
        predicate2 = l2.substr(0, startPos2);
        if (predicate1 == negation(predicate2)) { // if predicates are negative
            if (endPos1 == std::string::npos || endPos1 == std::string::npos || startPos1 >= endPos1 || startPos2 >= endPos2) {
                return false;
            } else {
                // get tokens of each literal
                std::string tempStr1 = l1.substr(startPos1 + 1, endPos1 - startPos1 - 1);
                std::replace(tempStr1.begin(), tempStr1.end(), ',', ' ');
                std::istringstream iss1(tempStr1);
                std::vector<std::string> tokens1((std::istream_iterator<std::string>(iss1)),std::istream_iterator<std::string>());
                std::string tempStr2 = l2.substr(startPos2 + 1, endPos2 - startPos2 - 1);
                std::replace(tempStr2.begin(), tempStr2.end(), ',', ' ');
                std::istringstream iss2(tempStr2);
                std::vector<std::string> tokens2((std::istream_iterator<std::string>(iss2)),std::istream_iterator<std::string>());
                // compare the number of parameters
                if (tokens1.size() != tokens2.size()) {
                    return false;
                }
                for (int i = 0; i < tokens1.size(); i++) {
                    if (isVariable(tokens1[i])) {
                        // variable in the first token
                        if (isVariable(tokens2[i])) {
                            // variable in the second token
                            if (unification.find(tokens1[i]) == unification.end() && unification.find(tokens2[i]) == unification.end()) {
                                unification[tokens1[i]] = tokens2[i];
                            }
                        } else {
                            unification[tokens1[i]] = tokens2[i];
                        }
                    } else {
                        if (isVariable(tokens2[i])) {
                            // variable in the second token
                            unification[tokens2[i]] = tokens1[i];
                        } else {
                            // handle both not variable (avoid handling the compound case recursively)
                            if (tokens1[i] != tokens2[i]) {
                                return false;
                            }
                        }
                    }
                }
            }
            return true;
        }
        return false;
    }
    
    // lift other literals by the unification map while resolving new clause
    std::string lift(std::string literal) {
        std::string returnLiteral = "";
        size_t firstPos = literal.find_first_of('(');
        size_t lastPos = literal.find_last_of(')');
        if (firstPos == std::string::npos || lastPos == std::string::npos || firstPos >= lastPos) {
            returnLiteral = literal;
        } else {
            // parse the literal
            returnLiteral = literal.substr(0, firstPos + 1);
            std::string tempStr = literal.substr(firstPos + 1, lastPos - firstPos - 1);
            std::replace(tempStr.begin(), tempStr.end(), ',', ' ');
            std::istringstream iss(tempStr);
            std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)),std::istream_iterator<std::string>());
            for (int i = 0; i < tokens.size(); i++) {
                if (i > 0) {
                    returnLiteral += ",";
                }
                if (!tokens[i].empty()) {
                    if (tokens[i].find_first_of('(') != std::string::npos) {
                        // handle compounds
                        returnLiteral += lift(tokens[i]);
                    } else {
                        // find all variable in the literal, lift it if it is needed
                        if (isVariable(tokens[i]) && unification.find(tokens[i]) != unification.end()) {
                            returnLiteral += unification[tokens[i]];
                        } else {
                            returnLiteral += tokens[i];
                        }
                    }
                }
            }
            returnLiteral += ")";
        }
        return returnLiteral;
    }
    
    // main process of resolution, check if two clauses the be resolved
    void resolve(size_t c1, size_t c2) {
        // loop each pair in two clauses
        for (int i = 0; i < clauses[c1].size(); i++) {
            for (int j = 0; j < clauses[c2].size(); j++) {
                // check if two literals can be unified
                if (unify(clauses[c1][i], clauses[c2][j])) {
                    // if yes, generate a new clause by resolving the two clauses together
                    std::vector<std::string> newClause;
                    // insert literals from the first clause
                    for (int k = 0; k < clauses[c1].size(); k++) {
                        std::string tempStr = lift(clauses[c1][k]);
                        if (k != i && !isLiteralInClause(tempStr, newClause)) {
                            newClause.push_back(tempStr);
                        }
                    }
                    // insert literals from the second clause
                    for (int k = 0; k < clauses[c2].size(); k++) {
                        std::string tempStr = lift(clauses[c2][k]);
                        if (k != j && !isLiteralInClause(tempStr, newClause)) {
                            newClause.push_back(tempStr);
                        }
                    }
                    // if the new clause is empty, that query is proved by contradition
                    if (newClause.empty()) {
                        isProved = true;
                        clauses.push_back(newClause);
                        // store the resolve path
                        resolvePath[clauses.size() - 1] = std::make_pair(c1, c2);
                        return;
                    }
                    // if the new clause is not empty, check if it is contained in KB
                    if (isClauseInKB(newClause)) {
                        return;
                    } else {
                        // if not, insert it to the KB
                        clauses.push_back(newClause);
                        resolvePath[clauses.size() - 1] = std::make_pair(c1, c2);
                        // store the resolve path
                   }
                }
            }
        }
    }

    // try to prove the query by resolution algorithm, return true if the query can be proved by the KB
    bool resolutionProve() {
        size_t startIndex = 0;
        size_t endIndex = clauses.size();
        // the startIndex and endIndex indicate the new set of clauses
        while (startIndex < endIndex) {
            // if there are new clauses being put into the KB, continue searching
            size_t tempIndex = startIndex;
            startIndex = endIndex;
            // match each new clauses to all clauses in the KB
            for (size_t i = 0; i < endIndex; i++) {
                for (size_t j = (i + 1 > tempIndex ? i + 1 : tempIndex); j < endIndex; j++) {
                    resolve(i, j);
                    if (isProved) return true;
                }
            }
            endIndex = clauses.size();
        }
        // if no new clauses being gerenated, the proof was failed
        return false;
    }
    
    // print the solve path in one line format
    std::string printPath1(size_t n) {
        std::string result = "";
        if (resolvePath.find(n) != resolvePath.end()) {
            result = "[ " + printPath1(resolvePath[n].first) + " , "
            + std::to_string(n)
            + " , " + printPath1(resolvePath[n].second) + " ]";
        } else {
            result = std::to_string(n);
        }
        return result;
    }
    
    // print the solve path in multiple lines format
    std::string printPath2(size_t n) {
        std::string result = "";
        if (resolvePath.find(n) != resolvePath.end()) {
            result = "[" + std::to_string(resolvePath[n].first) + ","
            + std::to_string(resolvePath[n].second) + "] => ["
            + std::to_string(n) + "]\n"
            + printPath2(resolvePath[n].first)
            + printPath2(resolvePath[n].second);
        }
        return result;
    }
    
    // print the solve path
    void printResolvePath() {
        std::cout << printPath1(static_cast<int>(clauses.size()) - 1) << std::endl << std::endl;
        std::cout << printPath2(static_cast<int>(clauses.size()) - 1) << std::endl;
    }
};

#endif /* CNFprover_h */
