/*
 * timer.h
 *
 *  Created on: Nov 18, 2025
 *      Author: omer
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include <stdint.h>

typedef struct {
	uint32_t start;
	uint32_t interval;
	uint32_t enable;
}timeCtrl_t;

uint32_t getTimeStamp(void);
void disableTimeCtrl(timeCtrl_t* timeCtrl);
void setTimeCtrl(timeCtrl_t* timeCtrl, uint32_t interval);
int checkTimeCtrl(timeCtrl_t* timeCtrl);
void resetTimeCtrl(timeCtrl_t* timeCtrl);

#endif /* INC_TIMER_H_ */
