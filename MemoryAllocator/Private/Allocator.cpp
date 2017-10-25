#include <assert.h>
#include <stdio.h>
#include <memory.h>
#include <new>

#include "Allocator.h"
#include "Utilities.h"
#include "Utilities\MyOwnAssert.h"

#define _GUARD_BAND_ 1
using namespace AllocatorTools;

uintptr_t Allocator::calc_align(uintptr_t n, uintptr_t align) {
	return ((n - align + 1) & (~(align - 1)));
}

void Allocator::initialize(void * memory, size_t sizeOfMem, size_t numberOfBlocks) {
	pTotalMemory = pPoolMemory = reinterpret_cast<int8_t *>(memory);
 	size_t blockTotalSize = numberOfBlocks * sizeof(DescriptionBlock);
	ASSERT(blockTotalSize < sizeOfMem,"too many blocks set!");
	pPoolMemory = pTotalMemory + blockTotalSize;
	int8_t * p = pTotalMemory;
	// initializing available blocks list
	pAvailableBlocksList = (DescriptionBlock *)p;
	p += (sizeof(DescriptionBlock));
	pInUseMemoryList = (DescriptionBlock *)p;
	p += (sizeof(DescriptionBlock));
	pFreeMemoryList = (DescriptionBlock *)p;

	pAvailableBlocksList->init();
	pInUseMemoryList->init();
	pFreeMemoryList->init();

	for (unsigned int i = 0;i < numberOfBlocks;i++) {
		p += (sizeof(DescriptionBlock));
		DescriptionBlock * db = reinterpret_cast<DescriptionBlock *>(p);
		db->init();
		DescriptionBlock * p1 = pAvailableBlocksList;
		DescriptionBlock * p2 = pAvailableBlocksList->next;
		for (unsigned int j = 0;j < i;j++) {
			p1 = p2;
			p2 = p2->next;
		}
		if (i == 0) {
			pAvailableBlocksList->next = db;
			db->prev = pAvailableBlocksList;
		}
		else {			
			p2 = db;
			p1->next = p2;
			p2->prev = p1;
		}

	}
	//describe the pool and initializing FreeMemoryList;
	pFreeMemoryList->next = nullptr;
	DescriptionBlock * block = pAvailableBlocksList->next;
	pAvailableBlocksList->next = block->next;
	block->next->prev = pAvailableBlocksList;

	block->next = pFreeMemoryList->next;
	pFreeMemoryList->next = block;
	block->prev = pFreeMemoryList;
	
	uintptr_t initialguardband = 0;
#if _GUARD_BAND_
	initialguardband = 4;
#endif
	block->pBlock = reinterpret_cast<void *>((reinterpret_cast<uintptr_t>(pPoolMemory)+sizeof(uintptr_t) + initialguardband));

	block->iBlockSize = sizeOfMem - blockTotalSize;
	pFreeMemoryList->iBlockSize = block->iBlockSize;
	pFreeMemoryList->pBlock = block->pBlock;
	//initializing inUseMemory;
	pInUseMemoryList->next = nullptr;
}
void * Allocator::_alloc(size_t size) {
	const size_t defaultAlignment = 4;
	return _align_alloc(size, defaultAlignment);
}
void * Allocator::_align_alloc(size_t size, size_t i_alignment) {
	size_t sizeBetterNeed = (size+sizeof(uintptr_t));
#if _GUARD_BAND_
	//add more 8 bytes to do guardbands
	sizeBetterNeed += 8;
#endif
	//let us search the list of free memory list.
	DescriptionBlock * block = pFreeMemoryList->next;
	DescriptionBlock * temp = pFreeMemoryList;

	while (1) {
		if (!block) {
			//this means there is not a single block that we can use right now.
			//do the garbage collection and try again.
			printf("There is no available memory!\n");
			return nullptr;
		}
		if (block && (block->iBlockSize > sizeBetterNeed)) {
			// good, we find one block, now let's check.
			uintptr_t pEnd = reinterpret_cast<uintptr_t>(block->pBlock) + block->iBlockSize-sizeof(uintptr_t)-block->offset;
#if _GUARD_BAND_
			//have four bytes for guardbands.
			pEnd -= 8;
#endif
			pEnd -= size;
			//now let's see what is the aligned address
			uintptr_t pPos = calc_align(pEnd, i_alignment);
			if (pPos < (reinterpret_cast<uintptr_t>(block->pBlock) - block->offset)) {
				//this block we can not use. go on and search.
				temp = block;
				block = block->next;
				continue;
			}
			else {
				//yes, we can use this one.
				uintptr_t  pPointerOfUserReturn = pPos;
				DescriptionBlock * pTempBlock;
				intptr_t offset = pEnd - pPos;
				//change the block that was just divided
				ASSERT((block->iBlockSize - (sizeBetterNeed + offset)) >= 0,"internal system go wrong.");
				if ((block->iBlockSize - (sizeBetterNeed + offset)) <= 8) {
					//this means that this block need not to be divided
					temp->next = block->next;
					if (block->next)
						block->next->prev = temp;
					//put it in the OutStanding List;
					block->next = pInUseMemoryList->next;
					block->prev = pInUseMemoryList;
					if (pInUseMemoryList->next)
						pInUseMemoryList->next->prev = block;
					pInUseMemoryList->next = block;
					block->offset = pPos - (reinterpret_cast<uintptr_t>(block->pBlock) - block->offset);
					block->pBlock = reinterpret_cast<void *>(pPos);
					pTempBlock = block;
				}
				else {
					//this means that we need to "divide" this block
					DescriptionBlock * aNewBlock = pAvailableBlocksList->next;
					if (!aNewBlock) {
						//even if we have the space, but we do not have any more blocks. For now we just return null
						printf("No more available blocks!\n");
						return nullptr;
					}

					size_t temp = block->iBlockSize;
					block->iBlockSize -= (sizeBetterNeed + offset);
					aNewBlock->iBlockSize = sizeBetterNeed+offset;
					aNewBlock->pBlock = reinterpret_cast<void *>(pPos);
					pAvailableBlocksList->next = aNewBlock->next;
					if(aNewBlock->next)
						aNewBlock->next->prev = pAvailableBlocksList;
					//put it in the OutStanding List;
					aNewBlock->next = pInUseMemoryList->next;
					aNewBlock->prev = pInUseMemoryList;
					if (pInUseMemoryList->next)
						pInUseMemoryList->next->prev = aNewBlock;
					pInUseMemoryList->next = aNewBlock;
					pTempBlock = aNewBlock;
				}

				//now to add the address of Description block ahead.
				pPointerOfUserReturn -= sizeof(uintptr_t);
				uintptr_t * pTemp = reinterpret_cast<uintptr_t *>(pPointerOfUserReturn);
				*pTemp = reinterpret_cast<uintptr_t>(pTempBlock);
#if _GUARD_BAND_
				// the first four bytes to do guardbands
				pPointerOfUserReturn -= 4;
				unsigned char * _bNoMansLandFill = reinterpret_cast<unsigned char *>(pPointerOfUserReturn);
				for (int i = 0; i < 4; i++) {
					*(_bNoMansLandFill + i) = 0xFD;
				}

				pPointerOfUserReturn += pTempBlock->iBlockSize - pTempBlock->offset;
				pPointerOfUserReturn -= 4;

				_bNoMansLandFill = reinterpret_cast<unsigned char *>(pPointerOfUserReturn);
				for (int i = 0; i < 4; i++) {
					*(_bNoMansLandFill + i) = 0xFD;
				}
#endif
				return pTempBlock->pBlock;
			}
		}
		else {
			//this block we can not use. go on and search.
			temp = block;
			block = block->next;
			continue;
		}
	}
}
bool Allocator::_free(void * ptr) {
	// when an user attemps to free a chunk of memory, check four bytes ahead
	uintptr_t ppp = (reinterpret_cast<uintptr_t>(ptr) - sizeof(uintptr_t));
	uintptr_t * pInt = reinterpret_cast<uintptr_t *>(ppp);
	DescriptionBlock * pBlock = reinterpret_cast<DescriptionBlock *>((*pInt));
	DescriptionBlock * pTemp = pFreeMemoryList->next;
	
	DescriptionBlock * blockAhead = pBlock->prev;
	blockAhead->next = pBlock->next;
	if (pBlock->next) {
		pBlock->next->prev = blockAhead;
	}

	pBlock->next = pTemp;
	if(pTemp)
		pTemp->prev = pBlock;

	pBlock->prev = pFreeMemoryList;
	pFreeMemoryList->next = pBlock;
	
	return true;
}
void Allocator::destroy() {
	//are there still outstanding memories?
	DescriptionBlock * p = pInUseMemoryList->next;
	if (p != nullptr) {
		Log("There are/is still some memories not freeed!!\n");
		Log("You can not destory heap now.\n");
		getchar();
		return;
	}
	pAvailableBlocksList = pFreeMemoryList = pInUseMemoryList = nullptr;
	pTotalMemory = pPoolMemory = nullptr;
}

