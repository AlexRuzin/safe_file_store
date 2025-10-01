#include "crypto.h"
#include "shared.h"
#include "error.h"

#include <vector>
#include <stdint.h>
#include <openssl/evp.h>

CRC32 CalculateCrc32(const std::vector<std::byte> data)
{
    const char *ptr = reinterpret_cast<const char*>(data.data());

    CRC32 crc = 0xFFFFFFFF;

    for(size_t i = 0; i < data.size(); i++) {
        crc ^= ptr[i]; 

        for(int j = 0; j < 8; j++) {
            if(crc & 1) {
                crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc;
}

ERROR EncryptData(const std::vector<std::byte> &data, const std::byte sha256[EVP_MAX_MD_SIZE])
{
    return ERROR_OK;
}

ERROR CalculateSHA256(const std::vector<std::byte> &in, std::byte out[EVP_MAX_MD_SIZE])
{
    if (out == nullptr) {
        return ERROR_PARAMS;
    }

    EVP_MD_CTX *mdctx = nullptr;
    const EVP_MD *md = nullptr;
    unsigned char md_value[EVP_MAX_MD_SIZE] = { 0 };
    uint32_t md_len = 0;

    md = EVP_sha256(); 
    mdctx = EVP_MD_CTX_new();

    if(mdctx == NULL) {
        return ERROR_SHA256_INIT_FAIL;
    }

    if(EVP_DigestInit_ex(mdctx,md,NULL) != 1) {
        return ERROR_SHA256_DIGEST_ERROR;
    }

    if(EVP_DigestUpdate(mdctx, in.data(), in.size()) != 1) {
        return ERROR_SHA256_DIGEST_ERROR;
    }

    if(EVP_DigestFinal_ex(mdctx, md_value, &md_len) != 1) {
        return ERROR_SHA256_CALC_FAIL;
    }

    EVP_MD_CTX_free(mdctx);

    std::memset(out, 0x00,EVP_MAX_MD_SIZE);
    std::memcpy(out, md_value, EVP_MAX_MD_SIZE);
    std::memset(md_value, 0x00,EVP_MAX_MD_SIZE);

    return ERROR_OK;
}
