#include "Credential.hpp"
#include <Shlwapi.h>
#include "Util.hpp"
#include "Provider.hpp"

CCredentialProviderCredential::CCredentialProviderCredential()
{
	this->instances = 0;
	this->AddRef();
	this->events = nullptr;
	this->parent = nullptr;
}

CCredentialProviderCredential::~CCredentialProviderCredential()
{
	if(this->events)
	{
		this->events->Release();
		this->events = nullptr;
	}
}

HRESULT CCredentialProviderCredential::QueryInterface(REFIID riid, void ** ppvObject)
{
#pragma warning(push)
#pragma warning(disable:4838)
	static const QITAB interfaces[] = {
		QITABENT(CCredentialProviderCredential, ICredentialProviderCredential),
		{}
	};
#pragma warning(pop)
	return QISearch(this, interfaces, riid, ppvObject);
}

ULONG CCredentialProviderCredential::AddRef()
{
	_InterlockedIncrement(&global_instances);
	return _InterlockedIncrement(&this->instances);
}

ULONG CCredentialProviderCredential::Release()
{
	auto decr = _InterlockedDecrement(&this->instances);
	if(decr == 0)
	{
		delete this;
	}
	_InterlockedDecrement(&global_instances);
	return decr;
}

HRESULT CCredentialProviderCredential::Advise(ICredentialProviderCredentialEvents * pcpce)
{
	if(this->events)
	{
		this->events->Release();
		this->events = nullptr;
	}
	pcpce->OnCreatingWindow(&this->parent);
	return pcpce->QueryInterface(__uuidof(ICredentialProviderCredentialEvents), (void **)&this->events);
}

HRESULT CCredentialProviderCredential::UnAdvise()
{
	if(this->events)
	{
		this->events->Release();
		this->events = nullptr;
	}
	this->parent = nullptr;
	return S_OK;
}

HRESULT CCredentialProviderCredential::SetSelected(BOOL * pbAutoLogon)
{
	*pbAutoLogon = FALSE;
	return S_OK;
}

HRESULT CCredentialProviderCredential::SetDeselected()
{
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetFieldState(DWORD dwFieldID, CREDENTIAL_PROVIDER_FIELD_STATE * pcpfs, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE * pcpfis)
{
	*pcpfs = CCredentialProvider::field_states[dwFieldID].cpfs;
	*pcpfis = CCredentialProvider::field_states[dwFieldID].cpfis;
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetStringValue(DWORD dwFieldID, LPWSTR * ppwsz)
{
	switch(dwFieldID)
	{
	case 1:
		SHStrDupW(CCredentialProvider::_fields[0].pszLabel, ppwsz);
		break;
	case 2:
		SHStrDupW(CCredentialProvider::_fields[1].pszLabel, ppwsz);
		break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetBitmapValue(DWORD dwFieldID, HBITMAP * phbmp)
{
	if(dwFieldID != 0)
	{
		return E_INVALIDARG;
	}
	*phbmp = nullptr;
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetCheckboxValue(DWORD, BOOL *, LPWSTR *)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::GetSubmitButtonValue(DWORD dwFieldID, DWORD * pdwAdjacentTo)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::GetComboBoxValueCount(DWORD, DWORD *, DWORD *)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::GetComboBoxValueAt(DWORD, DWORD, LPWSTR *)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::SetStringValue(DWORD dwFieldID, LPCWSTR pwz)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::SetCheckboxValue(DWORD, BOOL)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::SetComboBoxSelectedValue(DWORD, DWORD)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::CommandLinkClicked(DWORD)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE * pcpgsr, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *, LPWSTR * ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON * pcpsiOptionalStatusIcon)
{
	*pcpgsr = CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE::CPGSR_NO_CREDENTIAL_NOT_FINISHED;
	SHStrDupW(L"カードを認識できませんでした。", ppwszOptionalStatusText);
	*pcpsiOptionalStatusIcon = CREDENTIAL_PROVIDER_STATUS_ICON::CPSI_ERROR;

	// ここでSmartCardHelperと連携

	return S_OK;
}

HRESULT CCredentialProviderCredential::ReportResult(NTSTATUS, NTSTATUS, LPWSTR * ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON * pcpsiOptionalStatusIcon)
{
	MessageBoxFmt(this->parent, L"CCredentialProviderCredential::ReportResult", 0, L"");
	*ppwszOptionalStatusText = nullptr;
	*pcpsiOptionalStatusIcon = CREDENTIAL_PROVIDER_STATUS_ICON::CPSI_SUCCESS;
	return S_OK;
}
