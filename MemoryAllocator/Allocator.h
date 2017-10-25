#pragma once
#include<stdlib.h>
#include<assert.h>
#include<stdint.h>

struct DescriptionBlock
{	
	void * pBlock;
	size_t iBlockSize;
	intptr_t offset;
	DescriptionBlock * next;
	DescriptionBlock * prev;

	void init() {
		pBlock = nullptr;
		iBlockSize = 0;
		offset = 0;
		next = prev = nullptr;
	}
	DescriptionBlock():pBlock(nullptr),iBlockSize(0),next(nullptr),offset(0){
	}
};

class Allocator {
public:
	Allocator():isFreeMemoryCollected(false) {

	}
	void destroy();
	//create&initialize
	
	void initialize(void * memory, size_t sizeOfMem, size_t numberOfBlocks);

	//main functions
	void * _alloc(size_t size);
	
	void * _align_alloc(size_t size, size_t i_alignment);
	
	bool _free(void * ptr);

	//garbage collection
	void garbageCollection();
	bool isTwoBlocksAbut(DescriptionBlock *pLeftBlock,DescriptionBlock * pRightBlock) const;
	DescriptionBlock * combineBlocksTogether(DescriptionBlock * block1, DescriptionBlock * block2);

	//utilities
	bool isContained(void * pPtr) const;
	bool isAllocated(void * pPtr) const;
/*
	@parameter option:
		0: to see freeMemoryList;
		1: to see inUse/Outstanding MemoryList;
		2: to see availableMemoryList;
*/
	void visualizeAllBlockLists(int option) const;

	int getBlockArrayLength(int option) const;

	uintptr_t calc_align(uintptr_t n, uintptr_t align);

private:
	~Allocator() {

	}
	DescriptionBlock * pFreeMemoryList;
	DescriptionBlock * pInUseMemoryList;
	DescriptionBlock * pAvailableBlocksList;

	int8_t * pTotalMemory;
	int8_t * pPoolMemory;
	bool isFreeMemoryCollected;
};