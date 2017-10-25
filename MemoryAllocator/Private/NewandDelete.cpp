#include "NewandDelete.h"
#include "MyHeapManager.h"
void * operator new(size_t i_size, MyHeapManager * i_pHeap) {

	Log("Trying to get memory from Allocator..\n");
	ASSERT(i_pHeap != nullptr, "Error: Failed to get HeapManager.");


	void * ptr = nullptr;
	FixedSizeAllocator * pFsa = i_pHeap->getFixedAllocator(i_size);
	if (pFsa)
		ptr = pFsa->alloc();
	if (!ptr) {
		ptr = i_pHeap->getHeap()->_align_alloc(i_size, g_DefaultAlignment);
	}
	Log("returning the pointer of % " PRIxPTR ". \n", reinterpret_cast<uintptr_t>(ptr));
	return ptr;
}
void operator delete(void * i_ptr, MyHeapManager * i_pHeap) {
	Log("Trying to free memory from Allocator..\n");
	if (i_ptr) {
		ASSERT(i_pHeap != nullptr, "Error: Failed to get HeapManager.");

		for (size_t i = 0; i < i_pHeap->getFSASize(); i++) {
			FixedSizeAllocator * fsa = i_pHeap->getFixedAllocatotByIndex(i);
			if (fsa->isContained(reinterpret_cast<int8_t *>(i_ptr))) {
				Log("Successfully returned the memory by a FSA.\n");
				fsa->_free(i_ptr);
				return;
			}
		}

		if (i_pHeap->getHeap()->_free(i_ptr)) {
			Log("Successfully returned the memory by default heap.\n");
		}
		else {
			Log("Free failed. Check for log.\n");
		}
	}
	return;
}

void * operator new(size_t i_size, MyHeapManager * i_pHeap, unsigned int i_align) {
	Log("Trying to get memory from Allocator..\n");

	ASSERT(i_pHeap != nullptr, "Error: Failed to get HeapManager.");

	void * ptr = i_pHeap->getHeap()->_align_alloc(i_size, i_align);

	Log("returning the pointer of % " PRIxPTR ". \n", reinterpret_cast<uintptr_t>(ptr));

	return ptr;
}
void operator delete(void * i_ptr, MyHeapManager * i_pHeap, unsigned int i_align) {
	Log("Trying to free memory from Allocator..\n");
	if (i_ptr) {
		ASSERT(i_pHeap != nullptr, "Error: Failed to get HeapManager.");

		if (i_pHeap->getHeap()->_free(i_ptr)) {
			Log("Successfully returned the memory.\n");
		}
		else {
			Log("Free failed. Check for log.\n");
		}

	}
	return;
}


void * operator new(size_t i_size, Allocator * i_pHeap) {

	Log("Trying to get memory from Allocator..\n");
	ASSERT(i_pHeap != nullptr, "Error: Failed to get Allocator.");

	void * ptr = nullptr;
	if (!ptr) {
		ptr = i_pHeap->_align_alloc(i_size, g_DefaultAlignment);
	}
	Log("returning the pointer of % " PRIxPTR ". \n", reinterpret_cast<uintptr_t>(ptr));
	return ptr;
}
void operator delete(void * i_ptr, Allocator * i_pHeap) {
	Log("Trying to free memory from Allocator..\n");
	if (i_ptr) {
		ASSERT(i_pHeap != nullptr, "Error: Failed to get Allocator.");

		if (i_pHeap->_free(i_ptr)) {
			Log("Successfully returned the memory.\n");
		}
		else {
			Log("Free failed. Check for log.\n");
		}
	}
	return;
}

void * operator new(size_t i_size, Allocator * i_pHeap, unsigned int i_align) {
	Log("Trying to get memory from Allocator..\n");

	ASSERT(i_pHeap != nullptr, "Error: Failed to malloc new memory.");

	void * ptr = i_pHeap->_align_alloc(i_size, i_align);

	Log("returning the pointer of % " PRIxPTR ". \n", reinterpret_cast<uintptr_t>(ptr));

	return ptr;
}
void operator delete(void * i_ptr, Allocator * i_pHeap, unsigned int i_align) {
	Log("Trying to free memory from Allocator..\n");
	if (i_ptr) {
		ASSERT(i_pHeap != nullptr, "Error: Failed to malloc new memory.");
		if (i_pHeap->_free(i_ptr)) {
			Log("Successfully returned the memory.\n");
		}
		else {
			Log("Free failed. Check for log.\n");
		}

	}
	return;
}