#include "ClassDef.hpp"
#include <winscard.h>
#include <vector>

CCredentialProviderFilter::CCredentialProviderFilter()
{
	this->instances = 0UL;
	this->AddRef();
}

CCredentialProviderFilter::~CCredentialProviderFilter()
{
}

HRESULT CCredentialProviderFilter::QueryInterface(REFIID riid, void ** ppvObject)
{
	if(riid == __uuidof(IUnknown))
	{
		this->AddRef();
		*ppvObject = this;
	}
	else if(riid == __uuidof(ICredentialProviderFilter))
	{
		this->AddRef();
		*ppvObject = this;
	}
	else
	{
		*ppvObject = nullptr;
	}
	return *ppvObject ? S_OK : E_NOINTERFACE;
}

ULONG CCredentialProviderFilter::AddRef()
{
	_InterlockedIncrement(&global_instances);
	return _InterlockedIncrement(&this->instances);
}

ULONG CCredentialProviderFilter::Release()
{
	auto decr = _InterlockedDecrement(&this->instances);
	if(decr == 0UL)
	{
		delete this;
	}
	_InterlockedDecrement(&global_instances);
	return decr;
}

HRESULT CCredentialProviderFilter::Filter(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags, GUID * rgclsidProviders, BOOL * rgbAllow, DWORD cProviders)
{
	SCARDCONTEXT ctx;

	// SDカードリーダーが使用できない場合はほかのプロバイダーを許可する
	if(SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr, &ctx) != SCARD_S_SUCCESS)
	{
		return E_ACCESSDENIED;
	}

	wchar_t * readers;
	DWORD scards_auto_allocate = SCARD_AUTOALLOCATE;
	SCardListReadersW(ctx, nullptr, (LPWSTR)&readers, &scards_auto_allocate);

	std::vector<std::wstring> * reader_list = new std::vector<std::wstring>;
	for(wchar_t * reader = readers; *reader; reader += (1 + wcslen(reader)))
	{
		reader_list->push_back(std::wstring(reader));
	}
	SCardFreeMemory(ctx, readers);

	for(auto it = reader_list->begin(), it_end = reader_list->end(); it < it_end; it = it++)
	{
		OutputDebugStringW(L"READER:");
		OutputDebugStringW((*it).c_str());
		OutputDebugStringW(L"\n");
	}
	delete reader_list;

	SCardReleaseContext(ctx);

	return S_OK;
}

HRESULT CCredentialProviderFilter::UpdateRemoteCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcsIn, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcsOut)
{
	return E_NOTIMPL;
}
