#pragma once
#include <Credentialprovider.h>
#include <winscard.h>
#include <vector>

extern ULONG global_instances;
extern "C" int MessageBoxFmt(HWND, const wchar_t *, UINT, const wchar_t *, ...);
extern "C" void DebugPrint(const wchar_t *, ...);

enum SmartCardProtocol : unsigned char
{
	Default, Optimal, Character, Block
};

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

class SmartCardReader
{
	SCARDCONTEXT context;
	std::wstring * name;
	SCARDHANDLE handle;
	DWORD protocol;
public:
	SmartCardReader(SCARDCONTEXT, const wchar_t *);
	~SmartCardReader();
	LONG Connect();
	LONG Connect(SmartCardProtocol);
	LONG Disconnect();
};

class SmartCardHelper
{
	SCARDCONTEXT context;
	std::vector<SmartCardReader> * readers;
public:
	SmartCardHelper();
	~SmartCardHelper();
	SCARDCONTEXT GetContext();
	unsigned short GetReadersCount();
	SmartCardReader * GetReaderAt(unsigned short);
private:
	void EnumerateReaders();
};
