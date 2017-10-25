#include "Semaphore.h"

#include "Utilities\MyOwnAssert.h"
#include <Windows.h>

namespace Engine
{
	namespace Threads {
		Semaphore::Semaphore( unsigned int i_initCount, unsigned int i_maxCount, LPCWSTR i_pName )
		{
			ASSERTNL( i_initCount <= i_maxCount );

			m_Handle = CreateSemaphore( NULL, i_initCount, i_maxCount, i_pName );
			ASSERTNL( m_Handle != NULL );
		}

		Semaphore::~Semaphore()
		{
			BOOL result = CloseHandle( m_Handle );
			ASSERTNL( result == TRUE );
		}

		// should 
		void Semaphore::Increment( unsigned int i_count )
		{
			BOOL result = ReleaseSemaphore( m_Handle, i_count, NULL );
			ASSERTNL( result == TRUE );
		}

		void Semaphore::Decrement( void )
		{
			DWORD result = WaitForSingleObject( m_Handle, INFINITE );
			ASSERTNL( result == WAIT_OBJECT_0 );
		}

		bool Semaphore::Decrement( wait_t i_WaitMilliseconds )
		{
			DWORD result = WaitForSingleObject( m_Handle, i_WaitMilliseconds );
			ASSERTNL( ( ( i_WaitMilliseconds == WaitInfinite ) && ( result == WAIT_OBJECT_0 ) ) || ( result == WAIT_TIMEOUT ) );

			return result == WAIT_OBJECT_0;
		}
	}
} // namespace Azra
