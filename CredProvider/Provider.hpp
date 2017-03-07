#pragma once

#include <Windows.h>
#include <credentialprovider.h>
#include "Credential.hpp"

struct FIELD_STATE_PAIR
{
	CREDENTIAL_PROVIDER_FIELD_STATE cpfs;
	CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE cpfis;
};
typedef struct FIELD_STATE_PAIR FIELD_STATE_PAIR;

class CCredentialProvider : public ICredentialProvider
{
private:
	ULONG instances;
public:
	static const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR _fields[3];
	static const FIELD_STATE_PAIR field_states[3];
private:
	CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR * fields;
	CCredentialProviderCredential * credential;
	CREDENTIAL_PROVIDER_USAGE_SCENARIO usage_scenario;
	bool valid_scenario;
	bool renew_credentials;
public:
	CCredentialProvider();
private:
	~CCredentialProvider();
public:
	// IUnknown
	virtual HRESULT QueryInterface(REFIID, void **);
	virtual ULONG AddRef();
	virtual ULONG Release();
	// ICredentialProvider
	virtual HRESULT SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO, DWORD);
	virtual HRESULT SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *);
	virtual HRESULT Advise(ICredentialProviderEvents *, UINT_PTR);
	virtual HRESULT UnAdvise();
	virtual HRESULT GetFieldDescriptorCount(DWORD *);
	virtual HRESULT GetFieldDescriptorAt(DWORD, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **);
	virtual HRESULT GetCredentialCount(DWORD *, DWORD *, BOOL *);
	virtual HRESULT GetCredentialAt(DWORD, ICredentialProviderCredential **);
};
