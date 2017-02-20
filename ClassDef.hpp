#pragma once
#include <Credentialprovider.h>
#include <winscard.h>
#include <vector>
#include <map>
#include <functional>

extern ULONG global_instances;
extern "C" int MessageBoxFmt(HWND, const wchar_t *, UINT, const wchar_t *, ...);
#if defined(_DEBUG)
extern "C" void DebugPrint(const wchar_t *, ...);
#else
#define DebugPrint(msg, ...)
#endif

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

typedef void(* ConnectionHandler)(class SmartCardReader *, class SmartCard *);
typedef void(* DisconnectionHandler)(class SmartCardReader *, class SmartCard *);

class SmartCard
{
	SCARDCONTEXT context;
	SCARDHANDLE handle;
	std::vector<unsigned char> id;
public:
	SmartCard(SCARDCONTEXT, SCARDHANDLE);
	~SmartCard();
	std::wstring GetID();
	inline SCARDHANDLE GetHandle() { return this->handle; }
};

class SmartCardReader
{
	SCARDCONTEXT context;
	std::wstring * name;
	SmartCard * card;
	class SmartCardHelper * helper;
	DWORD protocol;
	HANDLE connection_thread;
	std::vector<ConnectionHandler> connection_handler;
	std::vector<DisconnectionHandler> disconnection_handler;
	bool do_exit_thread_loop;
public:
	SmartCardReader(SCARDCONTEXT, const wchar_t *);
	~SmartCardReader();
	inline std::wstring GetName() { return std::wstring(*name); }
	inline SmartCard * GetCard() { return this->card; }
	inline void SetHelper(SmartCardHelper * h) { this->helper = h; }
	LONG Connect();
	LONG Connect(SmartCardProtocol);
	void Disconnect();
	void RegisterConnectionHandler(ConnectionHandler);
	void RegisterDisconnectionHandler(DisconnectionHandler);
	bool StartConnection();
	void StopConnection();
private:
	void ConnectionThread();
};

class SmartCardHelper
{
	SCARDCONTEXT context;
	std::vector<SmartCardReader *> readers;
	std::vector<ConnectionHandler> connection_handler;
	std::vector<DisconnectionHandler> disconnection_handler;
public:
	SmartCardHelper();
	~SmartCardHelper();
	SCARDCONTEXT GetContext();
	unsigned short GetReadersCount();
	SmartCardReader * GetReaderAt(unsigned short);
	void RegisterConnectionHandler(ConnectionHandler);
	void RegisterDisconnectionHandler(DisconnectionHandler);
	void dispatch_connection_handler(SmartCardReader *);
	void dispatch_disconnection_handler(SmartCardReader *);
	void WatchAll();
	void UnwatchAll();
private:
	void EnumerateReaders();
};
