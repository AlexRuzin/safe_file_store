#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdint.h>

#include "file.h"
#include "error.h"
#include "user_logging.h"

FileOps::FileOps(const std::string &path) :
    path(path)
{
    
}

FileOps::~FileOps(void)
{
    if (buf.size() > 0) {
        std::memset(buf.data(),0x00,buf.size());
        buf.clear();
    }

    // release lock on file
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

bool FileOps::LockFile(void)
{
     this->fileStream = std::fstream(path, std::ios::app | std::ios::in | std::ios::out);
     if (fileStream.is_open()) {
        return true;
     }

     return false;
}

void FileOps::SetData(const std::vector<std::byte> &in)
{
    if (in.size() == 0) {
        return;
    }

    if (buf.size() > 0) {
        std::memset(buf.data(), 0x00, buf.size());
        buf.clear();
    }

    buf = in;
}

const std::vector<std::byte> *FileOps::GetData(void) const
{
    if (buf.size() == 0) {
        return nullptr;
    }

    return &buf;
}

ERROR FileOps::ReadFile(void)
{
    if(!fileStream.is_open()) {
        return ERROR_FILE_NOT_OPEN;
    }

    if (buf.size() > 0) {
        std::memset(buf.data(),0x00,buf.size());
        buf.clear();
    }

    std::streamsize fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    
    if (!fileStream.read(reinterpret_cast<char *>(buf.data()), fileSize)) {
        return ERROR_FILE_READ_FAIL;
    } 

    return ERROR_OK;
}

ERROR FileOps::WriteFile(void)
{
    if (!fileStream.is_open()) {
        return ERROR_FILE_NOT_OPEN;
    }

    if (buf.size() == 0) {
        return ERROR_NO_DATA;
    }

    fileStream.write(reinterpret_cast<const char*>(buf.data()), buf.size());

    return ERROR_OK;
}

bool FileOps::DeleteFile(const std::string &path)
{
    if (!std::filesystem::exists(path)) {
        return true;
    }

    return std::filesystem::remove(path);
}