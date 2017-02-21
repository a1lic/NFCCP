#pragma once

#include <Windows.h>
#include <credentialprovider.h>

class CCredentialProviderFilter : public ICredentialProviderFilter
{
private:
	ULONG instances;
public:
	CCredentialProviderFilter();
private:
	~CCredentialProviderFilter();
public:
	// IUnknown
	HRESULT QueryInterface(REFIID, void **);
	ULONG AddRef();
	ULONG Release();
	// ICredentialProviderFilter
	HRESULT Filter(CREDENTIAL_PROVIDER_USAGE_SCENARIO, DWORD, GUID *, BOOL *, DWORD);
	HRESULT UpdateRemoteCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *);
};
