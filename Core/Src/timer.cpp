/*
 * timer.c
 *
 *  Created on: Nov 18, 2025
 *      Author: omer
 */
#include "timer.h"
#include "main.h"

uint32_t timeStamp1ms;

uint32_t getTimeStamp(void)
{
	return timeStamp1ms;
}

void disableTimeCtrl(timeCtrl_t* timeCtrl)
{
	timeCtrl->enable = 0;
}

void setTimeCtrl(timeCtrl_t* timeCtrl, uint32_t interval)
{
	timeCtrl->enable = 1;
	timeCtrl->interval = interval;
	timeCtrl->start = getTimeStamp();
}

int checkTimeCtrl(timeCtrl_t* timeCtrl)
{
	if (timeCtrl->enable)
		return getTimeStamp() >= (timeCtrl->start + timeCtrl->interval);

	return 0;
}

void resetTimeCtrl(timeCtrl_t* timeCtrl)
{
	if (timeCtrl->enable)
		timeCtrl->start += timeCtrl->interval;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM2) {
    	timeStamp1ms++;
    }
}
