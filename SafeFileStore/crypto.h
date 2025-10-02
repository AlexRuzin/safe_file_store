#pragma once

#include <vector>
#include <cstddef>

#include <openssl/evp.h>
#include <stdint.h>
#include "error.h"
#include "shared.h"

#define CRC32_POLYNOMIAL 0x90903c3d 

class CryptOps {
private:
    std::vector<std::byte> iv;
    std::string pass;

    std::vector<std::byte> sha256;

public:
    CryptOps(const std::string pass, const std::vector<std::byte> &iv);
    ~CryptOps(void);

private:
    static ERROR CalculateSHA256(const std::vector<std::byte> &in,std::vector<std::byte> &out);

public:
    static CRC32 CalculateCrc32(const std::vector<std::byte> data);
    ERROR GenShaFromPass(void);
    static std::string GetPass(void);
    const ERROR EncryptData(
        __in const std::vector<std::byte> &data,
        __out std::vector<std::byte> *cipherOut
    ) const;
    const ERROR DecryptData(
        __in const std::vector<std::byte> &data,
        __out std::vector<std::byte> *plainOut
    ) const;
};