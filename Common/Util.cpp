#include <string>
#include <Windows.h>

using std::wstring;

void BinaryToString(const void * buf, size_t buf_size, wstring & str)
{
	static const wchar_t digit_t[] = {L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F'};
	wchar_t digit[4];
	digit[3] = L'\0';

	str.clear();

	for(size_t i = 0; i < buf_size; i++)
	{
		auto uc = static_cast<const unsigned char*>(buf)[i];
		if(i == 0)
		{
			digit[0] = digit_t[(uc & 0xF0) >> 4];
			digit[1] = digit_t[uc & 0x0F];
			digit[2] = L'\0';
		}
		else
		{
			digit[0] = L' ';
			digit[1] = digit_t[(uc & 0xF0) >> 4];
			digit[2] = digit_t[uc & 0x0F];
		}

		str.append(digit);
	}
}

extern "C" int MessageBoxFmt(HWND hWnd, const wchar_t * lpCaption, UINT uType, const wchar_t * lpText, ...)
{
	auto msg = new wchar_t[1024];
	va_list ap;
	va_start(ap, lpText);
	vswprintf_s(msg, 1024, lpText, ap);
	va_end(ap);
	auto r = MessageBoxW(hWnd, msg, lpCaption, uType);
	delete[] msg;
	return r;
}

#if defined(_DEBUG)
extern "C" void DebugPrint(const wchar_t * lpOutputString, ...)
{
	auto msg = new wchar_t[1024];
	va_list ap;
	va_start(ap, lpOutputString);
	vswprintf_s(msg, 1024, lpOutputString, ap);
	va_end(ap);
	if(wcslen(msg) == 1023)
	{
		msg[1022] = L'\n';
	}
	else
	{
		wcscat_s(msg, 1024, L"\n");
	}
	OutputDebugStringW(msg);
	delete[] msg;
}
#endif
