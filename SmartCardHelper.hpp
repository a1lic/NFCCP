#pragma once

#include <Windows.h>
#include <winscard.h>
#include <string>
#include <vector>

enum SmartCardProtocol : unsigned char
{
	Default, Optimal, Character, Block
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
