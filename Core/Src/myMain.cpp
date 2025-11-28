#include "myMain.h"
#include "main.h"

#include <iostream>
#include <cstdint>
#include "comm.h"

Comm<> serialUart;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    serialUart.writeRxFifo();
    serialUart.startRx();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	serialUart.startTx();
}

struct Frame {
    uint8_t type;
    uint8_t payload[255];
};

uint16_t crc16_modbus(const uint8_t* data, size_t size) {
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

class FrameParser
{
public:
    FrameParser(Frame* f) : m_f(f) { reset(); }

    bool parse(uint8_t byte)
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

Frame g_frame;
FrameParser parser(&g_frame);

using CmdHandler = void(*)(const uint8_t* data);

CmdHandler CmdFuncList[256] = { nullptr };

void handleFrame(const Frame& f)
{
    CmdHandler h = CmdFuncList[f.type];
    if (h)
        h(f.payload);
}

void processRx()
{
    uint8_t b;

    while (serialUart.readRxFifo(b)) {
        if (parser.parse(b)) {
            handleFrame(g_frame);
        }
    }
}

void RequestPing()
{
    uint8_t txData[6] = { 0x55, 0x02, 0x00, 0x00 };
    uint16_t crc = crc16_modbus(&txData[2], txData[1]);
    txData[4] = crc >> 8;
    txData[5] = crc & 0xFF;

    serialUart.writeTxFifoMulti(txData, 6);
}

void CmdPing(const uint8_t* data)
{
	if (data[0] == 0) {
	    uint8_t txData[6] = { 0x55, 0x02, 0x00, 0x01 };
	    uint16_t crc = crc16_modbus(&txData[2], txData[1]);
	    txData[4] = crc >> 8;
	    txData[5] = crc & 0xFF;

	    serialUart.writeTxFifoMulti(txData, 6);
	    serialUart.startTx();
	}
    if (data[0] == 1)
        ; // ok
}

void InitGetFuncList()
{
    CmdFuncList[0] = CmdPing;
}

int flag;

void myMain()
{
	InitGetFuncList();
	serialUart.startRx();

	while (1)
	{
		processRx();
		if (flag == 1) {
			flag = 0;
			RequestPing();
		}
	}
}