void Allocator::visualizeAllBlockLists(int option) const{
	DescriptionBlock * head;
	switch (option)
	{
		case(0):
			head = pFreeMemoryList;
			break;
		case(1):
			head = pInUseMemoryList;
			break;
		case(2):
			head = pAvailableBlocksList;
			break;
		default:
			break;
	}
	DescriptionBlock * p = head;
	printf_s("***********************************************\n");
	while ((p = p->next) != nullptr) {
#ifdef _WIN64
		printf_s("   user address: 0x%.8llx", reinterpret_cast<uintptr_t>(p->pBlock));
		printf_s("   block size:%zd bytes\n", p->iBlockSize);
#else
		printf_s("   user address: 0x%.8x", reinterpret_cast<uintptr_t>(p->pBlock));
		printf_s("   block size:%d bytes\n", p->iBlockSize);
#endif
	}
	printf_s("***********************************************\n");
}
int Allocator::getBlockArrayLength(int option) const {
	DescriptionBlock * head;
	int size = 0;
	switch (option)
	{
	case(0):
		head = pFreeMemoryList;
		break;
	case(1):
		head = pInUseMemoryList;
		break;
	case(2):
		head = pAvailableBlocksList;
		break;
	default:
		break;
	}
	DescriptionBlock * p = head;
	while ((p = p->next) != nullptr) {
		size++;
	}
	return size;
}

