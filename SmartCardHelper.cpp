#include "ClassDef.hpp"

SmartCardReader::SmartCardReader(SCARDCONTEXT context, const wchar_t * reader_name)
{
	this->context = context;
	this->name = new std::wstring(reader_name);
	this->handle = 0;
	this->protocol = SCARD_PROTOCOL_UNDEFINED;
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

	auto result = SCardConnectW(this->context, this->name->c_str(), SCARD_SHARE_SHARED, dwPrefferedProtocols, &this->handle, &this->protocol);

	if(result != SCARD_S_SUCCESS)
	{
		DebugPrint(L"Attempt to connect reader `%s` - failed 0x%X", this->name->c_str(), result);
		this->handle = 0;
	}

	return result;
}

LONG SmartCardReader::Disconnect()
{
	LONG result;
	if(this->handle != 0)
	{
		result = SCardDisconnect(this->handle, SCARD_LEAVE_CARD);
		if(result == SCARD_S_SUCCESS)
		{
			this->handle = 0;
		}
	}
	else
	{
		result = SCARD_S_SUCCESS;
	}
	return result;
}

SmartCardHelper::SmartCardHelper()
{
	auto sc_result = SCardEstablishContext(SCARD_SCOPE_SYSTEM, nullptr, nullptr, &this->context);
	if(sc_result != SCARD_S_SUCCESS)
	{
		DebugPrint(L"SmartCardHelper failed (0x%X). - 0x%p", sc_result, this);
		throw;
	}

	this->readers = new std::vector<SmartCardReader>;
	this->EnumerateReaders();
	DebugPrint(L"SmartCardHelper() - 0x%p", this);
}

SmartCardHelper::~SmartCardHelper()
{
	DebugPrint(L"~SmartCardHelper() - 0x%p", this);
	delete this->readers;
	SCardReleaseContext(this->context);
}

SCARDCONTEXT SmartCardHelper::GetContext()
{
	return this->context;
}

unsigned short SmartCardHelper::GetReadersCount()
{
	return (unsigned short)(this->readers->size());
}

SmartCardReader * SmartCardHelper::GetReaderAt(unsigned short index)
{
	if(index >= this->GetReadersCount())
	{
		return nullptr;
	}
	return &(this->readers->at(index));
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
		this->readers->push_back(*reader);
		if(this->readers->size() >= USHRT_MAX)
		{
			DebugPrint(L"Too many readers.");
			break;
		}
	}

	SCardFreeMemory(this->context, reader_names);
}
