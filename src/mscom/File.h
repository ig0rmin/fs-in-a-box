#ifndef __FSBOX_MSCOM_FILE_H__
#define __FSBOX_MSCOM_FILE_H__

#include "Fs.h"

#include <libfsbox/ContainerIntf.h>

#include <memory>

class File : public IFile
{
public:
	File(std::shared_ptr<FsBox::ContainerIntf> containerIntf, IDirectory* parent, FsBox::BlockHandle file);
	~File();

	// IUnknown
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IFile
	HRESULT __stdcall Read(BYTE* buff, DWORD buffSize, DWORD* read);
	HRESULT __stdcall Write(BYTE* buff, DWORD buffSize);
	HRESULT __stdcall Seek(long long pos);
	HRESULT __stdcall Tell(long long pos);
	HRESULT __stdcall GetSize(long long* size);
	HRESULT __stdcall Truncate(long long newSize);
private:
	std::shared_ptr<FsBox::ContainerIntf> _containerIntf;
	IDirectory* _parent;
	FsBox::BlockHandle _file;
	long long _pos;
	long _refCount;
};

#endif