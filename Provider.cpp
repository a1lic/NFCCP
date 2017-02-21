#include "Provider.hpp"
#include "Util.hpp"
#include <ShlGuid.h>
#include <Shlwapi.h>

const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR CCredentialProvider::_fields[] = {
	{0, CREDENTIAL_PROVIDER_FIELD_TYPE::CPFT_TILE_IMAGE, nullptr, GUID_NULL},
	{1, CREDENTIAL_PROVIDER_FIELD_TYPE::CPFT_LARGE_TEXT, L"ICカード", GUID_NULL},
	{2, CREDENTIAL_PROVIDER_FIELD_TYPE::CPFT_SMALL_TEXT, L"カードをセットしてログオン", GUID_NULL}};

const FIELD_STATE_PAIR CCredentialProvider::field_states[] = {
	{CREDENTIAL_PROVIDER_FIELD_STATE::CPFS_DISPLAY_IN_BOTH, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE::CPFIS_NONE},
	{CREDENTIAL_PROVIDER_FIELD_STATE::CPFS_DISPLAY_IN_BOTH, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE::CPFIS_NONE},
	{CREDENTIAL_PROVIDER_FIELD_STATE::CPFS_DISPLAY_IN_SELECTED_TILE, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE::CPFIS_NONE}};

CCredentialProvider::CCredentialProvider()
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProvider", this, "CCredentialProvider");
	this->instances = 0UL;
	this->AddRef();

	this->fields = new CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR[3];
	for(int i = 0; i < 3; i++)
	{
		this->fields[i] = _fields[i];
		if(_fields[i].pszLabel)
		{
			SHStrDupW(_fields[i].pszLabel, &(this->fields[i].pszLabel));
		}
	}

	this->renew_credentials = true;
	this->credential = nullptr;
}

CCredentialProvider::~CCredentialProvider()
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProvider", this, "~CCredentialProvider");
	if(this->credential)
	{
		this->credential->Release();
		this->credential = nullptr;
	}
	for(int i = 0; i < 3; i++)
	{
		if(this->fields[i].pszLabel)
		{
			CoTaskMemFree(this->fields[i].pszLabel);
			this->fields[i].pszLabel = nullptr;
		}
	}
	delete this->fields;
}

HRESULT CCredentialProvider::QueryInterface(REFIID riid, void ** ppvObject)
{
#pragma warning(push)
#pragma warning(disable:4838)
	static const QITAB interfaces[] = {
		QITABENT(CCredentialProvider, ICredentialProvider),
		{}
	};
#pragma warning(pop)
	return QISearch(this, interfaces, riid, ppvObject);
}

ULONG CCredentialProvider::AddRef()
{
	global_instances++;
	return ++(this->instances);
}

ULONG CCredentialProvider::Release()
{
	global_instances--;
	auto new_count = --(this->instances);
	if(new_count == 0)
	{
		delete this;
	}
	return new_count;
}

HRESULT CCredentialProvider::SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProvider", this, "SetUsageScenario");
	HRESULT r;

	switch(cpus)
	{
	case CREDENTIAL_PROVIDER_USAGE_SCENARIO::CPUS_LOGON:
	case CREDENTIAL_PROVIDER_USAGE_SCENARIO::CPUS_UNLOCK_WORKSTATION:
	case CREDENTIAL_PROVIDER_USAGE_SCENARIO::CPUS_CREDUI:
		this->usage_scenario = cpus;
		this->valid_scenario = true;
		this->renew_credentials = true;
		r = S_OK;
		break;

	case CREDENTIAL_PROVIDER_USAGE_SCENARIO::CPUS_CHANGE_PASSWORD:
		this->valid_scenario = false;
		r = E_NOTIMPL;
		break;

	default:
		this->valid_scenario = false;
		r = E_INVALIDARG;
		break;
	}
	return r;
}

HRESULT CCredentialProvider::SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcs)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProvider::Advise(ICredentialProviderEvents *, UINT_PTR)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProvider::UnAdvise()
{
	return E_NOTIMPL;
}

HRESULT CCredentialProvider::GetFieldDescriptorCount(DWORD *pdwCount)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProvider", this, "GetFieldDescriptorCount");
	*pdwCount = 3;
	return S_OK;
}

HRESULT CCredentialProvider::GetFieldDescriptorAt(DWORD dwIndex, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR ** ppcpfd)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProvider", this, "GetFieldDescriptorAt");
	if(dwIndex >= 3)
	{
		return E_INVALIDARG;
	}

	*ppcpfd = static_cast<CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR*>(CoTaskMemAlloc(sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR)));
	**ppcpfd = this->fields[dwIndex];
	if(this->fields[dwIndex].pszLabel)
	{
		SHStrDupW(this->fields[dwIndex].pszLabel, &(*ppcpfd)->pszLabel);
	}
	return S_OK;
}

HRESULT CCredentialProvider::GetCredentialCount(DWORD * pdwCount, DWORD * pdwDefault, BOOL * pbAutoLogonWithDefault)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProvider", this, "GetCredentialCount");
	if(this->renew_credentials)
	{
		this->renew_credentials = false;
		if(this->credential)
		{
			this->credential->Release();
			this->credential = nullptr;
		}
		if(this->valid_scenario)
		{
			this->credential = new CCredentialProviderCredential();
		}
	}

	*pdwCount = 1;
	*pdwDefault = CREDENTIAL_PROVIDER_NO_DEFAULT;
	*pbAutoLogonWithDefault = TRUE;

	return S_OK;
}

HRESULT CCredentialProvider::GetCredentialAt(DWORD dwIndex, ICredentialProviderCredential ** ppcpc)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProvider", this, "GetCredentialAt");
	if(dwIndex != 0)
	{
		return E_INVALIDARG;
	}

	return this->credential->QueryInterface(__uuidof(ICredentialProviderCredential), (void **)ppcpc);
}
