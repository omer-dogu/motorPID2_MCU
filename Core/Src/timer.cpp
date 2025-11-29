/*
 * timer.c
 *
 *  Created on: Nov 18, 2025
 *      Author: omer
 */
#include "timer.h"
#include "main.h"

uint32_t Timer::s_timeStamp1ms = 0;

uint32_t Timer::GetTimeStamp(void)
{
	return s_timeStamp1ms;
}

void Timer::DisableTimeCtrl()
{
	m_enable = false;
}

void Timer::SetTimeCtrl(uint32_t interval)
{
	m_enable = true;
	m_interval = interval;
	m_start = GetTimeStamp();
}

bool Timer::CheckTimeCtrl()
{
	if (m_enable)
		return GetTimeStamp() >= (m_start + m_interval);

	return false;
}

void Timer::ResetTimeCtrl()
{
	if (m_enable)
		m_start += m_interval;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM2) {
    	Timer::s_timeStamp1ms++;
    }
}
