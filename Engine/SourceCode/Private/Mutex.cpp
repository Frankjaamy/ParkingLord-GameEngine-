#include <windows.h>

#include "Mutex.h"
#include"Utilities\MyOwnAssert.h"

namespace Engine
{
	namespace Threads {
		Mutex::Mutex( LPCWSTR i_pName, bool i_bTakeOwnership)
		{
			m_Handle = CreateMutex( NULL, (BOOL) i_bTakeOwnership, i_pName );
			ASSERTNL( m_Handle != NULL );
		}

		Mutex::~Mutex()
		{
			BOOL result = CloseHandle( m_Handle );
			ASSERTNL( result == TRUE );
		}

		bool Mutex::TryAcquire( void )
		{
			DWORD result = WaitForSingleObject( m_Handle, 0 );

			return result == WAIT_OBJECT_0;
		}

		void Mutex::Acquire( void )
		{
			DWORD result = WaitForSingleObject( m_Handle, INFINITE );
			ASSERTNL( result == WAIT_OBJECT_0 );

		}

		bool Mutex::Acquire( wait_t i_WaitMilliseconds )
		{
			DWORD result = WaitForSingleObject( m_Handle, i_WaitMilliseconds );
			ASSERTNL( ( ( i_WaitMilliseconds == WaitInfinite ) && ( result == WAIT_OBJECT_0 ) ) || ( result == WAIT_TIMEOUT ) );

			return result == WAIT_OBJECT_0;
		}

		void Mutex::Release( void )
		{
			BOOL result = ReleaseMutex( m_Handle );
			ASSERTNL( result == TRUE );
		}
	}
} // namespace Engine