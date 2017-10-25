#pragma once
#include "MemoryAllocator\Allocator.h"
#include "MemoryAllocator\MyHeapManager.h"

#define _USE_COSTUM_HEAP_ 1
namespace Engine {
	namespace Memory {
		enum Alignment {
			Alignment_Default,
			Alignment_4 = 4,
			Alignment_16 = 16,
			Alignment_32 = 32
		};
		const int g_DefaultAlignment = 4;
		void MemoryAllocate();
		void MemoryClear();
	}
}
//standard new/delete
void * operator new(size_t i_size);

void * operator new[](size_t i_size);

void operator delete(void * i_ptr);
void operator delete[](void * i_ptr);



