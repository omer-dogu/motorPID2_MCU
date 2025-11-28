#pragma once

#include <cstdint>
#include "fifo.h"
#include "main.h"

template<typename T = uint8_t>
class Comm
{
public:
    void startRx(void) {
        HAL_UART_Receive_IT(&huart2, &rxData, 1);
    }
    void startTx(void) {
        if (!txFifo.isEmpty()) {
            size_t size = txFifo.size();
            size_t count = 0;
            while (size--)
                txFifo.pop(tempBuf[count++]);
            HAL_UART_Transmit_IT(&huart2, tempBuf, count);
        }
    }

    void writeTxFifo(const T& in) {
        txFifo.push(in);
    }

    void writeTxFifoMulti(const T* buf, size_t len) {
        txFifo.pushMulti(buf, len);
    }

    void writeRxFifo(void) {
        rxFifo.push(rxData);
    }

    bool readRxFifo(T& out) {
        return rxFifo.pop(out);
    }
private:
    T rxData{};
    StaticFifo<T, 128> rxFifo;
    StaticFifo<T, 128> txFifo;
    T tempBuf[128] = { 0 };
};
