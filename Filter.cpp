#include "Filter.hpp"
#include "Util.hpp"

CCredentialProviderFilter::CCredentialProviderFilter()
{
	this->instances = 0UL;
	this->AddRef();
}

CCredentialProviderFilter::~CCredentialProviderFilter()
{
}

HRESULT CCredentialProviderFilter::QueryInterface(REFIID riid, void ** ppvObject)
{
	if(riid == __uuidof(IUnknown))
	{
		this->AddRef();
		*ppvObject = this;
	}
	else if(riid == __uuidof(ICredentialProviderFilter))
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

ULONG CCredentialProviderFilter::AddRef()
{
	_InterlockedIncrement(&global_instances);
	return _InterlockedIncrement(&this->instances);
}

ULONG CCredentialProviderFilter::Release()
{
	auto decr = _InterlockedDecrement(&this->instances);
	if(decr == 0UL)
	{
		delete this;
	}
	_InterlockedDecrement(&global_instances);
	return decr;
}

HRESULT CCredentialProviderFilter::Filter(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags, GUID * rgclsidProviders, BOOL * rgbAllow, DWORD cProviders)
{
	return E_NOTIMPL;
}

HRESULT CCredentialProviderFilter::UpdateRemoteCredential(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcsIn, CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION * pcpcsOut)
{
	return E_NOTIMPL;
}
