#include "stdafx.h"
#include "windows.h"
#include <atlstr.h>
#include "GMiniDump.h"
#include "Dbghelp.h"
//#include "GFileUtils.h"
#include <time.h>

CString __dumpPrefix = L"PerfectoDumpFile";

CGMiniDump::CGMiniDump(void)
{
}

CGMiniDump::~CGMiniDump(void)
{
}

void CGMiniDump::SetPrefix(LPCTSTR szPrefix)
{
	__dumpPrefix = szPrefix;
}

///////////////////////////////////////////////////////////////////////////////
//	GetModuleFolder
///////////////////////////////////////////////////////////////////////////////
void CGMiniDump::GetModuleFolder(HINSTANCE hModuleInstance, CString &sFolder)
{
	//LPTSTR  strDLLPath1 = new TCHAR[_MAX_PATH];
	TCHAR strDLLPath1[_MAX_PATH + 1] = { 0 };
	::GetModuleFileName(hModuleInstance, strDLLPath1, _MAX_PATH);
	sFolder = strDLLPath1;

	int nPos = sFolder.ReverseFind(_T('\\'));
	if (nPos > -1)
		sFolder = sFolder.Left(nPos + 1);
	else
		sFolder.Empty();
}


void CGMiniDump::Dump(HINSTANCE hModuleInstance, PEXCEPTION_POINTERS pExcptionPointer)
{
	CString sFileName;
	GetModuleFolder(hModuleInstance, sFileName);
	time_t ltime;
	time(&ltime);
	CString sTime;
	sTime.Format(L"_%ld", ltime);
	sFileName += __dumpPrefix;
	sFileName += sTime;
	sFileName += L".dmp";

	HANDLE hMiniDumpFile = CreateFile(sFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
	if (hMiniDumpFile != INVALID_HANDLE_VALUE)
	{
		Dump(hMiniDumpFile, pExcptionPointer);
		CloseHandle(hMiniDumpFile);
	}
}

void CGMiniDump::Dump(HANDLE hFile, PEXCEPTION_POINTERS pExcptionPointer)
{
    if (pExcptionPointer == NULL) 
    {
		return; 
		
		// Generate exception to get proper context in dump
		/*__try 
		{
			OutputDebugString(_T("raising exception\r\n"));
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
				 EXCEPTION_CONTINUE_EXECUTION) 
		{
		} */
    } 
    else
    {
        MINIDUMP_EXCEPTION_INFORMATION eInfo;
        eInfo.ThreadId = GetCurrentThreadId();
        eInfo.ExceptionPointers = pExcptionPointer;
        eInfo.ClientPointers = FALSE;

        // note:  MiniDumpWithIndirectlyReferencedMemory does not work on Win98
        MiniDumpWriteDump(GetCurrentProcess(),
						  GetCurrentProcessId(),
						  hFile,
						  MiniDumpWithDataSegs, //MiniDumpWithDataSegs (instead of MiniDumpNormal), which is the equivalent to the Minidump With Heap option in the Visual Studio debugger. This does result in substantially larger dump files
						  pExcptionPointer ? &eInfo : NULL,
						  NULL,
						  NULL);
    }
}
