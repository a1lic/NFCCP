#define SMARTCARDHELPER_CPP
#include "SmartCardHelper.hpp"
#include <process.h>
#include <minmax.h>
#include "../Common/Util.hpp"

SmartCard::SmartCard(SCARDCONTEXT context, SCARDHANDLE handle)
{
	this->context = context;
	this->handle = handle;

	// カードが認識されたときに呼ばれ、ここでカードからIDを取得する
	DWORD atr_size = 1024;
	BYTE * atr = new BYTE[1024];
	static const unsigned char cmd_get_info_card_id[] = {0xFF, 0xCA, 0x00, 0x00, 0x00};
	// PC/SC 2.0のGetData(0, 0)を送信
	if(SCardTransmit(this->handle, SCARD_PCI_T1, cmd_get_info_card_id, 5, nullptr, atr, &atr_size) == SCARD_S_SUCCESS)
	{
		if((atr[atr_size - 1] == 0x00) && (atr[atr_size - 2] == 0x90))
		{
			// OK
			atr_size -= 2;
			for(DWORD i = 0; i < atr_size; i++)
			{
				this->id.push_back(atr[i]);
			}
		}
		else
		{
			// ERROR
			this->id.push_back(0xDE);
			this->id.push_back(0xAD);
			for(DWORD i = 0; i < atr_size; i++)
			{
				this->id.push_back(atr[i]);
			}
		}
	}
	else
	{
		// ERROR
		this->id.push_back(0xDE);
		this->id.push_back(0xAD);
	}
	delete atr;
}

SmartCard::~SmartCard()
{
	if(!this->handle)
	{
		return;
	}

	if(SCardDisconnect(this->handle, SCARD_LEAVE_CARD) != SCARD_S_SUCCESS)
	{
		ExitProcess(4);
	}
}

std::wstring SmartCard::GetID()
{
	std::wstring str;
	BinaryToString(this->id.data(), this->id.size(), str);
	return str;
}

void SmartCard::PutIDToBinary(unsigned char * buffer)
{
	memcpy(buffer, this->id.data(), 8);
}

SmartCardReader::SmartCardReader(SCARDCONTEXT context, const wchar_t * reader_name)
{
	this->context = context;
	this->name = new std::wstring(reader_name);
	this->card = nullptr;
	this->protocol = SCARD_PROTOCOL_UNDEFINED;
	this->connection_thread = nullptr;
	DebugPrint(L"SmartCardReader() - 0x%p", this);
}

SmartCardReader::~SmartCardReader()
{
	DebugPrint(L"~SmartCardReader() - 0x%p", this);
	this->Disconnect();
	delete this->name;
}

LONG SmartCardReader::Connect()
{
	return this->Connect(SmartCardProtocol::Default);
}

LONG SmartCardReader::Connect(SmartCardProtocol protocol)
{
	DWORD dwPrefferedProtocols;
	switch(protocol)
	{
	case SmartCardProtocol::Default:
		dwPrefferedProtocols = SCARD_PROTOCOL_DEFAULT;
		break;
	case SmartCardProtocol::Optimal:
		dwPrefferedProtocols = SCARD_PROTOCOL_OPTIMAL;
		break;
	case SmartCardProtocol::Character:
		dwPrefferedProtocols = SCARD_PROTOCOL_T0;
		break;
	case SmartCardProtocol::Block:
		dwPrefferedProtocols = SCARD_PROTOCOL_T1;
		break;
	default:
		dwPrefferedProtocols = SCARD_PROTOCOL_DEFAULT;
		break;
	}

	SCARDHANDLE handle;
	auto result = SCardConnectW(this->context, this->name->c_str(), SCARD_SHARE_SHARED, dwPrefferedProtocols, &handle, &this->protocol);

	if(result != SCARD_S_SUCCESS)
	{
		if(this->connection_thread == nullptr)
		{
			DebugPrint(L"Attempt to connect reader `%s` - failed 0x%X", this->name->c_str(), result);
		}
		return result;
	}

	this->card = new SmartCard(this->context, handle);

	return result;
}

void SmartCardReader::Disconnect()
{
	if(this->card)
	{
		ConnectionInfo ci;
		for(auto i = this->disconnection_handler.begin(), e = this->disconnection_handler.end(); i != e; i++)
		{
			ci.Helper = nullptr;
			ci.Reader = this;
			ci.Card = this->card;
			ci.Param = i->second;

			(*(i->first))(&ci);
		}
		if(this->helper)
		{
			this->helper->dispatch_disconnection_handler(this);
		}
		delete this->card;
		this->card = nullptr;
	}
}

void SmartCardReader::RegisterConnectionHandler(ConnectionHandler handler)
{
	this->connection_handler.insert_or_assign(handler, nullptr);
}

void SmartCardReader::RegisterConnectionHandler(ConnectionHandler handler, void * param)
{
	this->connection_handler.insert_or_assign(handler, param);
}

void SmartCardReader::RegisterDisconnectionHandler(ConnectionHandler handler)
{
	this->disconnection_handler.insert_or_assign(handler, nullptr);
}

void SmartCardReader::RegisterDisconnectionHandler(ConnectionHandler handler, void * param)
{
	this->disconnection_handler.insert_or_assign(handler, param);
}

bool SmartCardReader::StartConnection()
{
	if(this->connection_thread)
	{
		return false;
	}

	unsigned int tid;
	this->connection_thread = reinterpret_cast<HANDLE>(_beginthreadex(
		nullptr,
		0,
		[](void *t)
		{
			static_cast<SmartCardReader *>(t)->ConnectionThread();
			return 0u;
		},
		this,
		0,
		&tid)
	);
	auto c = this->name->size();
	DebugPrint(L"Start connection thread for `%s' as thread %u", this->name->c_str(), tid);

	return (this->connection_thread != nullptr);
}

