#include <Windows.h>
#include <string>
#include "../Common/Util.hpp"

using std::wstring;

void * dll;
wstring * module_path;

extern wstring * database;
extern wstring * confidentiality;

namespace Lsa
{
	HANDLE Heap;
}

extern "C" bool dll_process_attach()
{
	DisableThreadLibraryCalls(static_cast<HMODULE>(dll));
	DebugPrint(L"Start DLL_PROCESS_ATTACH");

	Lsa::Heap = HeapCreate(0, 65536, 0);
	if(!Lsa::Heap)
	{
		DebugPrint(L"HeapCreate failed.");
		return false;
	}

	// DllRegisterServerで使用するフルパスを取得
	// Windows 10以降ではMAX_PATHを超えることが可能になったのでその対策
	DWORD _module_path_chars = 256;
	wchar_t * _module_path;
	for(;;)
	{
		_module_path = new wchar_t[_module_path_chars];
		SetLastError(0);
		GetModuleFileNameW(static_cast<HMODULE>(dll), _module_path, _module_path_chars);
		if(GetLastError() == ERROR_SUCCESS)
		{
			break;
		}
		delete[] _module_path;
		_module_path_chars += 256;
		if(_module_path_chars >= 65536)
		{
			DebugPrint(L"Can't get module path.");
			HeapDestroy(Lsa::Heap);
			return false;
		}
	}

	module_path = new wstring(_module_path);
	delete[] _module_path;

	DebugPrint(L"Module path:%s", module_path->c_str());

	database = new wstring;
	confidentiality = new wstring;

	DebugPrint(L"End DLL_PROCESS_ATTACH");
	return true;
}

extern "C" void dll_process_detach()
{
	DebugPrint(L"Start DLL_PROCESS_DETACH");
	delete confidentiality;
	delete database;
	delete module_path;
	HeapDestroy(Lsa::Heap);
	DebugPrint(L"End DLL_PROCESS_DETACH");
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_DETACH:
		dll_process_detach();
		break;
	case DLL_PROCESS_ATTACH:
		dll = hinstDLL;
		return dll_process_attach() ? TRUE : FALSE;
	}

	return TRUE;
}
