#pragma once

#include <openssl/evp.h>
#include <stdint.h>
#include "error.h"
#include "shared.h"

#define CRC32_POLYNOMIAL 0x90903c3d 

CRC32 CalculateCrc32(const std::vector<std::byte> data);

ERROR CalculateSHA256(const std::vector<std::byte> &in,std::vector<std::byte> &out);

ERROR EncryptData(
    __in const std::vector<std::byte> &data,
    __in const std::vector<std::byte> &sha256,
    __in const std::vector<std::byte> &iv,
    __out std::vector<std::byte> *cipherText);