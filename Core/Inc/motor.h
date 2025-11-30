#pragma once

#include <cstdint>

class Motor {
public:
	uint16_t GetRpm()const { return m_rpm; }
	void SetRpm(uint16_t rpm) { m_rpm = rpm; }
	uint8_t GetDuty()const { return m_duty; }
	void SetDuty(uint8_t duty) { m_duty = duty; }
private:
	uint16_t m_rpm{};
	uint8_t m_duty{};
};
