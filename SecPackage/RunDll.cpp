#if defined(WINAPI_FAMILY)
#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif
#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <Windows.h>
#include <winscard.h>
#include <string>
#include <vector>
#include <memory>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <wincred.h>
#include <Ole2.h>
#include "Lsa.hpp"
#include "SecIdentity.hpp"
#include "../Common/StringClass.hpp"
#include "../Common/Util.hpp"

using std::vector;
using std::unique_ptr;
extern wstring * module_path;

extern "C" void CALLBACK TestA(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	MessageBoxW(hwnd, L"このメッセージはでないはずだよ。", nullptr, MB_ICONHAND);
}

#define PACKAGE_KEY L"Security Packages"

extern "C" void EnumAuthenticationPackages()
{
	HKEY lsa;
	LRESULT reg_result;

	reg_result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, LR"(SYSTEM\CurrentControlSet\Control\Lsa)", 0, KEY_QUERY_VALUE, &lsa);
	if(reg_result != ERROR_SUCCESS)
	{
		wprintf(L"LSAのレジストリキーを開けません 0x%08X\n", static_cast<unsigned int>(reg_result));
		return;
	}

	DWORD size = 0;
	DWORD type = REG_NONE;
	RegQueryValueExW(lsa, PACKAGE_KEY, nullptr, &type, nullptr, &size);
	if(!((type == REG_SZ) || (type == REG_EXPAND_SZ) || (type == REG_MULTI_SZ)))
	{
		wprintf(L"文字列型ではありません。");
		RegCloseKey(lsa);
		return;
	}

	auto reg_value = new wchar_t[size / sizeof(wchar_t)];
	RegQueryValueExW(lsa, PACKAGE_KEY, nullptr, nullptr, reinterpret_cast<BYTE*>(reg_value), &size);

	auto packages = new ustrings;
	packages->reserve(32);

	if(type == REG_SZ)
	{
		packages->push_back(reg_value);
	}
	else if(type == REG_EXPAND_SZ)
	{
		auto expand_size = ExpandEnvironmentStringsW(reg_value, nullptr, 0);
		auto expand_value = new wchar_t[expand_size];
		ExpandEnvironmentStringsW(reg_value, expand_value, expand_size);
		packages->push_back(expand_value);
		delete[] expand_value;
	}
	else if(type == REG_MULTI_SZ)
	{
		for(auto p = reg_value; *p; p += (1 + wcslen(p)))
		{
			packages->push_back(p);
		}
	}
	delete[] reg_value;
	RegCloseKey(lsa);

	wprintf_s(L"パッケージの数 %Iu\n", packages->size());
	auto c = 0u;
	LSA_HANDLE lsa_h;
	LSA_OPERATIONAL_MODE om;
	LSA_STRING * process;
	bool trusted_lsa;
	{
		auto file_name = wcsrchr(module_path->c_str(), L'\\');
		if(file_name && (wcslen(1 + file_name) > 0))
		{
			// ファイル名の部分を切り出し

			// c_str()で返るポインターはconstなので複製
			auto file_name_dup = _wcsdup(1 + file_name);
#pragma warning(push)
#pragma warning(disable:4996)
			_wcslwr(file_name_dup);
#pragma warning(pop)

			if(auto ext = wcsrchr(file_name_dup, L'.'))
			{
				// 拡張子を切り落とす
				*ext = L'\0';
			}

			auto file_name_lsa = ustring(file_name_dup);
			process = file_name_lsa.to_lsa_string();

			free(file_name_dup);
		}
		else
		{
			auto default_name_lsa = ustring(L"nfccp");
			process = default_name_lsa.to_lsa_string();
		}
	}
	if(LsaRegisterLogonProcess(process, &lsa_h, &om) != STATUS_SUCCESS)
	{
		trusted_lsa = false;

		if(LsaConnectUntrusted(&lsa_h) != STATUS_SUCCESS)
		{
			lsa_h = nullptr;
		}
	}
	else
	{
		wprintf_s(L"Trusted LSA mode. %u\n", om);
		trusted_lsa = true;
	}
	ustring::default_free(process);

	for(auto i = packages->begin(), e = packages->end(); i != e; i++)
	{
		ULONG pkgid;
		NTSTATUS lsa_result;
		if(lsa_h)
		{
			auto pkgname = (*i)->to_lsa_string();
			lsa_result = LsaLookupAuthenticationPackage(lsa_h, pkgname, &pkgid);
			if(lsa_result != STATUS_SUCCESS)
			{
				pkgid = 0xFFFFFFFEul;
			}
			(*releaser)(pkgname);
		}
		else
		{
			lsa_result = STATUS_NOT_SUPPORTED;
			pkgid = 0xFFFFFFFFul;
		}
		if(lsa_result < 0)
		{
			wprintf(L"%03u:%s(ERR:0x%08X)\n", c++, (*i)->c_str(), lsa_result);
		}
		else
		{
			wprintf(L"%03u:%s(%i)\n", c++, (*i)->c_str(), static_cast<int>(pkgid));
		}
	}
	if(lsa_h)
	{
		if(trusted_lsa)
		{
			LsaDeregisterLogonProcess(lsa_h);
		}
		else
		{
			LsaClose(lsa_h);
		}
	}

	//packages->clear();
	delete packages;
}

extern "C" void EnumAccounts()
{
	NtAccounts ac;
	vector<SecurityIdentity *> users;

	ac.GetUserAccounts(users);
	wstring name;
	for(auto i = users.begin(), e = users.end(); i != e; i++)
	{
		(*i)->GetName(name);
		wprintf(L"%s\n", name.c_str());
	}

	ac.GetAll(users);
	for(auto i = users.begin(), e = users.end(); i != e; i++)
	{
		if((*i)->IsGroup())
		{
			(*i)->GetName(name);
			wprintf(L"▶%s\n", name.c_str());
		}
	}
}

extern "C" void CALLBACK TestW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	open_stdio(StandardIO::Input);
	open_stdio(StandardIO::Output);
	open_stdio(StandardIO::Error);

	Lsa::AllocateLsaHeap = [](ULONG s) { return reinterpret_cast<void*>(new unsigned char[s]); };
	Lsa::FreeLsaHeap = [](void * b) { delete[] static_cast<unsigned char*>(b); };

	EnumAuthenticationPackages();
	EnumAccounts();

	_getwch();

	close_stdio(StandardIO::Error);
	close_stdio(StandardIO::Output);
	close_stdio(StandardIO::Input);
	FreeConsole();
}
