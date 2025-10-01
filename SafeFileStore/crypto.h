#pragma once

#include <stdint.h>
#include "shared.h"

#define CRC32_POLYNOMIAL 0x90903c3d 

CRC32 calculate_crc32(const BYTE *data, size_t length);