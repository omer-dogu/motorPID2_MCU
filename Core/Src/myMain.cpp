#include "myMain.h"
#include "main.h"

#include <cstdint>
#include "comm.h"
#include "parser.h"
#include "handler.h"

Comm<> serialUart;
Handler g_handler;
Frame g_frame;
Parser parser(&g_frame);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    serialUart.writeRxFifo();
    serialUart.startRx();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	serialUart.startTx();
}

void processRx()
{
    uint8_t b;

    while (serialUart.readRxFifo(b)) {
        if (parser.parse(b)) {
            if (Handler::CmdHandler h = g_handler.GetFuncList(g_frame.type))
                h(g_frame.payload);
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
    serialUart.startTx();
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

void myMain()
{
	g_handler.AddFuncList(0, CmdPing);
	serialUart.startRx();

	while (1)
	{
		processRx();
	}
}
