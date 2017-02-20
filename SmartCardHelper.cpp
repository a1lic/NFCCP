#include "ClassDef.hpp"
#include <process.h>

SmartCard::SmartCard(SCARDCONTEXT context, SCARDHANDLE handle)
{
	this->context = context;
	this->handle = handle;

	// カードが認識されたときに呼ばれ、ここでカードからIDを取得する
	DWORD atr_size = 256;
	BYTE * atr = new BYTE[256];
	static const unsigned char cmd_get_info_card_id[] = {0xFF, 0xCA, 0x00, 0x00, 0xFE};
	// PC/SC 2.0のGetData(0, 0)を送信
	if(SCardTransmit(this->handle, SCARD_PCI_T1, cmd_get_info_card_id, 5, nullptr, atr, &atr_size) == SCARD_S_SUCCESS)
	{
		// OK
		for(DWORD i = 2; i < atr_size; i++)
		{
			this->id.push_back(atr[i]);
		}
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
	wchar_t hex[4];
	bool first_element = true;
	for(auto i = this->id.begin(), e = this->id.end(); i < e; i++)
	{
		// IDを16進ダンプ
		_i64tow_s(*i, hex, 4, 16);
		if(hex[1] == L'\0')
		{
			// 変換後1桁の場合は0を付け加えて2桁にする
			hex[1] = hex[0];
			hex[0] = L'0';
		}
		hex[2] = L'\0';
		if(!first_element)
		{
			// バイト列の2番目以降だった場合は空白を追加
			str.append(L" ");
		}
		str.append(hex);

		first_element = false;
	}
	return str;
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
		for(auto i = this->disconnection_handler.begin(), e = this->disconnection_handler.end(); i < e; i++)
		{
			(*i)(this, this->card);
		}
		delete this->card;
		this->card = nullptr;
	}
}

void SmartCardReader::RegisterConnectionHandler(ConnectionHandler handler)
{
	this->connection_handler.push_back(handler);
}

void SmartCardReader::RegisterDisconnectionHandler(DisconnectionHandler handler)
{
	this->disconnection_handler.push_back(handler);
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

		// カードが認識されたことをハンドラーに通知する
		for(auto i = this->connection_handler.begin(), e = this->connection_handler.end(); i < e; i++)
		{
			(*i)(this, this->card);
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
	return &(this->readers.at(index));
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
		this->readers.push_back(*reader);
		if(this->readers.size() >= USHRT_MAX)
		{
			DebugPrint(L"Too many readers.");
			break;
		}
	}

	SCardFreeMemory(this->context, reader_names);
}
