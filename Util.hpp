#pragma once
#include <Windows.h>

extern ULONG global_instances;

extern "C" int MessageBoxFmt(HWND, const wchar_t *, UINT, const wchar_t *, ...);

#if defined(_DEBUG)
extern "C" void DebugPrint(const wchar_t *, ...);
#else
#define DebugPrint(msg, ...)
#endif
