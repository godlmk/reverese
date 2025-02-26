#include "Tools.h"
#include <Windows.h>
#include <stdio.h>
#include <varargs.h>

void __cdecl OutputDebugStringF(const char* format, ...)
{
	va_list vlArgs;
	char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);

	if (strBuffer == NULL) {
		return; // Handle allocation failure
	}

	va_start(vlArgs, format);
	_vsnprintf_s(strBuffer, 4096, _TRUNCATE, format, vlArgs);
	va_end(vlArgs);
	strcat_s(strBuffer, 4096, "\n");
	OutputDebugStringA(strBuffer);
	GlobalFree(strBuffer);
	return;
}
