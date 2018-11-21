#include "StdAfx.h"
#include "windows.h"
#include <atlstr.h>
#include "GExceptionUtils.h"
#include "GMiniDump.h"

void SetDumpPrefix(LPCTSTR szPrefix)
{
	CGMiniDump::SetPrefix(szPrefix);
}

int FilterExcept(unsigned int code, struct _EXCEPTION_POINTERS *pExcptionPointer)
{
	CGMiniDump oGMiniDump;

	switch (code)
	{
		case EXCEPTION_ACCESS_VIOLATION:
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
		case EXCEPTION_FLT_STACK_CHECK:
		case EXCEPTION_STACK_OVERFLOW:
			{
				HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
				oGMiniDump.Dump(hInstance, pExcptionPointer);
			}
		  break;

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		case EXCEPTION_DATATYPE_MISALIGNMENT:
		case EXCEPTION_FLT_DENORMAL_OPERAND:
		case EXCEPTION_FLT_INEXACT_RESULT:
		case EXCEPTION_FLT_INVALID_OPERATION:
		case EXCEPTION_FLT_OVERFLOW:
		case EXCEPTION_FLT_UNDERFLOW:
		case EXCEPTION_INT_OVERFLOW:
		case EXCEPTION_PRIV_INSTRUCTION:
		case EXCEPTION_IN_PAGE_ERROR:
		case EXCEPTION_ILLEGAL_INSTRUCTION:
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		case EXCEPTION_INVALID_DISPOSITION:     
		case EXCEPTION_GUARD_PAGE:
		case EXCEPTION_INVALID_HANDLE:
  		default:
			break;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
