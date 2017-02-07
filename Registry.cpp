#include "ClassDef.hpp"
#include <ktmw32.h>
#include <string>

extern ULONG global_instances;
extern std::wstring * module_path;

extern "C" void guid_to_string(const GUID & class_id, wchar_t * str)
{
	static const wchar_t padding_hex[9] = L"00000000";
	wchar_t hex[16];
	size_t hex_len;

	memset(str, 0, sizeof(wchar_t) * 39);
	wcscat_s(str, 39, L"{");

	_ui64tow_s(class_id.Data1, hex, 16, 16);
	_wcsrev(hex);
	hex_len = wcslen(hex);
	if(hex_len < 8)
	{
		wcsncat_s(hex, 16, padding_hex, 8 - hex_len);
	}
	_wcsrev(hex);
	wcscat_s(str, 39, hex);
	wcscat_s(str, 39, L"-");

	_ui64tow_s(class_id.Data2, hex, 16, 16);
	_wcsrev(hex);
	hex_len = wcslen(hex);
	if(hex_len < 4)
	{
		wcsncat_s(hex, 16, padding_hex, 4 - hex_len);
	}
	_wcsrev(hex);
	wcscat_s(str, 39, hex);
	wcscat_s(str, 39, L"-");

	_ui64tow_s(class_id.Data3, hex, 16, 16);
	_wcsrev(hex);
	hex_len = wcslen(hex);
	if(hex_len < 4)
	{
		wcsncat_s(hex, 16, padding_hex, 4 - hex_len);
	}
	_wcsrev(hex);
	wcscat_s(str, 39, hex);
	wcscat_s(str, 39, L"-");

	for(int i = 0; i < 8; i++)
	{
		_ui64tow_s(class_id.Data4[i], hex, 16, 16);
		_wcsrev(hex);
		hex_len = wcslen(hex);
		if(hex_len < 2)
		{
			wcsncat_s(hex, 16, padding_hex, 2 - hex_len);
		}
		_wcsrev(hex);
		wcscat_s(str, 39, hex);
		if(i == 1)
		{
			wcscat_s(str, 39, L"-");
		}
	}

	wcscat_s(str, 39, L"}");
	_wcsupr_s(str, 39);
}

extern "C" bool delete_class_registry(HKEY clsid, const GUID & class_id)
{
	wchar_t class_id_string[39];
	guid_to_string(class_id, class_id_string);
	auto r = RegDeleteTreeW(clsid, class_id_string);
	return (r == ERROR_SUCCESS);
}

extern "C" bool create_class_registry(HKEY clsid, const GUID & class_id)
{
	wchar_t class_id_string[54];
	guid_to_string(class_id, class_id_string);
	wcscat_s(class_id_string, 54, L"\\InProcServer32");

	// CClassFactoryを登録
	HKEY subkey;
	auto result = RegCreateKeyW(clsid, class_id_string, &subkey);
	if(result != ERROR_SUCCESS)
	{
		return false;
	}

	RegSetValueExW(subkey, nullptr, 0UL, REG_SZ, (const BYTE *)module_path->c_str(), (DWORD)(sizeof(wchar_t) * module_path->length()));
	RegSetValueExW(subkey, L"ThreadingModel", 0UL, REG_SZ, (const BYTE *)L"Apartment", (DWORD)(sizeof(wchar_t) * 9));

	RegCloseKey(subkey);

	return true;
}

extern "C" HRESULT __stdcall DllRegisterServer()
{
	auto transaction = CreateTransaction(nullptr, nullptr, 0UL, 0UL, 0UL, 0UL, L"SuicaCP▶DllUnregisterServer");
	if(transaction == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	LRESULT result;
	HKEY key;
	result = RegOpenKeyTransactedW(HKEY_CLASSES_ROOT, L"CLSID", 0UL, KEY_CREATE_SUB_KEY | KEY_SET_VALUE, &key, transaction, nullptr);
	if(result != ERROR_SUCCESS)
	{
		CloseHandle(transaction);
		return E_FAIL;
	}

	// CClassFactoryを登録
	if(!create_class_registry(key, __uuidof(CClassFactory)))
	{
		RegCloseKey(key);
		CloseHandle(transaction);
		return E_FAIL;
	}

	RegCloseKey(key);

	result = RegOpenKeyTransactedW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers", 0UL, KEY_CREATE_SUB_KEY | KEY_SET_VALUE, &key, transaction, nullptr);
	if(result != ERROR_SUCCESS)
	{
		CloseHandle(transaction);
		return E_FAIL;
	}

	HKEY subkey;
	wchar_t class_factory_id[39];
	guid_to_string(__uuidof(CClassFactory), class_factory_id);
	result = RegCreateKeyW(key, class_factory_id, &subkey);
	if(result != ERROR_SUCCESS)
	{
		CloseHandle(key);
		CloseHandle(transaction);
		return E_FAIL;
	}
	RegSetValueExW(subkey, nullptr, 0UL, REG_SZ, (const BYTE*)L"Suica® 資格情報プロバイダー", (DWORD)(sizeof(wchar_t) * 16));

	RegCloseKey(key);

	CommitTransaction(transaction);
	CloseHandle(transaction);
	return S_OK;
}

extern "C" HRESULT __stdcall DllUnregisterServer()
{
	auto transaction = CreateTransaction(nullptr, nullptr, 0UL, 0UL, 0UL, 0UL, L"SuicaCP▶DllRegisterServer");
	if(transaction == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	LRESULT result;
	HKEY key;
	result = RegOpenKeyTransactedW(HKEY_CLASSES_ROOT, L"CLSID", 0UL, DELETE | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &key, transaction, nullptr);
	if(result != ERROR_SUCCESS)
	{
		CloseHandle(transaction);
		return E_FAIL;
	}

	// CClassFactoryを削除
	if(!delete_class_registry(key, __uuidof(CClassFactory)))
	{
		RegCloseKey(key);
		CloseHandle(transaction);
		return E_FAIL;
	}
	RegCloseKey(key);

	result = RegOpenKeyTransactedW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers", 0UL, DELETE | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &key, transaction, nullptr);
	if(result != ERROR_SUCCESS)
	{
		CloseHandle(transaction);
		return E_FAIL;
	}
	wchar_t class_factory_id[39];
	guid_to_string(__uuidof(CClassFactory), class_factory_id);
	RegDeleteTreeW(key, class_factory_id);
	RegCloseKey(key);

	CommitTransaction(transaction);
	CloseHandle(transaction);
	return S_OK;
}
