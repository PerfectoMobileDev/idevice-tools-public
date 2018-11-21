// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

#ifndef __cplusplus

typedef unsigned char bool;

#define true 1
#define false 0

#ifndef CASSERT
#define CASSERT(exp, name) typedef int dummy##name [(exp) ? 1 : -1];
#endif

CASSERT(sizeof(bool) == 1, bool_is_one_byte)
CASSERT(true, true_is_true)
CASSERT(!false, false_is_false)

#endif

// TODO: reference additional headers your program requires here
