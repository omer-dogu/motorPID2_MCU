#pragma once

#include <cstdint>

class Handler {
public:
	using CmdHandler = void(*)(const uint8_t* data);
	void AddFuncList(uint8_t idx, CmdHandler h);
	CmdHandler GetFuncList(uint8_t idx);
private:
	CmdHandler CmdFuncList[256] = { nullptr };
};
