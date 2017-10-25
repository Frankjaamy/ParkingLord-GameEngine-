#pragma once
#include "Allocator.h"
namespace AllocatorTools {

	DescriptionBlock * getDescriptionBlock(const DescriptionBlock * blockArray, int index);

	void quickSort(DescriptionBlock * arrayBlocks, int p, int r);

	int partition(DescriptionBlock * arrayBlocks, int p, int r);

	void swapBlock(DescriptionBlock * p1, DescriptionBlock * p2);

}
