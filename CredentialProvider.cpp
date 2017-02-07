#include "ClassDef.hpp"

CCredentialProvider::CCredentialProvider()
{
	this->instances = 0UL;
	this->AddRef();
}

CCredentialProvider::~CCredentialProvider()
{
}

HRESULT CCredentialProvider::QueryInterface(REFIID riid, void ** ppvObject)
{
	if(riid == __uuidof(IUnknown))
	{
		this->AddRef();
		*ppvObject = this;
	}
	else if(riid == __uuidof(ICredentialProvider))
	{
		this->AddRef();
		*ppvObject = this;
	}
	else
	{
		*ppvObject = nullptr;
	}
	return *ppvObject ? S_OK : E_NOINTERFACE;
}

ULONG CCredentialProvider::AddRef()
{
	_InterlockedIncrement(&global_instances);
	return _InterlockedIncrement(&this->instances);
}

ULONG CCredentialProvider::Release()
{
	auto decr = _InterlockedDecrement(&this->instances);
	if(decr == 0UL)
	{
		delete this;
	}
	_InterlockedDecrement(&global_instances);
	return decr;
}

HRESULT CCredentialProvider::SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO, DWORD)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProvider::SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *)
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
	*pdwCount = 0UL;
	return S_OK;
}

HRESULT CCredentialProvider::GetFieldDescriptorAt(DWORD, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProvider::GetCredentialCount(DWORD *, DWORD *, BOOL *)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProvider::GetCredentialAt(DWORD, ICredentialProviderCredential **)
{
	return E_NOTIMPL;
}
