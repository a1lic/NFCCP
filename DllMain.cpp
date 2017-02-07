#include "ClassDef.hpp"
#include <string>

ULONG global_instances;
std::wstring * module_path;

extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);
		global_instances = 0UL;

		// DllRegisterServerで使用するフルパスを取得
		// Windows 10以降ではMAX_PATHを超えることが可能になったのでその対策
		DWORD _module_path_chars = 256UL;
		wchar_t * _module_path;
		for(;;)
		{
			_module_path = new wchar_t[_module_path_chars];
			SetLastError(0UL);
			GetModuleFileNameW(hinstDLL, _module_path, _module_path_chars);
			if(GetLastError() == ERROR_SUCCESS)
			{
				break;
			}
			delete _module_path;
			_module_path_chars += 256UL;
		}

		module_path = new std::wstring(_module_path);
		delete _module_path;
	}
	else if(fdwReason == DLL_PROCESS_DETACH)
	{
		delete module_path;
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
