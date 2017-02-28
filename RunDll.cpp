#define WIN32_NO_STATUS
#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <winscard.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <wincred.h>
#include <Ole2.h>
#include "SmartCardHelper.hpp"
#include "Lsa.hpp"
#include "SecIdentity.hpp"

using std::vector;

#undef NTSTATUS_FROM_WIN32
extern "C" NTSTATUS NTSTATUS_FROM_WIN32(long x)
{
	return x <= 0 ? (NTSTATUS)x : (NTSTATUS)(((x) & 0x0000FFFF) | (FACILITY_NTWIN32 << 16) | ERROR_SEVERITY_ERROR);
}

enum StandardIO { Input, Output, Error };

extern "C" void close_stdio(StandardIO type)
{
	switch(type)
	{
	case StandardIO::Input:
		fclose(stdin);
		break;
	case StandardIO::Output:
		fclose(stdout);
		break;
	case StandardIO::Error:
		fclose(stderr);
		break;
	}
}

extern "C" void open_stdio(StandardIO type)
{
	FILE * f = nullptr;
	switch(type)
	{
	case StandardIO::Input:
		_wfreopen_s(&f, L"CONIN$", L"r+,ccs=UTF-16LE", stdin);
		//SetStdHandle(STD_INPUT_HANDLE, reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(stdin))));
		fflush(stdin);
		break;
	case StandardIO::Output:
		_wfreopen_s(&f, L"CONOUT$", L"w+,ccs=UTF-16LE", stdout);
		//SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(stdout))));
		//{
		//	int fh = _fileno(stdout);
		//	std::wstring check(L"CHECK\n標準出力チェック\n");
		//	for(auto i = check.begin(), e = check.end(); i < e; i++)
		//	{
		//		auto c = *i;
		//		_write(fh, &c, sizeof(c));
		//	}
		//}
		fflush(stdout);
		break;
	case StandardIO::Error:
		_wfreopen_s(&f, L"CONOUT$", L"w+,ccs=UTF-16LE", stderr);
		//SetStdHandle(STD_ERROR_HANDLE, reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(stderr))));
		//{
		//	std::wstring check(L"ERR CHECK\n標準エラーチェック\n");
		//	for(auto i = check.begin(), e = check.end(); i < e; i++)
		//	{
		//		_fputwc_nolock(*i, stderr);
		//	}
		//}
		fflush(stderr);
		break;
	}
}

extern "C" void CALLBACK TestA(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	MessageBoxW(hwnd, L"このメッセージはでないはずだよ。", nullptr, MB_ICONHAND);
}

extern "C" void show_credential_dialog()
{
	WCHAR       szUserName[256];
	WCHAR       szDomainName[256];
	WCHAR       szPassword[256];
	DWORD       dwUserNameSize = sizeof(szUserName) / sizeof(WCHAR);
	DWORD       dwDomainSize = sizeof(szDomainName) / sizeof(WCHAR);
	DWORD       dwPasswordSize = sizeof(szPassword) / sizeof(WCHAR);
	DWORD       dwResult;
	DWORD       dwOutBufferSize;
	PVOID       pOutBuffer;
	ULONG       uAuthPackage = 0;
	CREDUI_INFOW uiInfo;
	HANDLE      hToken;

	uiInfo.cbSize = sizeof(CREDUI_INFO);
	uiInfo.hwndParent = NULL;
	uiInfo.pszMessageText = L"message";
	uiInfo.pszCaptionText = L"caption";
	uiInfo.hbmBanner = NULL;

	dwOutBufferSize = 1024;
	pOutBuffer = (PVOID)LocalAlloc(LPTR, dwOutBufferSize);

	dwResult = CredUIPromptForWindowsCredentialsW(&uiInfo, 0, &uAuthPackage, NULL, 0, &pOutBuffer, &dwOutBufferSize, NULL, 0);
	if(dwResult != ERROR_SUCCESS)
	{
		LocalFree(pOutBuffer);
		_getwch();
		return;
	}

	CredUnPackAuthenticationBufferW(0, pOutBuffer, dwOutBufferSize, szUserName, &dwUserNameSize, szDomainName, &dwDomainSize, szPassword, &dwPasswordSize);

	szUserName[dwUserNameSize] = '\0';
	szPassword[dwPasswordSize] = '\0';
	wprintf_s(L"User=%s,Pass=%s\n", szUserName, szPassword);

	if(LogonUserW(szUserName, NULL, szPassword, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken))
		CloseHandle(hToken);
	else
		wprintf_s(L"ログオンに失敗。(%u)\n", GetLastError());

	RtlSecureZeroMemory(szPassword, sizeof(szPassword));
	RtlSecureZeroMemory(pOutBuffer, dwOutBufferSize);
	LocalFree(pOutBuffer);

	_getwch();
}

