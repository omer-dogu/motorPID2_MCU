#include "handler.h"

void Handler::AddFuncList(uint8_t idx, CmdHandler h)
{
	CmdFuncList[idx] = h;
}

Handler::CmdHandler Handler::GetFuncList(uint8_t idx)
{
	return CmdFuncList[idx];
}
