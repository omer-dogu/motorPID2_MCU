#pragma once

#include <cstdint>

class Motor {
public:
	uint16_t GetRpm()const { return m_rpm; }
	void SetRpm(uint16_t rpm) { m_rpm = rpm; }
private:
	uint16_t m_rpm;
};
