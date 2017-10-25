#ifndef __MUTEX_H
#define __MUTEX_H

#include "WaitableObjects.h"

namespace Engine
{
	namespace Threads {
		class Mutex : public WaitableObject
		{
		public:
			Mutex( LPCWSTR i_pName = nullptr,bool i_bTakeOwnership = false );
			~Mutex();

			bool TryAcquire();
			void Acquire();
			bool Acquire( wait_t i_WaitMilliseconds );
			void Release();

		};
	}
} // namespace Engine

#endif // __MUTEX_H
