﻿#pragma once
#include <Windows.h>
#include <string>

using std::wstring;

extern ULONG global_instances;

enum StandardIO { Input, Output, Error };

extern "C" void close_stdio(StandardIO);
extern "C" void open_stdio(StandardIO);

extern "C" void * allocator(size_t);
extern "C" void releaser(void *);

extern "C" int MessageBoxFmt(HWND, const wchar_t *, UINT, const wchar_t *, ...);
extern "C" void guid_to_string(const GUID &, wchar_t *);

void BinaryToString(const void *, size_t, wstring &);

#if defined(_DEBUG)
extern "C" void DebugPrint(const wchar_t *, ...);
#else
#define DebugPrint(msg, ...)
#endif
