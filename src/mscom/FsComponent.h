#ifndef __FSBOX_MSCOM_FS_COMPONENT_H__
#define __FSBOX_MSCOM_FS_COMPONENT_H__

#include "Fs.h"

class FsComponent : public IFs
{
public:
	FsComponent();
	~FsComponent();

	// IUnknown
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) ;
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IFs
	HRESULT __stdcall Open(BSTR fileName, IDirectory** root);
private:
	long _refCount;
};

#endif