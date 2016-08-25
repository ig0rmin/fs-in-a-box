#include "FsClassFactory.h"

#include "FsComponent.h"
#include "InprocSrv.h"

#include <new>

FsClassFactory::FsClassFactory()
:_refCount(1)
{
	IncrementObjCount();
}

FsClassFactory::~FsClassFactory()
{
	DecrementObjCount();
}

HRESULT FsClassFactory::QueryInterface(const IID& iid, void** ppv)
{
	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IUnknown*>(this);
	}
	else if (iid == IID_IClassFactory)
	{
		*ppv = static_cast<IClassFactory*>(this);
	}
	else
	{
		*ppv = nullptr;
		return E_NOINTERFACE;
	}
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG FsClassFactory::AddRef()
{
	return InterlockedIncrement(&_refCount);
}

ULONG FsClassFactory::Release()
{
	if (!InterlockedDecrement(&_refCount))
	{
		delete this;
		return 0;
	}
	return _refCount;
}

HRESULT FsClassFactory::CreateInstance(IUnknown* pUnkOuter, const IID& iid, void** ppv)
{
	if (pUnkOuter)
	{
		return CLASS_E_NOAGGREGATION;
	}
	FsComponent* fsComponent = new(std::nothrow) FsComponent();
	if (!fsComponent)
	{
		return E_OUTOFMEMORY;
	}
	HRESULT hr = fsComponent->QueryInterface(iid, ppv);
	fsComponent->Release();
	return hr;
}

HRESULT FsClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
	{
		IncrementServerLocks();
	}
	else
	{
		DecrementServerLocks();
	}
	return S_OK;
}

