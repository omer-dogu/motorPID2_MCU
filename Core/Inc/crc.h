#pragma once

#include <cstdint>

static uint16_t crc16_modbus(const uint8_t* data, size_t size) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < size; ++i) {
        crc ^= static_cast<uint16_t>(data[i]);
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            }
            else {
                crc >>= 1;
            }
        }
    }
    return crc;
}
