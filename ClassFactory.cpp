#include "ClassFactory.hpp"
#include <Credentialprovider.h>
#include <Shlwapi.h>
#include "Util.hpp"
#include "Provider.hpp"
#include "Filter.hpp"

CClassFactory::CClassFactory()
{
	this->instances = 0UL;
	this->AddRef();
}

CClassFactory::~CClassFactory()
{
}

HRESULT CClassFactory::QueryInterface(REFIID riid, void ** ppvObject)
{
#pragma warning(push)
#pragma warning(disable:4838)
	static const QITAB interfaces[] = {
		QITABENT(CClassFactory, IClassFactory),
		{}
	};
#pragma warning(pop)
	return QISearch(this, interfaces, riid, ppvObject);
}

ULONG CClassFactory::AddRef()
{
	global_instances++;
	return ++(this->instances);
}

ULONG CClassFactory::Release()
{
	global_instances--;
	auto new_count = --(this->instances);
	if(new_count == 0)
	{
		delete this;
	}
	return new_count;
}

extern "C" void guid_to_string(const GUID &, wchar_t *);

HRESULT CClassFactory::CreateInstance(IUnknown * pUnkOuter, REFIID riid, void ** ppvObject)
{
	// 集約？知らんなァ～
	if(pUnkOuter)
	{
		return CLASS_E_NOAGGREGATION;
	}

	HRESULT result;

	if(riid == __uuidof(IClassFactory))
	{
		this->AddRef();
		*ppvObject = static_cast<IClassFactory*>(this);
		return S_OK;
	}

	if(riid == __uuidof(ICredentialProvider))
	{
		ICredentialProvider * cp = new CCredentialProvider();
		result = cp->QueryInterface(riid, ppvObject);
		cp->Release();
		return result;
	}

	if(riid == __uuidof(ICredentialProviderFilter))
	{
		ICredentialProviderFilter * cp = new CCredentialProviderFilter();
		result = cp->QueryInterface(riid, ppvObject);
		cp->Release();
		return result;
	}

	*ppvObject = nullptr;
	return E_NOINTERFACE;
}

HRESULT CClassFactory::LockServer(BOOL fLock)
{
	return S_OK;
}
