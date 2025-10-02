#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdint.h>

#include "shared.h"
#include "error.h"

class FileOps {
private:
    const std::string path;
    std::fstream fileStream;

    // Buffer for file data
    std::vector<std::byte> buf;

public:
    FileOps(const std::string &path);
    ~FileOps(void);

    bool LockFile(void);

    ERROR ReadFile(void);
    ERROR WriteFile(void);

    void SetData(const std::vector<std::byte> &in);
    const std::vector<std::byte> *GetData(void) const;

    static bool DeleteFile(const std::string &path);
};