#pragma once

#include <vector>
#include <string>
#include <stdint.h>

#include "shared.h"
#include "error.h"


std::vector<std::byte> *ReadFile(const std::string &path);


ERROR WriteFile(const std::vector<std::byte> &data,const std::string filePath);