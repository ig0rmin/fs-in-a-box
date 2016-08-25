#include "FsComponent.h"
#include "Directory.h"
#include "InprocSrv.h"
#include "BSTRUtils.h"

#include <libfsbox/Container.h>
#include <libfsbox/ContainerIntf.h>

#include <memory>

FsComponent::FsComponent()
:_refCount(1)
{
	IncrementObjCount();
}

FsComponent::~FsComponent()
{
	DecrementObjCount();
}

HRESULT FsComponent::QueryInterface(const IID& iid, void** ppv)
{
	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IUnknown*>(this);
	}
	else if (iid == IID_FsBox_Fs)
	{
		*ppv = static_cast<IFs*>(this);
	}
	else
	{
		*ppv = nullptr;
		return E_NOINTERFACE;
	}
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG FsComponent::AddRef()
{
	return InterlockedIncrement(&_refCount);
}

ULONG FsComponent::Release()
{
	if (!InterlockedDecrement(&_refCount))
	{
		delete this;
		return 0;
	}
	return _refCount;
}

HRESULT FsComponent::Open(BSTR fileName, IDirectory** root)
{
	if (!fileName || !root)
	{
		return E_INVALIDARG;
	}
	std::shared_ptr<FsBox::Container> container(new(std::nothrow) FsBox::Container);
	if (!container)
	{
		return E_OUTOFMEMORY;
	}
	if (!container->Open(BSTRToString(fileName)))
	{
		return E_FAIL;
	}
	std::shared_ptr<FsBox::ContainerIntf> containerIntf(new(std::nothrow) FsBox::ContainerIntf(container));
	if (!containerIntf)
	{
		return E_OUTOFMEMORY;
	}
	FsBox::BlockHandle rootDir = containerIntf->GetRoot();
	if (!rootDir)
	{
		return E_FAIL;
	}
	Directory* directory = new(std::nothrow) Directory(containerIntf, nullptr, rootDir);
	if (!directory)
	{
		return E_OUTOFMEMORY;
	}
	directory->AddRef();
	*root = directory;
	return S_OK;
}
