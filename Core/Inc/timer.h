#pragma once

#include <cstdint>

class Timer {
public:
	uint32_t GetTimeStamp(void);
	void DisableTimeCtrl();
	void SetTimeCtrl(uint32_t interval);
	bool CheckTimeCtrl();
	void ResetTimeCtrl();
private:
	uint32_t m_start;
	uint32_t m_interval;
	bool m_enable;
};


