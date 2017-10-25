#include "Utilities.h"
#include "Utilities\MyOwnAssert.h"

namespace AllocatorTools {

	void swapBlock(DescriptionBlock * p1, DescriptionBlock * p2) {
		if (p1 == p2) {
			return;
		}
		void * p = p2->pBlock;
		size_t size = p2->iBlockSize;
		intptr_t offset = p2->offset;

		p2->iBlockSize = p1->iBlockSize;
		p2->pBlock = p1->pBlock;
		p2->offset = p1->offset;

		p1->iBlockSize = size;
		p1->pBlock = p;
		p1->offset = offset;
	}

	DescriptionBlock * getDescriptionBlock(const DescriptionBlock * blockArray, int index) {
		int i = 0;
		DescriptionBlock * pBlock = nullptr;
		DescriptionBlock * p = const_cast<DescriptionBlock *>(blockArray);
		while (i <= index) {
			p = p->next;
			pBlock = p;
			i++;
		}
		ASSERT(pBlock, "this could not be null!");
		return pBlock;
	}

	void quickSort(DescriptionBlock * arrayBlocks, int p, int r) {
		if (p < r) {
			int q = partition(arrayBlocks, p, r);
			quickSort(arrayBlocks, p, q - 1);
			quickSort(arrayBlocks, q + 1, r);
		}
	}

	int partition(DescriptionBlock * arrayBlocks, int p, int r) {
		intptr_t x = reinterpret_cast<intptr_t>(getDescriptionBlock(arrayBlocks, r)->pBlock);

		int i = p - 1;
		for (int j = p; j < r;j++) {
			intptr_t y = reinterpret_cast<intptr_t>(getDescriptionBlock(arrayBlocks, j)->pBlock);
			if (y <= x) {
				i++;
				swapBlock(getDescriptionBlock(arrayBlocks, i), getDescriptionBlock(arrayBlocks, j));
			}
		}
		i++;
		swapBlock(getDescriptionBlock(arrayBlocks, i), getDescriptionBlock(arrayBlocks, r));
		return i;
	}

}