#pragma once
#include "BitArray.h"
class Allocator;
class FixedSizeAllocator {
public:
	FixedSizeAllocator(Allocator * heap, void * pMemoryToStart, size_t fixedSizeWanted, size_t numOfBlocks);
	~FixedSizeAllocator();

	void * alloc();
	
	void _free(void * ptr);
	bool isContained(void * i_ptr) const;
	bool isAllocated(void *, uintptr_t & o_flagPosition) const;
	void destroySelf();

	inline const size_t getFixedSize() const { return fixedSize; }
	inline const bool isEmpty() const { return empty; }
private:
	BitArray * m_pAllocFlagArray;
	int8_t * pPoolMemory;
	int8_t * pStartMemory;
	Allocator * pHeap;
	bool empty;

	size_t fixedSize;
	size_t totalSize;
};