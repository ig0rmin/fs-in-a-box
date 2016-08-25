#ifndef __FSBOX_MSCOM_DIRECTORY_H__
#define __FSBOX_MSCOM_DIRECTORY_H__

#include "Fs.h"

#include <libfsbox/ContainerIntf.h>

#include <memory>

class Directory : public IDirectory
{
public:
	Directory(std::shared_ptr<FsBox::ContainerIntf> containerIntf, IDirectory* parent, FsBox::BlockHandle dir);
	~Directory();

	// IUnknown
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IDirectory
	HRESULT __stdcall GetParent(IDirectory** parent);
	HRESULT __stdcall OpenDirectory(BSTR name, IDirectory** dir);
	HRESULT __stdcall CreateDirectory(BSTR name, IDirectory** dir);
	HRESULT __stdcall OpenFile(BSTR name, IFile** file);
	HRESULT __stdcall CreateFile(BSTR name, IFile** file);
	HRESULT __stdcall DeleteFile(BSTR name);
	HRESULT __stdcall DeleteDirectory(BSTR name);
	HRESULT __stdcall Enumerate(IDirectoryEnumerator** dirEnum);
private:
	std::shared_ptr<FsBox::ContainerIntf> _containerIntf;
	IDirectory* _parent;
	FsBox::BlockHandle _dir;
	long _refCount;
};

class DirectoryEnumerator : public IDirectoryEnumerator
{
public:
	DirectoryEnumerator(std::shared_ptr<FsBox::ContainerIntf> containerIntf, FsBox::BlockHandle dir);
	~DirectoryEnumerator();

	void FetchResults();

	// IUnknown
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	// IDirectoryEnumerator
	HRESULT __stdcall Next(BSTR* name, DirectoryItemType* type);
private:
	std::shared_ptr<FsBox::ContainerIntf> _containerIntf;
	FsBox::BlockHandle _dir;
	std::vector<std::pair<std::string, DirectoryItemType>> _cachedItems;
	size_t _pos;
	long _refCount;
};

#endif