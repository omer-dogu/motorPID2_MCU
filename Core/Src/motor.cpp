#include "motor.h"
#include "main.h"
#include <cmath>

void Motor::SetRpm(uint16_t rpm)
{
	m_targetRpm = rpm;
	m_targetDuty = rpm / 10;
}

void Motor::SetDuty(uint8_t duty)
{
	m_targetDuty = duty;
	m_targetRpm = duty * 10;
}

void Motor::ApplyPwm(uint8_t duty)
{
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

    m_currentRpm = (float)std::abs(diff) * 6000.0f / m_cpr;
}

void Motor::ControlPI(void)
{
	float error = m_targetRpm - m_currentRpm;
	uint8_t pwm_output;

	if (m_targetRpm <= 1.0f)
	{
		m_integral = 0.0f;
		ApplyPwm(0);
		return;
	}

	// --- Integral ---
	m_integral += error * 0.01f;  // 10 ms => dt = 0.01 s

	// Anti-windup
	if (m_integral > 100.0f)  m_integral = 100.0f;
	if (m_integral < -100.0f) m_integral = -100.0f;

	// --- PI Output ---
	pwm_output = m_Kp * error + m_Ki * m_integral;

	// Sınırla
	if (pwm_output < 0.0f)   pwm_output = 0.0f;
	if (pwm_output > 100.0f) pwm_output = 100.0f;

	// PWM uygula
	ApplyPwm(pwm_output);
}

