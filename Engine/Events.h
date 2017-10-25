#pragma once
#include <Windows.h>

#include "WaitableObjects.h"

namespace Engine {
	namespace Threads {
		class ManualResetEvent : public WaitableObject
		{
		public:
			ManualResetEvent(LPCWSTR i_pName, bool i_bInitiallySignaled);
			~ManualResetEvent();

			void Reset();
			bool Wait(wait_t i_WaitMilliseconds);
			void Signal();
		};

		class AutoResetEvent :public WaitableObject
		{
			AutoResetEvent(LPCWSTR i_pName, bool i_bInitiallySignaled);
			~AutoResetEvent();

			bool Wait(wait_t i_WaitMilliseconds);
			void Signal();
		};
	}
}