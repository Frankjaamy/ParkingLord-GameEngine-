#include <inttypes.h>
#include <new.h>

#include "AllocatorOperation.h"
#include "Utilities\MyOwnAssert.h"
namespace Engine {
	namespace Memory {
		void * pMemory = nullptr;
		MyHeapManager * pHeap = nullptr;		

		void MemoryAllocate() {
			//no need to allocate here. See new function.
		}
		void MemoryClear() {
#if _USE_COSTUM_HEAP_
			if (pHeap && pMemory) {
				pHeap->destroyFSAs();
				pHeap->destroyHeap();
				pHeap = nullptr;

				_aligned_free(pMemory);
			}
#endif
		}
	}
}
using namespace Engine::Memory;

void * operator new(size_t i_size)
{
#if _USE_COSTUM_HEAP_
	if (!pMemory) {
		size_t memoryChunkSIze = (1024 * 1024 * 20);
		const size_t sizeTotal = memoryChunkSIze + sizeof(MyHeapManager);
		const size_t numOfDescriptors = 4096;

		pMemory = _aligned_malloc(sizeTotal, 4);
		pHeap = MyHeapManager::create(pMemory);
		ASSERT(pHeap, "creating HeapManager Failed!");

		uint8_t * pHeapMemory = reinterpret_cast<uint8_t *>(pMemory) + sizeof(MyHeapManager);
		size_t sizeHeap = sizeTotal - sizeof(MyHeapManager);

		pHeap->initializeHeap(reinterpret_cast<void *>(pHeapMemory), sizeHeap, numOfDescriptors);
		//create allocators and fixedsizeallocators;
		pHeap->createCompleteHeaps();
	}

	ASSERT(pHeap && pMemory, "Getting Memory and Heap Failed!");
	bool isFromFsa;
	return pHeap->_alloc(i_size, Alignment_4, isFromFsa);
#else
	Log("Calling new for memory of %Iu size using default.\n", i_size);
	return _aligned_malloc(i_size, g_DefaultAlignment);
#endif // _USE_COSTUM_HEAP_

}

void * operator new [](size_t i_size)
{
#if _USE_COSTUM_HEAP_
	if (!pMemory) {
		const size_t sizeTotal = 1024 * 1024 * 200 + sizeof(MyHeapManager);
		const size_t numOfDescriptors = 4096;

		pMemory = _aligned_malloc(sizeTotal, 4);
		pHeap = MyHeapManager::create(pMemory);
		ASSERT(pHeap, "creating HeapManager Failed!");

		uint8_t * pHeapMemory = reinterpret_cast<uint8_t *>(pMemory) + sizeof(MyHeapManager);
		size_t sizeHeap = sizeTotal - sizeof(MyHeapManager);

		pHeap->initializeHeap(reinterpret_cast<void *>(pHeapMemory), sizeHeap, numOfDescriptors);
		//create allocators and fixedsizeallocators;
		pHeap->createCompleteHeaps();
}

	ASSERT(pHeap && pMemory, "Getting Memory and Heap Failed!");
	bool isFromFsa;
	return pHeap->_alloc(i_size, Alignment_4, isFromFsa);
#else
	Log("Calling new for memory of %Iu size using default.\n", i_size);
	return _aligned_malloc(i_size, g_DefaultAlignment);
#endif // _USE_COSTUM_HEAP_
}


void operator delete(void * i_ptr)
{
#if _USE_COSTUM_HEAP_
	ASSERT(pHeap && pMemory, "Getting Memory and Heap Failed!");
	bool isSuccess = pHeap->_free(i_ptr);
	ASSERT(isSuccess, "Freeing Memory Failed!");
#else
	if (i_ptr) {
		_aligned_free(i_ptr);
	}
	return;
#endif
}

void operator delete [] (void * i_ptr)
{
#if _USE_COSTUM_HEAP_
	ASSERT(pHeap && pMemory, "Getting Memory and Heap Failed!");
	bool isSuccess = pHeap->_free(i_ptr);
	ASSERT(isSuccess, "Freeing Memory Failed!");
#else
	if (i_ptr) {
		_aligned_free(i_ptr);
	}
	return;
#endif
}