DescriptionBlock * Allocator::combineBlocksTogether(DescriptionBlock * block1, DescriptionBlock * block2) {
	block1->iBlockSize += block2->iBlockSize;
	//delete block2 from free list
	DescriptionBlock * pPrev = block2->prev;
	DescriptionBlock * pNext = block2->next;
	pPrev->next = pNext;
	if(pNext)
		pNext->prev = pPrev;
	//add block2 to Available list
	block2->init();
	block2->next = pAvailableBlocksList->next;
	block2->prev = pAvailableBlocksList;

	pAvailableBlocksList->next = block2;
	if (block2->next) {
		block2->next->prev = block2;
	}
	return block1;
}

bool Allocator::isTwoBlocksAbut(DescriptionBlock * pLeftBlock, DescriptionBlock * pRightBlock) const  {
	uintptr_t address_P1 = reinterpret_cast<uintptr_t>(pLeftBlock->pBlock);
	uintptr_t address_P2 = reinterpret_cast<uintptr_t>(pRightBlock->pBlock);
	ASSERT(address_P1 < address_P2,"internal system worong.");
	return (address_P1 + pLeftBlock->iBlockSize== address_P2 - pRightBlock->offset);	
}

void Allocator::garbageCollection() {
	DescriptionBlock * p1 = pFreeMemoryList;
	quickSort(pFreeMemoryList, 0, getBlockArrayLength(0)-1);
	//Log("Sort free list:\n");
	//visualizeAllBlockLists(0);
	DescriptionBlock * p2 = pFreeMemoryList->next;
	while(p2){
		p1 = p2;
		p2 = p2->next;
		if (!p2) {
			continue;
		}
		uintptr_t addressP1 = reinterpret_cast<uintptr_t>(p1->pBlock);
		uintptr_t addressP2 = reinterpret_cast<uintptr_t>(p2->pBlock);
		if (p1 && p2 && isTwoBlocksAbut(p1, p2)) {
			DescriptionBlock * pBoxRight;
			DescriptionBlock * pBoxLeft;
			if (addressP1 < addressP2) {
				pBoxLeft = p1;
				pBoxRight = p2;
			}
			else {
				pBoxLeft = p2;
				pBoxRight = p1;
			}
			p2 = combineBlocksTogether(pBoxLeft, pBoxRight);
			isFreeMemoryCollected = true;
		}
	}
}

bool Allocator::isContained(void * pPtr) const  {
	ASSERT(pPtr != nullptr,"ptr could not be null!");
	uintptr_t pPtr_pos = reinterpret_cast<uintptr_t>(pPtr);
	if (pPtr_pos >= reinterpret_cast<uintptr_t>(pPoolMemory) && pPtr_pos < reinterpret_cast<uintptr_t>(pPoolMemory) + pFreeMemoryList->iBlockSize) {
		return true;
	}
	else
		return false;
}

bool Allocator::isAllocated(void * pPtr) const  {
	ASSERT(pPtr != nullptr, "ptr could not be null!");
	DescriptionBlock * pTemp = pInUseMemoryList;
	do {
		pTemp = pTemp->next;
		if (!pTemp) {
			return false;
		}
		if (reinterpret_cast<uintptr_t>(pTemp->pBlock) == reinterpret_cast<uintptr_t>(pPtr)) {
			return true;
		}
	} while (1);
	return false;
}