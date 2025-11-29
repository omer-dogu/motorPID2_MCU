#include "myMain.h"
#include "main.h"

#include <cstdint>
#include "comm.h"
#include "parser.h"
#include "handler.h"
#include "motor.h"
#include "timer.h"

Comm<> serialUart;
Handler g_handler;
Frame g_frame;
Parser parser(&g_frame);
Motor g_motor;

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

void CmdRpm(const uint8_t* data)
{
	if (data[0] == 0) {
	    g_motor.SetRpm((data[1] << 8) | data[2]);
	}
}

void SendRpm()
{
    uint8_t txData[8] = { 0x55, 0x04, 0x01, 0x01 };
    uint16_t rpm = g_motor.GetRpm();
    txData[4] = rpm >> 8;
    txData[5] = rpm & 0xFF;
    uint16_t crc = crc16_modbus(&txData[2], txData[1]);
    txData[6] = crc >> 8;
    txData[7] = crc & 0xFF;

    serialUart.writeTxFifoMulti(txData, 8);
    serialUart.startTx();
}

timeCtrl_t task50ms;

void Task50Ms()
{
	if(checkTimeCtrl(&task50ms)) {
		resetTimeCtrl(&task50ms);
		SendRpm();
	}
}

void myMain()
{
	g_handler.AddFuncList(0, CmdPing);
	g_handler.AddFuncList(1, CmdRpm);
	serialUart.startRx();

	setTimeCtrl(&task50ms, 50);
	HAL_TIM_Base_Start_IT(&htim2);

	while (1)
	{
		processRx();
		Task50Ms();
	}
}
