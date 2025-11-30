#pragma once

#include <cstdint>

class Motor {
public:
	uint16_t GetRpm()const { return m_rpm; }
	void SetRpm(uint16_t rpm) { m_rpm = rpm; }
	uint8_t GetDuty()const { return m_duty; }
	void SetDuty(uint8_t duty);
	uint8_t GetP()const { return m_p; }
	void SetP(uint8_t p) { m_p = p; }
	uint8_t GetI()const { return m_i; }
	void SetI(uint8_t i) { m_i = i; }
	void CalculateRpm(uint16_t resolver);

	void MotorEnable();
	void MotorDisable();
	void MotorCW();
	void MotorCCW();
	bool GetMotorStatus();
	bool GetMotorDir();
private:
	uint16_t m_rpm{};
	uint8_t m_duty{};
	uint8_t m_p{};
	uint8_t m_i{};
	bool m_enable{false};
	bool m_dir{false};
	const double m_cpr{810.0};
};
