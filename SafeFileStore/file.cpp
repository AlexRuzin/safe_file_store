#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdint.h>

#include "file.h"
#include "error.h"
#include "user_logging.h"

std::vector<std::byte> *ReadFile(const std::string &path) 
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        LOG_ERROR("failed to open filepath %s", path);
        return nullptr;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::byte> *buffer = new std::vector<std::byte>(fileSize);
    if (!file.read((char *)buffer->data(), fileSize)) {
        LOG_ERROR("failed to read file %s", path);
        return nullptr;
    } 

    LOG_INFO("successfully read file %s (size %d)", path, fileSize);
    return buffer;
}

ERROR WriteFile(const std::vector<std::byte> &data, const std::string filePath)
{
    if (std::filesystem::exists(filePath)) {
        if (std::filesystem::remove(filePath)) {
            return ERROR_DELETE_FILE_FAIL;
        }
    }

    std::ofstream outFp(filePath, std::ios::binary);

    if (!outFp.is_open()) {
        return ERROR_FILE_NOT_OPEN;
    }

    outFp.write(reinterpret_cast<const char*>(data.data()), data.size());
    outFp.close();

    return ERROR_OK;
}