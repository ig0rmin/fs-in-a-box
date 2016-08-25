#include "Directory.h"

#include "InprocSrv.h"
#include "BSTRUtils.h"
#include "File.h"

Directory::Directory(std::shared_ptr<FsBox::ContainerIntf> containerIntf, IDirectory* parent, FsBox::BlockHandle dir)
:_containerIntf(containerIntf),
_parent(parent),
_dir(dir),
_refCount(1)
{
	if (_parent)
	{
		_parent->AddRef();
	}
	IncrementObjCount();
}

Directory::~Directory()
{
	_containerIntf->CloseDir(_dir);
	if (_parent)
	{
		_parent->Release();
	}
	DecrementObjCount();
}

HRESULT __stdcall Directory::QueryInterface(const IID& iid, void** ppv)
{
	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IUnknown*>(this);
	}
	else if (iid == IID_FsBox_Directory)
	{
		*ppv = static_cast<IDirectory*>(this);
	}
	else
	{
		*ppv = nullptr;
		return E_NOINTERFACE;
	}
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall Directory::AddRef()
{
	return InterlockedIncrement(&_refCount);
}

ULONG __stdcall Directory::Release()
{
	if (!InterlockedDecrement(&_refCount))
	{
		delete this;
		return 0;
	}
	return _refCount;
}

HRESULT Directory::GetParent(IDirectory** parent)
{
	if (_parent)
	{
		_parent->AddRef();
		*parent = _parent;
	}
	else
	{
		*parent = nullptr;
	}
	return S_OK;
}

HRESULT Directory::OpenDirectory(BSTR name, IDirectory** outDir)
{
	std::string sName = BSTRToString(name);
	FsBox::BlockHandle dirHandle = _containerIntf->OpenDir(_dir, sName);
	if (!dirHandle)
	{
		return E_FAIL;
	}
	Directory* directory = new(std::nothrow) Directory(_containerIntf, this, dirHandle);
	if (!directory)
	{
		_containerIntf->CloseDir(_dir);
		return E_OUTOFMEMORY;
	}
	*outDir = static_cast<IDirectory*>(directory);
	return S_OK;
}

HRESULT Directory::CreateDirectory(BSTR name, IDirectory** outDir)
{
	std::string sName = BSTRToString(name);
	FsBox::BlockHandle dirHandle = _containerIntf->CreateDir(_dir, sName);
	if (!dirHandle)
	{
		return E_FAIL;
	}
	Directory* directory = new(std::nothrow) Directory(_containerIntf, this, dirHandle);
	if (!directory)
	{
		_containerIntf->CloseDir(_dir);
		return E_OUTOFMEMORY;
	}
	*outDir = static_cast<IDirectory*>(directory);
	return S_OK;
}

HRESULT Directory::OpenFile(BSTR name, IFile** outFile)
{
	std::string sName = BSTRToString(name);
	FsBox::BlockHandle fileHandle = _containerIntf->OpenFile(_dir, sName);
	if (!fileHandle)
	{
		return E_FAIL;
	}
	File* file = new(std::nothrow) File(_containerIntf, this, fileHandle);
	if (!file)
	{
		_containerIntf->CloseFile(fileHandle);
		return E_OUTOFMEMORY;
	}
	*outFile = static_cast<IFile*>(file);
	return S_OK;
}

HRESULT Directory::CreateFile(BSTR name, IFile** outFile)
{
	std::string sName = BSTRToString(name);
	FsBox::BlockHandle fileHandle = _containerIntf->CreateFile(_dir, sName);
	if (!fileHandle)
	{
		return E_FAIL;
	}
	File* file = new(std::nothrow) File(_containerIntf, this, fileHandle);
	if (!file)
	{
		_containerIntf->CloseFile(fileHandle);
		return E_OUTOFMEMORY;
	}
	*outFile = static_cast<IFile*>(file);
	return S_OK;
}

HRESULT Directory::DeleteFile(BSTR name)
{
	return _containerIntf->DeleteFile(_dir, BSTRToString(name)) ? S_OK : E_FAIL;
}

HRESULT Directory::DeleteDirectory(BSTR name)
{
	return _containerIntf->DeleteDir(_dir, BSTRToString(name)) ? S_OK : E_FAIL;
}

HRESULT Directory::Enumerate(IDirectoryEnumerator** outDirEnum)
{
	if (!outDirEnum)
	{
		return E_INVALIDARG;
	}
	DirectoryEnumerator* dirEnum = new(std::nothrow) DirectoryEnumerator(_containerIntf, _dir);
	if (!dirEnum)
	{
		return E_OUTOFMEMORY;
	}
	dirEnum->FetchResults();
	*outDirEnum = static_cast<IDirectoryEnumerator*>(dirEnum);
	return S_OK;
}

DirectoryEnumerator::DirectoryEnumerator(std::shared_ptr<FsBox::ContainerIntf> containerIntf, FsBox::BlockHandle dir)
:_containerIntf(containerIntf),
_dir(dir),
_pos(0),
_refCount(1)
{
	IncrementObjCount();
}

DirectoryEnumerator::~DirectoryEnumerator()
{
	DecrementObjCount();
}

void DirectoryEnumerator::FetchResults()
{
	decltype(_cachedItems) cachedItems;
	auto enumCallback = [&cachedItems](FsBox::BlockTypes::FileType type, const std::string& name)
	{
		if (type == FsBox::BlockTypes::FileType::Dir)
		{
			cachedItems.push_back(std::make_pair(name, IDirectoryEnumerator::Directory));
		}
		else if (type == FsBox::BlockTypes::FileType::File)
		{
			cachedItems.push_back(std::make_pair(name, IDirectoryEnumerator::File));
		}
		return true;
	};
	_containerIntf->EnumerateDir(_dir, enumCallback);
	std::swap(cachedItems, _cachedItems);
}

HRESULT DirectoryEnumerator::QueryInterface(const IID& iid, void** ppv)
{
	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IUnknown*>(this);
	}
	else if (iid == IID_FsBox_DirectoryEnumerator)
	{
		*ppv = static_cast<IDirectoryEnumerator*>(this);
	}
	else
	{
		*ppv = nullptr;
		return E_NOINTERFACE;
	}
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG DirectoryEnumerator::AddRef()
{
	return InterlockedIncrement(&_refCount);
}

ULONG DirectoryEnumerator::Release()
{
	if (!InterlockedDecrement(&_refCount))
	{
		delete this;
		return 0;
	}
	return _refCount;
}

HRESULT DirectoryEnumerator::Next(BSTR* outName, DirectoryItemType* outType)
{
	if (_pos >= _cachedItems.size())
	{
		return E_FAIL;
	}
	if (outName)
	{
		std::string& sName = _cachedItems[_pos].first;
		*outName = StringToBSTR(sName);
	}
	if (outType)
	{
		DirectoryItemType type = _cachedItems[_pos].second;
		*outType = type;
	}
	return S_OK;
}

