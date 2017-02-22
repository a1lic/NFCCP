#include "Filter.hpp"
#include <Shlwapi.h>
#include "Util.hpp"
#include <stdio.h>

const GUID CCredentialProviderFilter::blacklist[7] = {
	// Smartcard Reader Selection Provider
	{0x1B283861,0x754F,0x4022,{0xAD,0x47,0xA5,0xEA,0xAA,0x61,0x88,0x94}},
	// Smartcard WinRT Provider
	{0x1EE7337F,0x85AC,0x45E2,{0xA2,0x3C,0x37,0xC7,0x53,0x20,0x97,0x69}},
	// PasswordProvider
	{0x60B78E88,0xEAD8,0x445C,{0x9C,0xFD,0x0B,0x87,0xF7,0x4E,0xA6,0xCD}},
	// PasswordProvider V1
	{0x6F45DC1E,0x5384,0x457A,{0xBC,0x13,0x2C,0xD8,0x1B,0x0D,0x28,0xED}},
	// Smartcard Credential Provider
	{0x8FD7E19C,0x3BF7,0x489B,{0xA7,0x2C,0x84,0x6A,0xB3,0x67,0x8C,0x96}},
	// Smartcard Pin Provider
	{0x94596C7E,0x3744,0x41CE,{0x89,0x3E,0xBB,0xF0,0x91,0x22,0xF7,0x6A}},
	// WLIDCredentialProvider
	{0xF8A0B131,0x5F68,0x486c,{0x80,0x40,0x7E,0x8F,0xC3,0xC8,0x5B,0xB6}}};

CCredentialProviderFilter::CCredentialProviderFilter()
{
	this->instances = 0;
	this->AddRef();
}

CCredentialProviderFilter::~CCredentialProviderFilter()
{
}

HRESULT CCredentialProviderFilter::QueryInterface(REFIID riid, void ** ppvObject)
{
#pragma warning(push)
#pragma warning(disable:4838)
	static const QITAB interfaces[] = {
		QITABENT(CCredentialProviderFilter, ICredentialProviderFilter),
		{}
	};
#pragma warning(pop)
	return QISearch(this, interfaces, riid, ppvObject);
}

ULONG CCredentialProviderFilter::AddRef()
{
	global_instances++;
	return ++(this->instances);
}

ULONG CCredentialProviderFilter::Release()
{
	global_instances--;
	auto new_count = --(this->instances);
	if(new_count == 0)
	{
		delete this;
	}
	return new_count;
}

HRESULT CCredentialProviderFilter::Filter(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags, GUID * rgclsidProviders, BOOL * rgbAllow, DWORD cProviders)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderFilter", this, "Filter");
	DebugPrint(L"cpus=%u dwFlags=%X Providers=%u", cpus, dwFlags, cProviders);
	wchar_t idstr[39];
	for(DWORD i = 0; i < cProviders; i++)
	{
		guid_to_string(rgclsidProviders[i], idstr);
		CLSID_PasswordCredentialProvider;
		for(int j = 0; j < 7; j++)
		{
			if(IsEqualGUID(rgclsidProviders[i], CCredentialProviderFilter::blacklist[j]))
			{
				rgbAllow[i] = FALSE;
				break;
			}
		}
		DebugPrint(L"%c:%2u:%s", rgbAllow[i] ? L'*' : L' ' , i, idstr);
	}
	return S_OK;
}

HRESULT CCredentialProviderFilter::UpdateRemoteCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcsIn, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcsOut)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderFilter", this, "UpdateRemoteCredential");
	return E_NOTIMPL;
}
