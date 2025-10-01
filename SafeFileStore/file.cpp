#include <vector>
#include <string>
#include <fstream>
#include <stdint.h>

#include "file.h"
#include "user_logging.h"

std::vector<BYTE> *ReadFile(const std::string &path) 
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        LOG_ERROR("failed to open filepath %s", path);
        return nullptr;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<BYTE> *buffer = new std::vector<BYTE>(fileSize);
    if (!file.read(buffer->data(), fileSize)) {
        LOG_ERROR("failed to read file %s", path);
        return nullptr;
    } 

    LOG_INFO("successfully read file %s (size %d)", path, fileSize);
    return buffer;
}