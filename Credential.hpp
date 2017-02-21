#pragma once

#include <Windows.h>
#include <credentialprovider.h>

class CCredentialProviderCredential : public ICredentialProviderCredential
{
private:
	ULONG instances;
	ICredentialProviderCredentialEvents * events;
	HWND parent;
public:
	CCredentialProviderCredential();
private:
	~CCredentialProviderCredential();
public:
	// IUnknown
	HRESULT QueryInterface(REFIID, void **);
	ULONG AddRef();
	ULONG Release();
	// ICredentialProviderCredential
	HRESULT Advise(ICredentialProviderCredentialEvents *);
	HRESULT UnAdvise();
	HRESULT SetSelected(BOOL *);
	HRESULT SetDeselected();
	HRESULT GetFieldState(DWORD, CREDENTIAL_PROVIDER_FIELD_STATE *, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *);
	HRESULT GetStringValue(DWORD, LPWSTR *);
	HRESULT GetBitmapValue(DWORD, HBITMAP *);
	HRESULT GetCheckboxValue(DWORD, BOOL *, LPWSTR *);
	HRESULT GetSubmitButtonValue(DWORD, DWORD *);
	HRESULT GetComboBoxValueCount(DWORD, DWORD *, DWORD *);
	HRESULT GetComboBoxValueAt(DWORD, DWORD, LPWSTR *);
	HRESULT SetStringValue(DWORD, LPCWSTR);
	HRESULT SetCheckboxValue(DWORD, BOOL);
	HRESULT SetComboBoxSelectedValue(DWORD, DWORD);
	HRESULT CommandLinkClicked(DWORD);
	HRESULT GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *, LPWSTR *, CREDENTIAL_PROVIDER_STATUS_ICON *);
	HRESULT ReportResult(NTSTATUS, NTSTATUS, LPWSTR *, CREDENTIAL_PROVIDER_STATUS_ICON *);
};
