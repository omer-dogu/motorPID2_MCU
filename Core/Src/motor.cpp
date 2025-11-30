#include "motor.h"
#include "main.h"
#include <cmath>

void Motor::SetRpm(uint16_t rpm)
{
	m_targetRpm = rpm;
	m_integral = .0;
	SetDuty(m_targetRpm / 10);
}

void Motor::SetDuty(uint8_t duty)
{
	m_targetDuty = duty;
//	ApplyPwm(duty);
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
	m_integral = 0.0f;
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
	float instantRpm;
    int16_t diff = (int16_t)(resolver - lastCount);
    diff = std::abs(diff);
    lastCount = resolver;
    instantRpm = (float)diff * 6000.0f / m_cpr;

    m_rpmBuffer[m_rpmIndex] = instantRpm;
    m_rpmIndex++;
    if (m_rpmIndex >= RPM_FILTER_N) {
        m_rpmIndex = 0;
        m_rpmFilled = true;
    }

    float sum = 0.0f;
    int count = m_rpmFilled ? RPM_FILTER_N : m_rpmIndex;
    for (int i = 0; i < count; ++i)
        sum += m_rpmBuffer[i];

    m_currentRpm = (count > 0) ? (sum / count) : instantRpm;
    m_currentDuty = m_currentRpm / 10;
}

void Motor::ControlPI(void)
{
    if (!m_enable){
        return;
    }

	float error = m_targetRpm - m_currentRpm;
    uint8_t duty;

	m_integral += error * 0.01f;  // 10 ms => dt = 0.01 s

//	// Anti-windup
//	if (m_integral > 100.0f)  m_integral = 100.0f;
//	if (m_integral < -100.0f) m_integral = -100.0f;

	// --- PI Output ---
	duty = m_Kp * error + m_Ki * m_integral;

	// Sınırla
	if (duty < 0.0f)   duty = 0.0f;
	if (duty > 100.0f) duty = 100.0f;

	// PWM uygula
	ApplyPwm(duty);
}

