#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "WaitableObjects.h"

namespace Engine
{
	namespace Threads {
		class Semaphore : public WaitableObject
		{
		public:
			Semaphore( unsigned int i_initCount, unsigned int i_maxCount, LPCWSTR i_pName = nullptr );
			~Semaphore();

			void Increment( unsigned int i_Count = 1 );
			void Decrement();
			bool Decrement( wait_t i_WaitMilliseconds );
		};
	}
} // namespace Engine

#endif // __SEMAPHORE_H