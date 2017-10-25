#include"MyOwnAssert.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
namespace CYNTools
{
	void BreakInDebugger(bool assertion, const char * additionalLog) {
#if defined(_MSC_VER)
	// to do
#elif defined(__GNUC__)
	// to do
#endif
	}
}