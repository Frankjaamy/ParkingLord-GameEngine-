#include <new>
#include "Allocator.h"
#include "NewandDelete.h"
#include "FixedSizeAllocator.h"
#include "Utilities\MyOwnAssert.h"

FixedSizeAllocator::FixedSizeAllocator(Allocator * heap, void * pMemoryToStart, size_t fixedSizeWanted, size_t numOfBlocks):
totalSize(fixedSizeWanted * numOfBlocks)
{

	this->pHeap = heap;
	this->fixedSize = fixedSizeWanted;

	int8_t * pMemory = reinterpret_cast<int8_t *>(pMemoryToStart);
	
	pStartMemory = pMemory;
	pPoolMemory = pMemory + (numOfBlocks + 7) / 8;

	bool isSet = true;
	m_pAllocFlagArray = new(heap) BitArray(numOfBlocks,pMemory, isSet);
	empty = isSet ? false : true;
}

FixedSizeAllocator::~FixedSizeAllocator() {
	size_t firstClear;
	if (m_pAllocFlagArray->getFirstClearBit(firstClear)) {
		Log("There are still outstanding memory blocks!");
		getchar();
	}
	pHeap->_free(pStartMemory);
	pHeap->_free(m_pAllocFlagArray);
	//delete m_pAllocFlagArray;
}

void * FixedSizeAllocator::alloc() {
	size_t i_firstAvailable;

	if (m_pAllocFlagArray->getFirstSetBit(i_firstAvailable)) {
		m_pAllocFlagArray->clearBit(i_firstAvailable);
		return pPoolMemory + (i_firstAvailable * fixedSize);
	}
	else {
		empty = true;
		Log("FSA-%d is use up.\n", static_cast<int>(getFixedSize()));
		getchar();
		return nullptr;
	}
}

void FixedSizeAllocator::_free(void * ptr) {
	uintptr_t flagPosition;
	bool success = isAllocated(ptr, flagPosition);
	ASSERT(success,"Pointer is not marked as outstanding!");
	ASSERT(flagPosition >= 0 && flagPosition < m_pAllocFlagArray->getTotalSize(), "Invalid flagPosition!");
	empty = false;
	m_pAllocFlagArray->setBit(flagPosition);
}

bool FixedSizeAllocator::isContained(void * i_ptr) const {
	return ((reinterpret_cast<int8_t *>(i_ptr) >= pPoolMemory) && (reinterpret_cast<int8_t *>(i_ptr) <= (pPoolMemory + totalSize - fixedSize)));
}

bool FixedSizeAllocator::isAllocated(void * ptr, uintptr_t & o_flagPosition) const {
	int8_t * pTemp = reinterpret_cast<int8_t *>(ptr);
	ASSERT(pTemp >= pPoolMemory, "Wrong Pointer Address!");
	uintptr_t distance = pTemp - pPoolMemory;

	o_flagPosition = distance / fixedSize;
	return m_pAllocFlagArray->isBitClear(o_flagPosition);
}

void FixedSizeAllocator::destroySelf() {
	size_t firstClear;
	if (m_pAllocFlagArray->getFirstClearBit(firstClear)) {
		Log("There are still outstanding memory blocks!");
		getchar();
	}
	pHeap->_free(pStartMemory);
	pHeap->_free(m_pAllocFlagArray);
}
