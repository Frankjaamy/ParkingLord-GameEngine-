#pragma once
#include<limits>
#include<stdint.h>

namespace Utilities {
	namespace Profile {
		struct Accumulator
		{
			double		m_Sum;
			uint64_t	m_Count;
			float		m_Min, m_Max;

			Accumulator(const char * name) :
				m_Sum(0.0),
				m_Count(0),
				m_Max(std::numeric_limits<float>::min()),
				m_Min(std::numeric_limits<float>::max())
			{
				//g_Profiler.RegisterAccumulator(name,*this);
			}

			void operator+=(float i_time)
			{
				m_Sum += i_time;
				m_Count++;

				if (i_time > m_Max)
					m_Max = i_time;
				else if (i_time < m_Min)
					m_Min = i_time;
			}

			float average()
			{
				return (float)(m_Sum / m_Count);
			}
		};	


		class Profiler
		{
			std::map<const char *, Accumulator *> m_AllAccumulators;

		public:
			void RegisterAccumulator(const char * i_pName, Accumulator & i_Accumulator) {
				m_AllAccumulators[i_pName] = &i_Accumulator;
			}
		};
		extern Profiler g_Profiler;
	}
}