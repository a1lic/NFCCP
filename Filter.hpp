#pragma once

#include <Windows.h>
#include <credentialprovider.h>

class CCredentialProviderFilter : public ICredentialProviderFilter
{
private:
	ULONG instances;
	static const GUID blacklist[7];
public:
	CCredentialProviderFilter();
private:
	~CCredentialProviderFilter();
public:
	// IUnknown
	virtual HRESULT QueryInterface(REFIID, void **);
	virtual ULONG AddRef();
	virtual ULONG Release();
	// ICredentialProviderFilter
	virtual HRESULT Filter(CREDENTIAL_PROVIDER_USAGE_SCENARIO, DWORD, GUID *, BOOL *, DWORD);
	virtual HRESULT UpdateRemoteCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *);
};
