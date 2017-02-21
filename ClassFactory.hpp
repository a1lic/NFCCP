#pragma once

#include <Windows.h>
#include <Unknwnbase.h>

class __declspec(uuid("D2BB2AE9-8214-4499-BC3E-D2EAB632EC22")) CClassFactory : public IClassFactory
{
private:
	ULONG instances;
public:
	CClassFactory();
private:
	~CClassFactory();
public:
	// IUnknown
	virtual HRESULT QueryInterface(REFIID, void **);
	virtual ULONG AddRef();
	virtual ULONG Release();
	// IClassFactory
	virtual HRESULT CreateInstance(IUnknown *, REFIID, void **);
	virtual HRESULT LockServer(BOOL);
};
