#define OEMRESOURCE
#include "Credential.hpp"
#include <Shlwapi.h>
#include <NTSecAPI.h>
#include <sspi.h>
#include <process.h>
#include "../Common/Util.hpp"
#include "Provider.hpp"
#include "ClassFactory.hpp"

const unsigned char CCredentialProviderCredential::card_id_null[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

CCredentialProviderCredential::CCredentialProviderCredential()
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "CCredentialProviderCredential");
	this->instances = 0;
	this->AddRef();
	this->events = nullptr;
	this->parent = nullptr;
	this->query_continue = nullptr;

	memcpy(this->card_id, CCredentialProviderCredential::card_id_null, 8);

	this->scard = new SmartCardHelper();

	this->scard->RegisterConnectionHandler([](ConnectionInfo * ci) {
		auto _this = static_cast<CCredentialProviderCredential*>(ci->Param);
		auto e = _this->GetEvent();
		if(e)
		{
			auto id = ci->Card->GetID();
			ci->Card->PutIDToBinary(_this->card_id);
			e->SetFieldString(_this, 2, id.c_str());
			e->Release();
		}
	}, this);
	this->scard->RegisterDisconnectionHandler([](ConnectionInfo * ci) {
		auto _this = static_cast<CCredentialProviderCredential*>(ci->Param);
		auto e = _this->GetEvent();
		if(e)
		{
			e->SetFieldString(_this, 2, CCredentialProvider::_fields[2].pszLabel);
			e->Release();
		}
		memcpy(_this->card_id, CCredentialProviderCredential::card_id_null, 8);
	}, this);

	this->scard->WatchAll();
}

CCredentialProviderCredential::~CCredentialProviderCredential()
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "~CCredentialProviderCredential");
	this->scard->UnwatchAll();
	delete this->scard;
	
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
	global_instances++;
	return ++(this->instances);
}

ULONG CCredentialProviderCredential::Release()
{
	global_instances--;
	auto new_count = --(this->instances);
	if(new_count == 0)
	{
		delete this;
	}
	return new_count;
}

HRESULT CCredentialProviderCredential::Advise(ICredentialProviderCredentialEvents * pcpce)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "Advise");
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
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "UnAdvise");
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
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "SetSelected");
	*pbAutoLogon = TRUE;
	return S_OK;
}

HRESULT CCredentialProviderCredential::SetDeselected()
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "SetDeselected");
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetFieldState(DWORD dwFieldID, CREDENTIAL_PROVIDER_FIELD_STATE * pcpfs, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE * pcpfis)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "GetFieldState");
	*pcpfs = CCredentialProvider::field_states[dwFieldID].cpfs;
	*pcpfis = CCredentialProvider::field_states[dwFieldID].cpfis;
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetStringValue(DWORD dwFieldID, LPWSTR * ppwsz)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "GetStringValue");
	switch(dwFieldID)
	{
	case 1:
	case 2:
		SHStrDupW(CCredentialProvider::_fields[dwFieldID].pszLabel, ppwsz);
		break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetBitmapValue(DWORD dwFieldID, HBITMAP * phbmp)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "GetBitmapValue");
	if(dwFieldID != 0)
	{
		return E_INVALIDARG;
	}
	*phbmp = reinterpret_cast<HBITMAP>(LoadImageW(nullptr, MAKEINTRESOURCEW(OBM_CLOSE), IMAGE_BITMAP, 0, 0, LR_SHARED));
	return S_OK;
}

HRESULT CCredentialProviderCredential::GetCheckboxValue(DWORD, BOOL *, LPWSTR *)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderCredential::GetSubmitButtonValue(DWORD, DWORD *)
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

HRESULT CCredentialProviderCredential::GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE * pcpgsr, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcs, LPWSTR * ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON * pcpsiOptionalStatusIcon)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "GetSerialization");
	*pcpgsr = CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE::CPGSR_RETURN_NO_CREDENTIAL_FINISHED;
	//SHStrDupW(L"カードを認識できませんでした。", ppwszOptionalStatusText);
	//*pcpsiOptionalStatusIcon = CREDENTIAL_PROVIDER_STATUS_ICON::CPSI_ERROR;

	if(!memcmp(this->card_id, CCredentialProviderCredential::card_id_null, 8))
	{
		return S_OK;
	}

	/*
	 * カードのシリアル番号で認証を行うには、こちらで作成した認証パッケージを呼び出す必要があるので
	 * そのパッケージ名をLsaLookupAuthenticationPackage関数に渡してパッケージIDを調べ
	 * これをLsaLogonUser関数に渡す必要がある
	 * そこで、pcpcs->ulAuthenticationPackageには、LsaLogonUser関数に渡されるパッケージIDを指定する
	 * あとはrgbSerializationにシリアル番号を格納し、cbSerializationにシリアル番号のサイズを格納
	 * clsidCredentialProviderはとりあえずClassFactoryのCLSID
	 *
	 * 以上が終わったら
	 * *pcpgsr = CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE::CPGSR_RETURN_CREDENTIAL_FINISHED;
	 * これでGetSerializationメソッドから戻れば、LsaLogonUser関数に渡っていく
	 */

#if false
	NTSTATUS st;

	HANDLE lsa;
	st = LsaConnectUntrusted(&lsa);
	if(st != 0)
	{
		DebugPrint(L"%hs fail (%08X)", "LsaConnectUntrusted", st);
		return E_FAIL;
	}

	static const LSA_STRING msv1_0_package = {sizeof(MSV1_0_PACKAGE_NAME)-1, sizeof(MSV1_0_PACKAGE_NAME)-1, MSV1_0_PACKAGE_NAME};
	st = LsaLookupAuthenticationPackage(lsa, const_cast<LSA_STRING*>(&msv1_0_package), &pcpcs->ulAuthenticationPackage);
	if(st != 0)
	{
		LsaDeregisterLogonProcess(lsa);
		DebugPrint(L"%hs fail (%08X)", "LsaLookupAuthenticationPackage", st);
		return E_FAIL;
	}

	LsaDeregisterLogonProcess(lsa);

	pcpcs->clsidCredentialProvider = __uuidof(CClassFactory);


	pcpcs->cbSerialization = 8;
	pcpcs->rgbSerialization = static_cast<byte*>(CoTaskMemAlloc(sizeof(SEC_WINNT_AUTH_IDENTITY_EX2)));

	auto nt_auth_id = reinterpret_cast<SEC_WINNT_AUTH_IDENTITY_EX2*>(pcpcs->rgbSerialization);
	nt_auth_id->Version = SEC_WINNT_AUTH_IDENTITY_VERSION_2;


	memcpy(pcpcs->rgbSerialization, this->card_id, 8);

	DebugPrint(L"Serialization done.");
	*pcpgsr = CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE::CPGSR_RETURN_CREDENTIAL_FINISHED;
#endif

	return S_OK;
}

HRESULT CCredentialProviderCredential::ReportResult(NTSTATUS, NTSTATUS, LPWSTR * ppwszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON * pcpsiOptionalStatusIcon)
{
	DebugPrint(L"%hs(%p)::%hs", "CCredentialProviderCredential", this, "ReportResult");
	MessageBoxFmt(this->parent, L"CCredentialProviderCredential::ReportResult", 0, L"");
	*ppwszOptionalStatusText = nullptr;
	*pcpsiOptionalStatusIcon = CREDENTIAL_PROVIDER_STATUS_ICON::CPSI_SUCCESS;
	return S_OK;
}