void SmartCardReader::StopConnection()
{
	if(this->connection_thread == nullptr)
	{
		return;
	}

	DebugPrint(L"Shutdowning connection thread.");
	this->do_exit_thread_loop = true;
	WaitForSingleObject(this->connection_thread, INFINITE);

	CloseHandle(this->connection_thread);
	this->connection_thread = nullptr;
}

void SmartCardReader::ConnectionThread()
{
	this->do_exit_thread_loop = false;

	// ループ終了
	while(!this->do_exit_thread_loop)
	{
		auto result = this->Connect();
		if(result != SCARD_S_SUCCESS)
		{
			Sleep(500);
			continue;
		}

		ConnectionInfo ci;
		// カードが認識されたことをハンドラーに通知する
		for(auto i = this->connection_handler.begin(), e = this->connection_handler.end(); i != e; i++)
		{
			ci.Helper = nullptr;
			ci.Reader = this;
			ci.Card = this->card;
			ci.Param = i->second;

			(*(i->first))(&ci);
		}
		if(this->helper)
		{
			this->helper->dispatch_connection_handler(this);
		}

		DWORD state = SCARD_UNKNOWN;
		auto h = this->card->GetHandle();
		// カードが認識されている間はループを止める
		while(!this->do_exit_thread_loop && (state != SCARD_ABSENT))
		{
			if(SCardStatusW(h, nullptr, nullptr, &state, nullptr, nullptr, nullptr) != SCARD_S_SUCCESS)
			{
				// ScardStatusが失敗した場合はハンドルが無効なのでループを抜ける
				break;
			}
			Sleep(250);
		}

		this->Disconnect();
	}
}

SmartCardHelper::SmartCardHelper()
{
	auto sc_result = SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr, &this->context);
	if(sc_result != SCARD_S_SUCCESS)
	{
		DebugPrint(L"SmartCardHelper failed (0x%X). - 0x%p", sc_result, this);
		throw;
	}

	this->EnumerateReaders();
	DebugPrint(L"SmartCardHelper() - 0x%p", this);
}

SmartCardHelper::~SmartCardHelper()
{
	DebugPrint(L"~SmartCardHelper() - 0x%p", this);
	SCardReleaseContext(this->context);
}

SCARDCONTEXT SmartCardHelper::GetContext()
{
	return this->context;
}

unsigned short SmartCardHelper::GetReadersCount()
{
	return (unsigned short)(this->readers.size());
}

SmartCardReader * SmartCardHelper::GetReaderAt(unsigned short index)
{
	if(index >= this->GetReadersCount())
	{
		return nullptr;
	}
	return this->readers.at(index);
}

void SmartCardHelper::RegisterConnectionHandler(ConnectionHandler handler)
{
	this->connection_handler.insert_or_assign(handler, nullptr);
}

void SmartCardHelper::RegisterConnectionHandler(ConnectionHandler handler, void * param)
{
	this->connection_handler.insert_or_assign(handler, param);
}

void SmartCardHelper::RegisterDisconnectionHandler(ConnectionHandler handler)
{
	this->disconnection_handler.insert_or_assign(handler, nullptr);
}

void SmartCardHelper::RegisterDisconnectionHandler(ConnectionHandler handler, void * param)
{
	this->disconnection_handler.insert_or_assign(handler, param);
}

void SmartCardHelper::dispatch_connection_handler(SmartCardReader * reader)
{
	ConnectionInfo ci;
	for(auto i = this->connection_handler.begin(), e = this->connection_handler.end(); i != e; i++)
	{
		ci.Helper = this;
		ci.Reader = reader;
		ci.Card = reader->GetCard();
		ci.Param = i->second;

		(*(i->first))(&ci);
	}
}

void SmartCardHelper::dispatch_disconnection_handler(SmartCardReader * reader)
{
	ConnectionInfo ci;
	for(auto i = this->disconnection_handler.begin(), e = this->disconnection_handler.end(); i != e; i++)
	{
		ci.Helper = this;
		ci.Reader = reader;
		ci.Card = reader->GetCard();
		ci.Param = i->second;

		(*(i->first))(&ci);
	}
}

void SmartCardHelper::WatchAll()
{
	for(auto i = this->readers.begin(), e = this->readers.end(); i < e; i++)
	{
		(*i)->StartConnection();
	}
}

void SmartCardHelper::UnwatchAll()
{
	for(auto i = this->readers.begin(), e = this->readers.end(); i < e; i++)
	{
		(*i)->StopConnection();
	}
}

void SmartCardHelper::EnumerateReaders()
{
	wchar_t * reader_names;
	DWORD scards_auto_allocate = SCARD_AUTOALLOCATE;
	LONG sc_result = SCardListReadersW(this->context, nullptr, (LPWSTR)&reader_names, &scards_auto_allocate);
	if(sc_result != SCARD_S_SUCCESS)
	{
		DebugPrint(L"SCardListReaders failed. 0x%X", sc_result);
		return;
	}

	for(auto reader_name = reader_names; *reader_name; reader_name += (1 + wcslen(reader_name)))
	{
		SmartCardReader * reader = new SmartCardReader(this->context, reader_name);
		reader->SetHelper(this);
		this->readers.push_back(reader);
		if(this->readers.size() >= USHRT_MAX)
		{
			DebugPrint(L"Too many readers.");
			break;
		}
	}

	SCardFreeMemory(this->context, reader_names);
}
