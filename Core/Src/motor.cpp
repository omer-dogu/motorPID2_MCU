#include "motor.h"
#include "main.h"
#include <cmath>

void Motor::SetDuty(uint8_t duty)
{
	if (duty > 100) duty = 100;
	m_duty = duty;
    __HAL_TIM_SET_COMPARE(&htim22, TIM_CHANNEL_1, (duty * 799) / 100);
}

void Motor::MotorEnable()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	m_enable = true;
}
void Motor::MotorDisable()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	m_enable = false;
}
void Motor::MotorCW()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	m_dir = false;
}
void Motor::MotorCCW()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	m_dir = true;
}
bool Motor::GetMotorStatus()
{
	return m_enable;
}
bool Motor::GetMotorDir()
{
	return m_dir;
}
void Motor::CalculateRpm(uint16_t resolver)
{
	static uint16_t lastCount = 0;
    int16_t diff = (int16_t)(resolver - lastCount);
    lastCount = resolver;

    m_rpm = (float)std::abs(diff) * 6000.0f / m_cpr;
}

