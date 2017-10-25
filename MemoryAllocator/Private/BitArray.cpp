/*
Hi, this is a piece of code sample. 
It is part of my own Memory Manager, a class that has Memory Normal/Fast Allocation, 
Garbage Collection and other stuff.

This .cpp file is the implementation of class BitArray. Basically what it does is to
process a value on bit level to quickly set one bit, or find which bit is set.

I choose to send this as sample because I think it shows my knowledge in data, data structure,
math, C++ and thoughts on code optimization. And also it is not very long to read :-);

You could also find my other stuff in my personal website:

Thank you! http://chenynfrank.wixsite.com/cynfrank
*/


#include <intrin.h>
#include <new>
#include "BitArray.h"
#include "Utilities\MyOwnAssert.h"

#pragma intrinsic(_BitScanForward)
// a macro to reduce code amount
#define SET_VALUE(bytePosition, bitPosition, byteLocated) \
	ASSERT(indexOfArray >= 0 && indexOfArray < totalSize,"invalid index!");\
	size_t bytePosition = indexOfArray / BitArrayUnitSize;\
	size_t bitPosition = indexOfArray % BitArrayUnitSize;\
	bit_array_type byteLocated = bitArray[bytePosition];\
//use _BitScanForwad to quickly find the first set bit.
#if _WIN64
	#define BIT_SEARCH(o_position, dataMask, o_isSuccess) \
			unsigned long long dataMask = bitArray[index];	\
			unsigned long o_position;	\
			unsigned char o_isSuccess;	\
			o_isSuccess = _BitScanForward64(&o_position, dataMask);	\

#else
	#define BIT_SEARCH(o_position, dataMask, o_isSuccess) \
			unsigned long dataMask = bitArray[index];	\
			unsigned long o_position;	\
			unsigned char o_isSuccess;	\
			o_isSuccess = _BitScanForward(&o_position, dataMask);	\

#endif // _WIN64

BitArray * BitArray::create(size_t bitsWanted){
	static BitArray * pBA = nullptr;
	if (!pBA) {
		pBA = new BitArray(bitsWanted, nullptr, false);
	}
	return pBA;
}
// use placement new to make sure this chunk of memory got placed in the right position.
BitArray::BitArray(size_t totalBitsWanted, void * memory, bool setBitArray) {

	totalSize = totalBitsWanted;
	if(memory == nullptr)
		bitArray = new bit_array_type[totalBitsWanted / BitArrayUnitSize];
	else
		bitArray = new (memory) bit_array_type[totalBitsWanted / BitArrayUnitSize];

	ASSERT(bitArray,"initialize failed!");
	memset(bitArray, setBitArray ? 0xFF : 0x00, (totalBitsWanted / BitArrayUnitSize) * sizeof(bit_array_type));
}

BitArray::~BitArray() {
	
}

void BitArray::setBit(const size_t indexOfArray) {

	SET_VALUE(bytePosition, bitPosition, byteLocated)

	bitArray[bytePosition] = byteLocated | (0x1i64 << (bitPosition));
}

void BitArray::clearBit(const size_t indexOfArray) {

	SET_VALUE(bytePosition, bitPosition, byteLocated)

	bit_array_type bitMask = ~(0x01i64 << (bitPosition));

	bitArray[bytePosition] = byteLocated & bitMask;
}

bool BitArray::isBitSet(const size_t indexOfArray) {
	SET_VALUE(bytePosition, bitPosition, byteLocated)

	bit_array_type bitMask = (0x01i64 << (bitPosition));
	return (bitArray[bytePosition] & bitMask) > 0;
}

bool BitArray::isBitClear(const size_t indexOfArray)
{
	return !isBitSet(indexOfArray);
}

bool BitArray::getFirstSetBit(size_t & o_bitPosition) const{
	size_t index = 0;
	while ((bitArray[index] == EMPTY) && (index < totalSize / BitArrayUnitSize)) {
		index++;
		if (index == totalSize / BitArrayUnitSize) {
			o_bitPosition = -1;
			return false;
		}
	}
	BIT_SEARCH(o_position, dataMask, o_isSuccess)

	if (o_isSuccess) {
		o_bitPosition = index * BitArrayUnitSize + o_position;
		return true;
	}
	else {
		o_bitPosition = -1;
		return false;
	}
}

bool BitArray::getFirstClearBit(size_t & o_bitPosition) const{
	/*
		size_t index = 0;
		while ((bitArray[index] == ALLSET) && (index < totalSize / BitArrayUnitSize)) {
			index++;
		}
		if (index >= totalSize / BitArrayUnitSize) {
			o_bitPosition = -1;
			return false;
		}
		else {
			size_t bit;
			for (bit = 0; bit < BitArrayUnitSize; bit++) {
				if (!(bitArray[index] & (0x01i64 << bit))) {
					break;
				}
			}
			o_bitPosition = index * BitArrayUnitSize + bit;
			return true;
		}	
	*/
	//use a quicker way to find the first Clear Bit
	size_t index = 0;
	while ((bitArray[index] == ALLSET) && (index < totalSize / BitArrayUnitSize)) {
		index++;
	}
	if (index >= totalSize / BitArrayUnitSize) {
		o_bitPosition = -1;
		return false;
	}

	bit_array_type value = bitArray[index];

#ifdef _WIN64
	unsigned int i32 = ((value & 0xffffffff) == 0xffffffff) << 5;
	value >>= i32;
#endif // _WIN64
	unsigned int i16 = ((value & 0xffff) == 0xffff) << 4;
	value >>= i16;

	unsigned int i8 = ((value & 0xff) == 0xff) << 3;
	value >>= i8;

	unsigned int i4 = ((value & 0xf) == 0xf) << 2;
	value >>= i4;

	unsigned int i2 = ((value & 0x3) == 0x3) << 1;
	value >>= i2;

	unsigned int i1 = ((value & 0x1) == 0x1);

	if (((value >> i1) & 1) == 1) return false;
	o_bitPosition = i16 + i8 + i4 + i2 + i1;
#ifdef _WIN64
	o_bitPosition += i32;
#endif // _WIN64

	return true;
}

bool BitArray::areAllClear() const {
	size_t index = 0;
	while (index < totalSize / BitArrayUnitSize) {
		if (bitArray[index] == EMPTY) {
			index++;
		}
		else {
			return false;
		}	
	}
	return true;
}

bool BitArray::areAllSet() const {
	size_t index = 0;
	while (index < totalSize / BitArrayUnitSize) {
		if (bitArray[index] == ALLSET) {
			index++;
		}
		else {
			return false;
		}
	}
	return true;
}