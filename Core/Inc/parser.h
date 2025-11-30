#pragma once

#include <cstdint>
#include "frame.h"
#include "crc.h"
#include "fifo.h"

class Parser
{
public:
    Parser(Frame* f) : m_f(f) { reset(); }

    bool parse(uint8_t& byte)
    {
        switch (state)
        {
        case State::WAIT_START:
            if (byte == 0x55)
                state = State::WAIT_LEN;
            break;

        case State::WAIT_LEN:
            len = byte;
            state = State::WAIT_PAYLOAD;
            break;

        case State::WAIT_PAYLOAD:
            payload.push(byte);
            bytesRead++;
            if (bytesRead >= len)
                state = State::WAIT_CRC1;
            break;

        case State::WAIT_CRC1:
            crc = static_cast<uint16_t>(byte) << 8;
            state = State::WAIT_CRC2;
            break;

        case State::WAIT_CRC2:
            crc |= byte;
            if (crc == crc16_modbus(payload.getAddr(), payload.size())) {
                copyFrame();
                reset();
                return true;
            }
            else
                reset();
            break;
        }

        return false;
    }

private:
    enum class State {
        WAIT_START,
        WAIT_LEN,
        WAIT_PAYLOAD,
        WAIT_CRC1,
        WAIT_CRC2
    };

    State state;
    uint8_t len;
    uint8_t bytesRead;
    uint16_t crc;
    StaticFifo<uint8_t, 128> payload;
    Frame* m_f;

    void reset() {
        state = State::WAIT_START;
        len = 0;
        bytesRead = 0;
        crc = 0;
        payload.clear();
    }
    void copyFrame() {
        uint16_t count = 0;
        payload.pop(m_f->type);
        while (!payload.isEmpty()) {
            payload.pop(m_f->payload[count++]);
        }
    }
};
