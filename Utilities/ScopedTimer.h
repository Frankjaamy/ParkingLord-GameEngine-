#pragma once
#include"ProfilingTool.h"
#include "Timing.h"
using namespace Utilities::Profile;
class ScopedTimer
{
public:
	ScopedTimer(Accumulator & i_MyAccumulator) :
		m_Start(Timing::GetCurrentTickCounter()),
		m_pAccumulator(&i_MyAccumulator)
	{ }

	~ScopedTimer()
	{
		*m_pAccumulator += Timing::GetTimeDiff_ms(Timing::GetCurrentTickCounter());
	}

private:
	Timing::Tick	m_Start;
	Accumulator *	m_pAccumulator;
	const char *	m_pScopeName;
};