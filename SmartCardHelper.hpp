#pragma once

#include <Windows.h>
#include <winscard.h>
#include <string>
#include <vector>
#include <map>

using std::vector;

enum SmartCardProtocol : unsigned char
{
	Default, Optimal, Character, Block
};

struct ConnectionInfo
{
	class SmartCardHelper * Helper;
	class SmartCardReader * Reader;
	class SmartCard * Card;
	void * Param;
};
typedef struct ConnectionInfo ConnectionInfo;

typedef void(* ConnectionHandler)(ConnectionInfo *);

class SmartCard
{
	SCARDCONTEXT context;
	SCARDHANDLE handle;
	std::vector<unsigned char> id;
public:
	SmartCard(SCARDCONTEXT, SCARDHANDLE);
	~SmartCard();
	std::wstring GetID();
	void PutIDToBinary(unsigned char *);
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
	std::map<ConnectionHandler, void *> connection_handler;
	std::map<ConnectionHandler, void *> disconnection_handler;
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
	void RegisterConnectionHandler(ConnectionHandler, void *);
	void RegisterDisconnectionHandler(ConnectionHandler);
	void RegisterDisconnectionHandler(ConnectionHandler, void *);
	bool StartConnection();
	void StopConnection();
private:
	void ConnectionThread();
};

class SmartCardHelper
{
	SCARDCONTEXT context;
	std::vector<SmartCardReader *> readers;
	std::map<ConnectionHandler, void *> connection_handler;
	std::map<ConnectionHandler, void *> disconnection_handler;
public:
	SmartCardHelper();
	~SmartCardHelper();
	SCARDCONTEXT GetContext();
	unsigned short GetReadersCount();
	SmartCardReader * GetReaderAt(unsigned short);
	void RegisterConnectionHandler(ConnectionHandler);
	void RegisterConnectionHandler(ConnectionHandler, void *);
	void RegisterDisconnectionHandler(ConnectionHandler);
	void RegisterDisconnectionHandler(ConnectionHandler, void *);
	void dispatch_connection_handler(SmartCardReader *);
	void dispatch_disconnection_handler(SmartCardReader *);
	void WatchAll();
	void UnwatchAll();
private:
	void EnumerateReaders();
};

#if defined(SMARTCARDHELPER_CPP)
template class vector<unsigned char>;
template class vector<SmartCardReader *>;
#else
extern template class vector<unsigned char>;
extern template class vector<SmartCardReader *>;
#endif
