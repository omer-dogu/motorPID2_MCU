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

Timer task50ms;
Timer task10ms;

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

void CmdDuty(const uint8_t* data)
{
	if (data[0] == 0) {
	    g_motor.SetDuty(data[1]);
	}
}

void CmdP(const uint8_t* data)
{
	if (data[0] == 0) {
	    g_motor.SetP(data[1] / 10.0);
	}
}

void CmdI(const uint8_t* data)
{
	if (data[0] == 0) {
	    g_motor.SetI(data[1] / 10.0);
	}
}

void CmdMotor(const uint8_t* data)
{
	if (data[0] == 0) {
		if (data[1] == 0){
			if (data[2] == 0)
				g_motor.MotorDisable();
			else if (data[2] == 1)
				g_motor.MotorEnable();
		} else if (data[1] == 1){
			if (data[2] == 0)
				g_motor.MotorCW();
			else if (data[2] == 1)
				g_motor.MotorCCW();
		}
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
}

void SendDuty()
{
    uint8_t txData[7] = { 0x55, 0x03, 0x02, 0x01 };
    txData[4] = g_motor.GetDuty();
    uint16_t crc = crc16_modbus(&txData[2], txData[1]);
    txData[5] = crc >> 8;
    txData[6] = crc & 0xFF;

    serialUart.writeTxFifoMulti(txData, 7);
}

void SendP()
{
    uint8_t txData[7] = { 0x55, 0x03, 0x03, 0x01 };
    txData[4] = g_motor.GetP() * 10;
    uint16_t crc = crc16_modbus(&txData[2], txData[1]);
    txData[5] = crc >> 8;
    txData[6] = crc & 0xFF;

    serialUart.writeTxFifoMulti(txData, 7);
}

void SendI()
{
    uint8_t txData[7] = { 0x55, 0x03, 0x04, 0x01 };
    txData[4] = g_motor.GetI() * 10;
    uint16_t crc = crc16_modbus(&txData[2], txData[1]);
    txData[5] = crc >> 8;
    txData[6] = crc & 0xFF;

    serialUart.writeTxFifoMulti(txData, 7);
}

void SendMotor()
{
    uint8_t txData[8] = { 0x55, 0x04, 0x05, 0x01 };
    txData[4] = 0;
    txData[5] = g_motor.GetMotorStatus();
    uint16_t crc = crc16_modbus(&txData[2], txData[1]);
    txData[6] = crc >> 8;
    txData[7] = crc & 0xFF;
    serialUart.writeTxFifoMulti(txData, 8);

    txData[4] = 1;
    txData[5] = g_motor.GetMotorDir();
    crc = crc16_modbus(&txData[2], txData[1]);
    txData[6] = crc >> 8;
    txData[7] = crc & 0xFF;
    serialUart.writeTxFifoMulti(txData, 8);
}

void Task50Ms()
{
	if(task50ms.CheckTimeCtrl()) {
		SendRpm();
		SendDuty();
		SendP();
		SendI();
		SendMotor();
	    serialUart.startTx();

		task50ms.ResetTimeCtrl();
	}
}

void Task10Ms()
{
	if(task10ms.CheckTimeCtrl()) {
		g_motor.CalculateRpm(__HAL_TIM_GET_COUNTER(&htim21));
		g_motor.ControlPI();

		task10ms.ResetTimeCtrl();
	}
}

void myMain()
{
	g_handler.AddFuncList(0, CmdPing);
	g_handler.AddFuncList(1, CmdRpm);
	g_handler.AddFuncList(2, CmdDuty);
	g_handler.AddFuncList(3, CmdP);
	g_handler.AddFuncList(4, CmdI);
	g_handler.AddFuncList(5, CmdMotor);

	serialUart.startRx();

	task50ms.SetTimeCtrl(50);
	task10ms.SetTimeCtrl(10);

	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Encoder_Start(&htim21, TIM_CHANNEL_ALL);
	HAL_TIM_PWM_Start(&htim22, TIM_CHANNEL_1);

	g_motor.SetDuty(0);

	while (1)
	{
		processRx();
		Task50Ms();
		Task10Ms();
	}
}
