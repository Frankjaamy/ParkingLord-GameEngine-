#pragma once
#include "MyOwnAssert.h"
struct DescriptionBlock;
namespace CYNTools
{
	bool convertCharToInt(const char number[], int & output_value);
	void clearInput();

	bool isNan(float i_value);
	bool compareTwoFloats(const float lValue, const float rValue);

	void quickSort(int * Array, int p, int r);
	int partition(int * Array, int p, int r);
	void swapInt(int &a, int &b);

}