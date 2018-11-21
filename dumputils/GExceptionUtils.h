#pragma once

#include "Dbghelp.h"

void SetDumpPrefix(LPCTSTR);
int FilterExcept(unsigned int code, struct _EXCEPTION_POINTERS *pExcptionPointer);

#define CRASH_GUARD_BLOCK_START 	__try{
#define CRASH_GUARD_BLOCK_END 			}__except( FilterExcept(GetExceptionCode(), GetExceptionInformation()) ) {}


#define CRASH_GUARD_ENTRY_POINT_METHOD(RETURN_TYPE, funcCall)\
{\
	RETURN_TYPE hr = S_OK;\
	__try\
	{\
		hr = funcCall;\
	}\
	__except( FilterExcept(GetExceptionCode(), GetExceptionInformation()))\
	{\
		char aDummy = 'a';\
	}\
	return hr;\
}

#define CRASH_GUARD_ENTRY_POINT_METHOD_VOID(funcCall)\
{\
	__try\
	{\
		funcCall;\
	}\
	__except( FilterExcept(GetExceptionCode(), GetExceptionInformation()))\
	{\
		char aDummy = 'a';\
	}\
}
