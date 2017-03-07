#include <Windows.h>
#include <string>
#include "../Common/Util.hpp"
#include "ClassFactory.hpp"

using std::wstring;

void * dll;
ULONG global_instances;
wstring * module_path;

wstring * database;
wstring * confidentiality;

extern "C" bool dll_process_attach()
{
	DisableThreadLibraryCalls(static_cast<HMODULE>(dll));
	DebugPrint(L"Start DLL_PROCESS_ATTACH");
	global_instances = 0;

	database = new wstring;
	confidentiality = new wstring;

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
			return false;
		}
	}

	module_path = new wstring(_module_path);
	delete[] _module_path;

	DebugPrint(L"Module path:%s", module_path->c_str());

	DebugPrint(L"End DLL_PROCESS_ATTACH");
	return true;
}

extern "C" void dll_process_detach()
{
	DebugPrint(L"Start DLL_PROCESS_DETACH");
	delete module_path;
	delete confidentiality;
	delete database;
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

extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void ** ppv)
{
	HRESULT result;

	// rclsidはファクトリーのUUIDである必要がある
	if(rclsid != __uuidof(CClassFactory))
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	IClassFactory * f = new CClassFactory();
	// riidで指定したインターフェイスに対応するクラスのインスタンスを生成
	result = f->CreateInstance(nullptr, riid, ppv);
	// 工場爆破
	f->Release();

	return result;
}

extern "C" HRESULT __stdcall DllCanUnloadNow()
{
	return (global_instances == 0UL) ? S_OK : S_FALSE;
}
