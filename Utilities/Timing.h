#pragma once
#include <Windows.h>
namespace Timing {
	typedef LARGE_INTEGER Tick;
	float CalLastTimeFrameTime_ms();
	float CalLastTimeFrameTime_FixedValue_ms();

	Tick GetCurrentTickCounter();
	float GetTimeDiff_ms(Tick later_tick);
}