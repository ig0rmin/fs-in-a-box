#include "File.h"

#include "InprocSrv.h"
#include "BSTRUtils.h"

File::File(std::shared_ptr<FsBox::ContainerIntf> containerIntf, IDirectory* parent, FsBox::BlockHandle file)
:_containerIntf(containerIntf),
_parent(parent),
_file(file),
_pos(0),
_refCount(1)
{
	if (_parent)
	{
		_parent->AddRef();
	}
	IncrementObjCount();
}

File::~File()
{
	_containerIntf->CloseFile(_file);
	if (_parent)
	{
		_parent->Release();
	}
	DecrementObjCount();
}

HRESULT File::QueryInterface(const IID& iid, void** ppv)
{
	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IUnknown*>(this);
	}
	else if (iid == IID_FsBox_File)
	{
		*ppv = static_cast<IFile*>(this);
	}
	else
	{
		*ppv = nullptr;
		return E_NOINTERFACE;
	}
	reinterpret_cast<IUnknown*>(*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall File::AddRef()
{
	return InterlockedIncrement(&_refCount);
}

ULONG __stdcall File::Release()
{
	if (!InterlockedDecrement(&_refCount))
	{
		delete this;
		return 0;
	}
	return _refCount;
}

HRESULT File::Read(BYTE* buff, DWORD buffSize, DWORD* outRead)
{
	size_t read = _containerIntf->ReadFile(_file, reinterpret_cast<char*>(buff), static_cast<DWORD>(buffSize), _pos);
	_pos += read;
	if (outRead)
	{
		*outRead = read;
	}
	return S_OK;
}

HRESULT File::Write(BYTE* buff, DWORD buffSize)
{
	if (_containerIntf->WriteFile(_file, reinterpret_cast<char*>(buff), static_cast<DWORD>(buffSize), _pos))
	{
		_pos += buffSize;
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT File::Seek(long long pos)
{
	_pos = pos;
	return S_OK;
}

HRESULT File::Tell(long long pos)
{
	_pos = pos;
	return S_OK;
}

HRESULT File::GetSize(long long* size)
{
	*size = _containerIntf->GetFileSize(_file);
	return S_OK;
}

HRESULT File::Truncate(long long newSize)
{
	if (_containerIntf->TruncateFile(_file, newSize))
	{
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}


