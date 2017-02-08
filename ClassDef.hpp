#pragma once
#include <Credentialprovider.h>

extern ULONG global_instances;
extern "C" int MessageBoxFmt(HWND, const wchar_t *, UINT, const wchar_t *, ...);
extern "C" void DebugPrint(const wchar_t *, ...);

class __declspec(uuid("D2BB2AE9-8214-4499-BC3E-D2EAB632EC22")) CClassFactory : public IClassFactory
{
private:
	ULONG instances;
public:
	CClassFactory();
private:
	~CClassFactory();
public:
	HRESULT QueryInterface(REFIID, void **);
	ULONG AddRef();
	ULONG Release();
	HRESULT CreateInstance(IUnknown *, REFIID, void **);
	HRESULT LockServer(BOOL);
};


class CCredentialProviderFilter : public ICredentialProviderFilter
{
private:
	ULONG instances;
public:
	CCredentialProviderFilter();
private:
	~CCredentialProviderFilter();
public:
	HRESULT QueryInterface(REFIID, void **);
	ULONG AddRef();
	ULONG Release();
	HRESULT Filter(CREDENTIAL_PROVIDER_USAGE_SCENARIO, DWORD, GUID *, BOOL *, DWORD);
	HRESULT UpdateRemoteCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *);
};


class CCredentialProvider : public ICredentialProvider
{
private:
	ULONG instances;
public:
	CCredentialProvider();
private:
	~CCredentialProvider();
public:
	HRESULT QueryInterface(REFIID, void **);
	ULONG AddRef();
	ULONG Release();
	HRESULT SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO, DWORD);
	HRESULT SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *);
	HRESULT Advise(ICredentialProviderEvents *, UINT_PTR);
	HRESULT UnAdvise();
	HRESULT GetFieldDescriptorCount(DWORD *);
	HRESULT GetFieldDescriptorAt(DWORD, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **);
	HRESULT GetCredentialCount(DWORD *, DWORD *, BOOL *);
	HRESULT GetCredentialAt(DWORD, ICredentialProviderCredential **);
};
