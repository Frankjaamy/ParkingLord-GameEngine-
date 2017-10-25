#pragma once

#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Math.h>

#include "UtilFunctions.h"

namespace CYNTools
{
	bool compareTwoFloats(const float lValue, const float rValue) {
		if (lValue == rValue)
			return true;

		const float epsilon = .00001f;
		return  fabs(lValue - rValue) < epsilon ? true : false;
	}

	bool isNan(float i_value) {
		volatile float fValue = i_value;
		return fValue != fValue;
	}

	bool convertCharToInt(const char number[], int & output_value) {
		for (unsigned int i = 0; i < strlen(number); i++) {
			if ((short)number[i] < 48 || (short)number[i]>57) {
				printf("Invalid input. Please try again.\n");
				return false;
			}
			else {
				output_value = atoi(number);
				if (output_value == 0) {
					printf("...Zero? Be a man.\n");
					return false;
				}
				return true;
			}
		}
		return false;
	}

	void clearInput() {
		int c;
		while ((c = getchar()) != '\n' && c != EOF) {
			;
		}
	}

	void quickSort(int * Array, int p, int r) {
		if (p < r) {
			int q = partition(Array, p, r);
			quickSort(Array, p, q - 1);
			quickSort(Array, q+1,r);
		}
	}
	void swapInt(int &a, int & b) {
		if (a != b) {
			a ^= b;
			b ^= a;
			a ^= b;
		}
	}
	int partition(int * Array, int p, int r) {
		int x = Array[r];
		int i = p - 1;
		for (int j = p; j < r;j++) {
			if (Array[j] <= x) {
				i++;
				swapInt(Array[i], Array[j]);
			}
		}
		i++;
		swapInt(Array[i], Array[r]);
		return i;
	}
}