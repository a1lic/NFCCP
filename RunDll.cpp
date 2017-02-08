#include "ClassDef.hpp"
#include <winscard.h>
#include <string>
#include <vector>

extern "C" void CALLBACK TestA(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	MessageBoxW(hwnd, L"このメッセージはでないはずだよ。", nullptr, MB_ICONHAND);
}

extern "C" void CALLBACK TestW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow)
{
	if(IsDebuggerPresent())
	{
		__debugbreak();
	}

	SmartCardHelper * h = new SmartCardHelper();

	if(h->GetReadersCount() == 0)
	{
		DebugPrint(L"No card reader is installed on system.");
		delete h;
		return;
	}

	LONG sc_result;

	DebugPrint(L"%u of card readers are installed on system.", h->GetReadersCount());

	auto r = h->GetReaderAt(0);

	sc_result = r->Connect();
	if(sc_result != SCARD_S_SUCCESS)
	{
		MessageBoxFmt(hwnd, L"SuicaCP", MB_ICONASTERISK, L"SCardConnect-0x%x", sc_result);
	}

	delete h;

}
