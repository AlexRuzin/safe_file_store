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

ERROR EncryptData(
    __in const std::vector<std::byte> &data, 
    __in const std::vector<std::byte> &sha256,
    __in const std::vector<std::byte> &iv,
    __out std::vector<std::byte> &cipherText)
{
    if (iv.size() != EVP_MAX_IV_LENGTH || sha256.size() != EVP_MAX_MD_SIZE / 2) {
        return ERROR_PARAMS;
    }

    EVP_CIPHER_CTX *ctx = nullptr;
    int len = 0;
    int ciphertext_len = 0;

    if(!(ctx = EVP_CIPHER_CTX_new())) {
        return ERROR_AES_CTX_INIT;
    }

    if(1 != EVP_EncryptInit_ex(ctx, 
        EVP_aes_256_cbc(),
        NULL,
        reinterpret_cast<const unsigned char*>(sha256.data()),
        reinterpret_cast<const unsigned char*>(iv.data()))) 
    {
        return ERROR_AES_ENCRYPT_INIT;
    }
    
    cipherText.resize(data.size());
    if(1 != EVP_EncryptUpdate(ctx,
        reinterpret_cast<unsigned char*>(cipherText.data()),
        &len,
        reinterpret_cast<const unsigned char*>(data.data()),
        data.size())) {
        return ERROR_AES_ENCRYPT_UPDATE;
    }
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx,
        reinterpret_cast<unsigned char*>(cipherText.data()) + len,
        &len)) {
        return ERROR_AES_ENCRYPT_FINAL;
    }

    ciphertext_len += len;
    cipherText.push_back(std::byte(0));

    EVP_CIPHER_CTX_free(ctx);

    return ERROR_OK;    
}

ERROR CalculateSHA256(const std::vector<std::byte> &in, std::vector<std::byte> &out)
{
    if (out.size() != EVP_MAX_MD_SIZE / 2) {
        return ERROR_OK;
    }

    EVP_MD_CTX *mdctx = nullptr;
    const EVP_MD *md = nullptr;
    unsigned char md_value[EVP_MAX_MD_SIZE / 2] = { 0 };
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

    std::memset(out.data(), 0x00, EVP_MAX_MD_SIZE / 2);
    std::memcpy(out.data(), md_value, EVP_MAX_MD_SIZE / 2);
    std::memset(md_value, 0x00, EVP_MAX_MD_SIZE / 2);

    return ERROR_OK;
}
