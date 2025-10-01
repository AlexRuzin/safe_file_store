#pragma once

#include <openssl/evp.h>
#include <stdint.h>
#include "error.h"
#include "shared.h"

#define CRC32_POLYNOMIAL 0x90903c3d 

CRC32 CalculateCrc32(const std::vector<std::byte> data);

ERROR CalculateSHA256(const std::vector<std::byte> &in, std::byte out[EVP_MAX_MD_SIZE]);

ERROR EncryptData(const std::vector<std::byte> &data,const std::byte sha256[EVP_MAX_MD_SIZE]);