#pragma once
#define EMIT_LOCATION() printf("File:%s, Line:%d\n",__FILE__,__LINE__);
#define CONCAT_HELPER(left, right) left##right
#define CONCAT(left, right) CONCAT_HELPER(left, right)


#if defined(_MSC_VER)
	#include<stdio.h>
	#define DEBUG_BREAK()	__debugbreak();
#if defined(_DEBUG)
	#define Log(fmt,...)	printf(fmt,__VA_ARGS__);
	#define LogOutPutStandard(fmt,...)	char CONCAT(szBuff,__LINE__)[1024];\
										sprintf_s(CONCAT(szBuff,__LINE__),sizeof(CONCAT(szBuff,__LINE__)),fmt,__VA_ARGS__);\
										OutputDebugStringA(CONCAT(szBuff,__LINE__));
#else 
	#define Log(fmt,...)	   ;
	#define LogOutPutStandard(fmt,...) ;
#endif

#elif defined(__GNUC__)
	#define DEBUG_BREAK() __builtin_trap();
#else
	"Inplement the debug function here.";
#endif

#if defined(_DEBUG)
	#define PROFILING_ENABLED 1
#else
	#define PROFILING_ENABLED 0
#endif

#if defined(PROFILING_ENABLED)
	#define	PROFILE_UNSCOPED(name) \
		static Accumulator CONCAT(__Accumulator, __LINE__)(name); ScopedTimer CONCAT(__Timer, __LINE__)(CONCAT(__Accumulator, __LINE__));

	#define PROFILE_SCOPE_BEGIN(name)   {	\
		static Accumulator CONCAT(__Accumulator,__LINE__)(name); ScopedTimer CONCAT(__Timer,__LINE__)(CONCAT(__Accumulator,__LINE__));
	#define PROFILE_SCOPE_END()	   }
#else
	#define	PROFILE_UNSCOPED(name) ;
	#define PROFILE_SCOPE_BEGIN(name);
	#define PROFILE_SCOPE_END();
#endif



#if defined(_DEBUG)
	#include<assert.h>
	#define ASSERT(assertion,log,...) assert(assertion);
	#define ASSERTNL(assertion) assert(assertion);
#else
	#define ASSERT(assertion,log) void(0);
	#define ASSERTNL(assertion) void(0);
#endif



namespace CYNTools
{
	void BreakInDebugger(bool assertion, const char * additionalLog);
}