extern "C" void test_smartcard_class()
{
	SmartCardHelper * h = new SmartCardHelper();

	if(h->GetReadersCount() == 0)
	{
		wprintf_s(L"カードリーダーが接続されていないか認識されていません。\n");
		delete h;
		return;
	}

	wprintf_s(L"%u台のカードリーダーが見つかりました。\n", h->GetReadersCount());

	// カードがセットされたらIDを出力する処理を登録
	h->RegisterConnectionHandler([](ConnectionInfo * ci)
		{
			auto id = ci->Card->GetID();
			auto rname = ci->Reader->GetName();
			wprintf_s(L"[%s]カードを認識しました。(ID:%s)\n", rname.c_str(), id.c_str());
		});
	// カードが外された時の処理
	h->RegisterDisconnectionHandler([](ConnectionInfo * ci)
		{
			auto id = ci->Card->GetID();
			auto rname = ci->Reader->GetName();
			wprintf_s(L"[%s]カードが外されました。(ID:%s)\n", rname.c_str(), id.c_str());
		});
	// 監視を開始
	wprintf_s(L"カードリーダーの監視を開始します。\n");
	h->WatchAll();
	wprintf_s(L"何かキーを押すと終了します。\n");
	_getwch();
	// 監視を終了
	h->UnwatchAll();

	delete h;
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

	EnumAccounts();

#if false
	AllocateLsaHeap = [](ULONG s) { return reinterpret_cast<void*>(new unsigned char[s]); };
	FreeLsaHeap = [](void * b) { delete[] static_cast<unsigned char*>(b); };

	{
		auto lsa_str = CreateLsaString(L"LSA String Test");
		wprintf_s(L"LSA_STRING::Buffer=%hs\nLSA_STRING::Length=%u\nLSA_STRING::MaxLen=%u\n", lsa_str->Buffer, lsa_str->Length, lsa_str->MaximumLength);

		wstring w;
		LoadLsaString(lsa_str, w);
		wprintf_s(L"復元:%s\n", w.c_str());

		(*FreeLsaHeap)(lsa_str);
	}

	{
		auto lsa_str = CreateLsaString(L"LSA 文字列テスト");
		wprintf_s(L"LSA_STRING::Buffer=%hs\nLSA_STRING::Length=%u\nLSA_STRING::MaxLen=%u\n", lsa_str->Buffer, lsa_str->Length, lsa_str->MaximumLength);

		wstring w;
		LoadLsaString(lsa_str, w);
		wprintf_s(L"復元:%s\n", w.c_str());

		(*FreeLsaHeap)(lsa_str);
	}

	{
		wstring w(L"UNICODE_STRINGテスト");
		auto ustr = CreateUnicodeString(w);

		wprintf_s(L"UNICODE_STRING::Buffer=%s\nUNICODE_STRING::Length=%u\nUNICODE_STRING::MaxLen=%u\n", ustr->Buffer, ustr->Length, ustr->MaximumLength);

		(*FreeLsaHeap)(ustr);
	}

	_getwch();
#endif

#if false
	if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		CoInitializeEx(nullptr, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);
		show_credential_dialog();
		CoUninitialize();
	}
#endif
	//test_smartcard_class();

	_getwch();

	close_stdio(StandardIO::Error);
	close_stdio(StandardIO::Output);
	close_stdio(StandardIO::Input);
	FreeConsole();
}
