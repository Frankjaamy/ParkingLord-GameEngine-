#pragma once
#include<stdint.h>
#include <cstring>

#ifdef _WIN64
#define ALLSET 0xFFFFFFFFFFFFFFFF
#define EMPTY  0x0000000000000000
typedef  uint64_t bit_array_type;
const size_t UnitSize = 64;
#else
#define ALLSET 0xFFFFFFFF
#define EMPTY  0x00000000
typedef  uint32_t bit_array_type;
const size_t UnitSize = 32;
#endif // _WIN64

class BitArray {
public:
	static BitArray * create(size_t bitsWanted);
	~BitArray();

	void setBit(const size_t indexOfArray);
	void clearBit(const size_t indexOfArray);
	
	bool isBitSet(const size_t indexOfArray);
	bool isBitClear(const size_t indexOfArray);

	bool getFirstSetBit(size_t & o_bitPosition) const;
	bool getFirstClearBit(size_t & o_bitPosition) const;

	inline void clearAll() {memset(bitArray, 0x00, (totalSize / BitArrayUnitSize) * sizeof(bit_array_type));}
	inline void setAll() { memset(bitArray, 0xFF, (totalSize / BitArrayUnitSize) * sizeof(bit_array_type));}

	bool areAllClear() const;
	bool areAllSet()const;

	BitArray(size_t totalBitsWanted, void * memory, bool setBitArray);

	inline const size_t getCurrentBitUnitSize() const { return BitArrayUnitSize; }
	inline const size_t getTotalSize() const { return totalSize; }
	const static size_t BitArrayUnitSize = UnitSize;
private:
	bit_array_type * bitArray;
	size_t  totalSize;

};