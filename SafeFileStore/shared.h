#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

inline std::vector<std::string> SplitString(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s); 

    while (std::getline(tokenStream, token, delimiter)) { 
        tokens.push_back(token);
    }
    return tokens;
}