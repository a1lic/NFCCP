#pragma once

#include <Windows.h>
#include <credentialprovider.h>

class CCredentialProviderCredential : public ICredentialProviderCredential
{
private:
	ULONG instances;
	ICredentialProviderCredentialEvents * events;
	HWND parent;
	IQueryContinueWithStatus * query_continue;
	HANDLE query_continue_thread;
	bool kill_thread;
public:
	CCredentialProviderCredential();
private:
	~CCredentialProviderCredential();
public:
	// IUnknown
	virtual HRESULT QueryInterface(REFIID, void **);
	virtual ULONG AddRef();
	virtual ULONG Release();
	// ICredentialProviderCredential
	virtual HRESULT Advise(ICredentialProviderCredentialEvents *);
	virtual HRESULT UnAdvise();
	virtual HRESULT SetSelected(BOOL *);
	virtual HRESULT SetDeselected();
	virtual HRESULT GetFieldState(DWORD, CREDENTIAL_PROVIDER_FIELD_STATE *, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *);
	virtual HRESULT GetStringValue(DWORD, LPWSTR *);
	virtual HRESULT GetBitmapValue(DWORD, HBITMAP *);
	virtual HRESULT GetCheckboxValue(DWORD, BOOL *, LPWSTR *);
	virtual HRESULT GetSubmitButtonValue(DWORD, DWORD *);
	virtual HRESULT GetComboBoxValueCount(DWORD, DWORD *, DWORD *);
	virtual HRESULT GetComboBoxValueAt(DWORD, DWORD, LPWSTR *);
	virtual HRESULT SetStringValue(DWORD, LPCWSTR);
	virtual HRESULT SetCheckboxValue(DWORD, BOOL);
	virtual HRESULT SetComboBoxSelectedValue(DWORD, DWORD);
	virtual HRESULT CommandLinkClicked(DWORD);
	virtual HRESULT GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *, LPWSTR *, CREDENTIAL_PROVIDER_STATUS_ICON *);
	virtual HRESULT ReportResult(NTSTATUS, NTSTATUS, LPWSTR *, CREDENTIAL_PROVIDER_STATUS_ICON *);
};
