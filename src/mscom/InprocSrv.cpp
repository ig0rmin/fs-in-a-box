#include "InprocSrv.h"
#include "Fs.h"
#include "FsClassFactory.h"

#include <new>

static long gObjCount = 0;
static long gServerLocks = 0;

void IncrementObjCount()
{
	InterlockedIncrement(&gObjCount);
}

void DecrementObjCount()
{
	InterlockedDecrement(&gObjCount);
}

void IncrementServerLocks()
{
	InterlockedIncrement(&gServerLocks);
}

void DecrementServerLocks()
{
	InterlockedDecrement(&gServerLocks);
}

STDAPI DllCanUnloadNow()
{
	if (!gObjCount && !gServerLocks)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv)
{
	if (clsid != CLSID_FsBox)
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}
	FsClassFactory* fsClassFactory = new(std::nothrow) FsClassFactory();
	if (!fsClassFactory)
	{
		return E_OUTOFMEMORY;
	}
	HRESULT hr = fsClassFactory->QueryInterface(iid, ppv);
	fsClassFactory->Release();
	return hr;
}

STDAPI DllRegisterServer()
{
	//TODO:
	return E_NOTIMPL;
}

STDAPI DllUnregisterServer()
{
	//TODO:
	return E_NOTIMPL;
}