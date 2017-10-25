#pragma once
#include <inttypes.h>
#include <new.h>
#include "Allocator.h"
#include "FixedSizeAllocator.h"
#include "Utilities\MyOwnAssert.h"
class MyHeapManager {
public:
	static MyHeapManager * create(void * memoryToPlace);

	void initializeHeap(void *, size_t sizeHeap, size_t numOfBlocks);
	void createCompleteHeaps();

	FixedSizeAllocator * getFixedAllocator(size_t size);
	inline Allocator * getHeap() const { return pAllocator; }

	void * _alloc(size_t size, size_t alignment, bool & isFromFSA);
	bool _free(void * ptr);
	bool isAllocated(void * ptr) const;

	void destroyHeap();
	void destroyFSAs();

	inline FixedSizeAllocator * getFixedAllocatotByIndex(size_t index) const
	{
		ASSERT(index<FSASize, "index out of bounds!"); return fsaArray[index];
	}
	inline const size_t getFSASize() const { return FSASize; }
private:
	static const size_t FSASize = 4;
	struct FSAInitData {
		size_t fixedBlockSize;
		size_t numOfBlocks;
		FSAInitData(size_t blockSize, size_t blocks) : fixedBlockSize(blockSize), numOfBlocks(blocks) {
		}
	};
	const FSAInitData fsaData[FSASize] = {
		FSAInitData(16,1024),
		FSAInitData(32,2048),
		FSAInitData(64,2048),
		FSAInitData(128,2048)
	};
private:
	FixedSizeAllocator * fsaArray[FSASize];
	Allocator * pAllocator;
private:
	~MyHeapManager();
	MyHeapManager() :pAllocator(nullptr) {
		for (size_t i = 0; i < FSASize; i++) {
			fsaArray[i] = nullptr;
		}
	}
	void createFixedSizeAllocator(size_t fixedSize, size_t numOFBlocks, Allocator * heap);
};
