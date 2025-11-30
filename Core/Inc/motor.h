#pragma once

#include <cstdint>

class Motor {
public:
	uint16_t GetRpm()const { return m_currentRpm; }
	void SetRpm(uint16_t rpm);
	uint8_t GetDuty()const { return m_currentDuty; }
	void SetDuty(uint8_t duty);
	float GetP()const { return m_Kp; }
	void SetP(float p) { m_Kp = p; }
	float GetI()const { return m_Ki; }
	void SetI(float i) { m_Ki = i; }
	void CalculateRpm(uint16_t resolver);
	void ApplyPwm(uint8_t duty);
	void ControlPI(void);

	void MotorEnable();
	void MotorDisable();
	void MotorCW();
	void MotorCCW();
	bool GetMotorStatus();
	bool GetMotorDir();
private:
	uint16_t m_targetRpm{};
	uint16_t m_currentRpm{};
	uint8_t m_targetDuty{};
	uint8_t m_currentDuty{};
	float m_Kp{};
	float m_Ki{};
	float m_integral{};
	bool m_enable{false};
	bool m_dir{false};
	const double m_cpr{810.0};
};
