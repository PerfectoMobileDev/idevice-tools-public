#pragma once
#include <atlstr.h>
class CGMiniDump
{
public:
	CGMiniDump(void);
	virtual ~CGMiniDump(void);

	void Dump(HINSTANCE hModuleInstance, PEXCEPTION_POINTERS excpInfo);
	static void SetPrefix(LPCTSTR);
protected:
	void Dump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo);
	void GetModuleFolder(HINSTANCE hModuleInstance, CString &sFolder);
};
