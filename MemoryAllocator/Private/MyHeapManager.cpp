#include <inttypes.h>
#include <new.h>
#include "NewandDelete.h"
#include "MyHeapManager.h"
#include "Utilities\MyOwnAssert.h"

MyHeapManager::~MyHeapManager() {
}

MyHeapManager * MyHeapManager::create(void * memoryToPlace) {
	static MyHeapManager * ptr = nullptr;
	if (!ptr) {
		ptr = new(memoryToPlace) MyHeapManager();
	}
	return ptr;
}

void MyHeapManager::initializeHeap(void *memory, size_t sizeHeap, size_t numOfBlocks) {

	ASSERT(sizeHeap > sizeof(Allocator) + (sizeof(DescriptionBlock)*numOfBlocks) + 2 * g_DefaultAlignment, "Wrong initial size!");

	uint8_t *pHeapMem = reinterpret_cast<uint8_t *>(memory) + sizeof(Allocator);
	sizeHeap -= sizeof(Allocator);

	pAllocator = new(memory) Allocator();
	ASSERT(pAllocator, "heap create failed!");
	pAllocator->initialize(reinterpret_cast<void*>(pHeapMem), sizeHeap, numOfBlocks);
}

void MyHeapManager::createCompleteHeaps() {
	Allocator * pAllocator = getHeap();
	ASSERT(pAllocator, "Get Default Heap Failed!");

	for (size_t i = 0; i < FSASize; i++) {
		createFixedSizeAllocator(fsaData[i].fixedBlockSize, fsaData[i].numOfBlocks, pAllocator);
	}
}

void MyHeapManager::createFixedSizeAllocator(size_t fixedSize, size_t numOFBlocks, Allocator * heap) {
	size_t sizeNeeded = (numOFBlocks + 7) / 8 + fixedSize * numOFBlocks;
	void * pMemory = heap->_align_alloc(sizeNeeded, 4);
	ASSERT(pMemory, "Allocate Block Memory Failed!");

	static size_t index = 0;
	FixedSizeAllocator * fsa = new (heap) FixedSizeAllocator(heap, pMemory, fixedSize, numOFBlocks);
	ASSERT(fsa, "Allocate FSA Failed!");

	fsaArray[index] = fsa;
	index++;
}

FixedSizeAllocator * MyHeapManager::getFixedAllocator(size_t size) {
	for (size_t i = 0; i < FSASize; i++) {
		size_t fixedSize = fsaData[i].fixedBlockSize;
		if (size <= fixedSize) {
			return fsaArray[i];
		}
	}
	return nullptr;
}

void * MyHeapManager::_alloc(size_t size, size_t alignment, bool & isFromFSA) {
	//Log("Trying to get memory from HeapManager..\n");
	void * ptr = nullptr;
	FixedSizeAllocator * pFsa = getFixedAllocator(size);
	if (pFsa && !(pFsa->isEmpty())) {
		ptr = pFsa->alloc();
		isFromFSA = true;

		Log("returning %" PRIxPTR " by FSA-%d. \n", reinterpret_cast<uintptr_t>(ptr), static_cast<int>(pFsa->getFixedSize()));
	}
	if (!ptr) {
		ptr = getHeap()->_align_alloc(size, alignment);
		isFromFSA = false;

		Log("returning %" PRIxPTR ". \n", reinterpret_cast<uintptr_t>(ptr));
	}

	return ptr;
}

bool MyHeapManager::_free(void * ptr) {
	//Log("Trying to free memory from HeapManager..\n");
	for (size_t i = 0; i < getFSASize(); i++) {
		FixedSizeAllocator * fsa = getFixedAllocatotByIndex(i);
		if (fsa) {
			if (fsa->isContained(reinterpret_cast<int8_t *>(ptr))) {
				Log("freeing % " PRIxPTR " from a FSA - %d.\n", reinterpret_cast<uintptr_t>(ptr), static_cast<int>(fsa->getFixedSize()));
				fsa->_free(ptr);
				Log("Successfully freeed % " PRIxPTR " the memory by a FSA - %d.\n", reinterpret_cast<uintptr_t>(ptr), static_cast<int>(fsa->getFixedSize()));
				return true;
			}
		}
	}
	Log("freeing % " PRIxPTR " from default heap.\n", reinterpret_cast<uintptr_t>(ptr));
	if (getHeap()->_free(ptr)) {
		Log("Successfully returned the memory.\n");
		return true;
	}
	else {
		Log("Free failed. Check for log.\n");
		return false;
	}
}

void MyHeapManager::destroyFSAs() {
	for (size_t i = 0; i < getFSASize(); i++) {
		if (fsaArray[i]) {
			fsaArray[i]->destroySelf();
			getHeap()->_free(fsaArray[i]);
		}
	}
}

void MyHeapManager::destroyHeap() {
	getHeap()->destroy();
}

bool MyHeapManager::isAllocated(void * ptr) const {
	Log("checking the pointer of % " PRIxPTR ". \n", reinterpret_cast<uintptr_t>(ptr));
	for (size_t i = 0; i < getFSASize(); i++) {
		if (fsaArray[i]) {
			if (fsaArray[i]->isContained(ptr)) {
				uintptr_t flagPosition;
				if (fsaArray[i]->isAllocated(ptr, flagPosition)) {
					return true;
				}
			}
		}
	}
	if (getHeap()->isAllocated(ptr)) {
		return true;
	}
	else {
		Log("checking the pointer of % " PRIxPTR ". \n", reinterpret_cast<uintptr_t>(ptr));
		return false;
	}
}