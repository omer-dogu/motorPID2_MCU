#pragma once

#include <cstdint>

struct Frame {
    uint8_t type;
    uint8_t payload[255];
};
