#ifndef __FSBOX_MSCOM_FS_CLASS_FACTORY_H__
#define __FSBOX_MSCOM_FS_CLASS_FACTORY_H__

#include <Unknwn.h>

class FsClassFactory : public IClassFactory
{
public:
	FsClassFactory();
	~FsClassFactory();

	// IUnknown
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) ;
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IClassFactory
	HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, const IID& iid, void** ppv);
	HRESULT __stdcall LockServer(BOOL fLock);
private:
	long _refCount;
};

#endif