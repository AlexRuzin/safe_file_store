#include "crypto.h"
#include "shared.h"

#include <stdint.h>

CRC32 calculate_crc32(const BYTE *data, size_t length)
{
    CRC32 crc = 0xFFFFFFFF;

    for(size_t i = 0; i < length; i++) {
        crc ^= data[i]; 

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