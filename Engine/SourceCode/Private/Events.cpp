#include "Events.h"
#include "Utilities\MyOwnAssert.h"

#include<Windows.h>

namespace Engine {
	namespace Threads {
		ManualResetEvent::ManualResetEvent(LPCWSTR i_pName, bool i_bInitiallySignaled) {
			m_Handle = CreateEvent(NULL, TRUE, i_bInitiallySignaled, i_pName);

			ASSERTNL(m_Handle != INVALID_HANDLE_VALUE);
		}

		ManualResetEvent::~ManualResetEvent() {
			CloseHandle(m_Handle);
		}

		void ManualResetEvent::Reset(void) {
			ResetEvent(m_Handle);
		}

		bool ManualResetEvent::Wait(wait_t i_WaitMilliseconds) {
			DWORD result = WaitForSingleObject(m_Handle, i_WaitMilliseconds);
			ASSERTNL(((i_WaitMilliseconds == WaitInfinite) && (result == WAIT_OBJECT_0)) || (result == WAIT_TIMEOUT));
			return result == WAIT_OBJECT_0;
		}
		void ManualResetEvent::Signal() {
			BOOL result = SetEvent(m_Handle);
			ASSERTNL(result == TRUE);
		}

		AutoResetEvent::AutoResetEvent(LPCWSTR i_pName, bool i_bInitiallySignaled) {
			m_Handle = CreateEvent(NULL, TRUE, i_bInitiallySignaled, i_pName);
			ASSERTNL(m_Handle != INVALID_HANDLE_VALUE);
		}

		AutoResetEvent::~AutoResetEvent() {
			CloseHandle(m_Handle);
		}

		bool AutoResetEvent::Wait(wait_t i_WaitMilliseconds) {
			DWORD result = WaitForSingleObject(m_Handle, i_WaitMilliseconds);
			ASSERTNL(((i_WaitMilliseconds == WaitInfinite) && (result == WAIT_OBJECT_0)) || (result == WAIT_TIMEOUT));
			return result == WAIT_OBJECT_0;
		}
		void AutoResetEvent::Signal(void)
		{
			BOOL result = SetEvent(m_Handle);
			ASSERTNL(result == TRUE);
		}
	}
}