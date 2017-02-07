#include "ClassDef.hpp"

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
	if(riid == __uuidof(IUnknown))
	{
		this->AddRef();
		*ppvObject = this;
	}
	else if(riid == __uuidof(IClassFactory))
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

ULONG CClassFactory::AddRef()
{
	_InterlockedIncrement(&global_instances);
	return _InterlockedIncrement(&this->instances);
}

ULONG CClassFactory::Release()
{
	auto decr = _InterlockedDecrement(&this->instances);
	if(decr == 0UL)
	{
		delete this;
	}
	_InterlockedDecrement(&global_instances);
	return decr;
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
		*ppvObject = this;
		return S_OK;
	}

	if(riid == __uuidof(ICredentialProvider))
	{
		ICredentialProvider * cp = new CCredentialProvider();
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
