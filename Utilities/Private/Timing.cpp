#include <Windows.h>
#include "Timing.h"

namespace Timing {
	const long million = 1000000L;
	LONGLONG g_LastFrameStartTick = 0;
	float CalLastTimeFrameTime_ms() {
		float g_lastFrameTime_ms;
		Tick currentTick;
		BOOL bSuccessfulGettingCounter = QueryPerformanceCounter((Tick *)&currentTick);
		if (bSuccessfulGettingCounter) {
			if (g_LastFrameStartTick) {
				Tick currentFrequency;
				BOOL bSuccessfulGettingFrequency = QueryPerformanceFrequency((Tick *)&currentFrequency);
				if (bSuccessfulGettingFrequency) {
					g_lastFrameTime_ms = (float)((currentTick.QuadPart - g_LastFrameStartTick)*million / currentFrequency.QuadPart);
				}
			}
			else {
				g_lastFrameTime_ms = 13.3f; // assumging 60HZ, so 1/60 = 13.3;
			}
			g_LastFrameStartTick = currentTick.QuadPart;
		}

		return g_lastFrameTime_ms;
	}

	float CalLastTimeFrameTime_FixedValue_ms() {
		return 1000.0f / 60.0f;
	}

	Tick g_LastTick;
	Tick GetCurrentTickCounter() {
		BOOL bSuccessfulGetCounter = QueryPerformanceCounter((Tick *)&g_LastTick);
		if (bSuccessfulGetCounter) {
			return g_LastTick;
		}
		else {
			g_LastTick.QuadPart = 0LL;
			return g_LastTick;
		}
	}

	float GetTimeDiff_ms(Tick later_tick) {
		Tick currentFrequency;
		BOOL bSuccessfulGettingFrequency = QueryPerformanceFrequency((Tick *)&currentFrequency);
		if (bSuccessfulGettingFrequency) {
			return (float)((later_tick.QuadPart - g_LastTick.QuadPart)*million / currentFrequency.QuadPart);
		}
		else{
			return 0;
		}
	}
}