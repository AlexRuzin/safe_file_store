#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

typedef char BYTE;
typedef uint32_t CRC32;

typedef uint32_t HEADER;

#define MAX_DATE_FIELD 32

// File header
#pragma pack(push, 8)
typedef struct _crypt_file_header {
    HEADER hdr;
    uint32_t fileSize;
    CRC32 sum;
    char dateEdit[MAX_DATE_FIELD];
} CRYPT_FILE_HEADER, *PCRYPT_FILE_HEADER;
#pragma pack(pop)

inline std::vector<std::string> SplitString(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s); 

    while (std::getline(tokenStream, token, delimiter)) { 
        tokens.push_back(token);
    }
    return tokens;
}