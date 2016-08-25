#ifndef __FSBOX_MSCOM_FS_H__
#define __FSBOX_MSCOM_FS_H__

#include <guiddef.h>
#include <unknwn.h>

#undef CreateFile
#undef DeleteFile

// {1E8E9DE4-C66A-442C-868C-39BFD6DFAD00}
DEFINE_GUID(CLSID_FsBox, 
	0x1e8e9de4, 0xc66a, 0x442c, 0x86, 0x8c, 0x39, 0xbf, 0xd6, 0xdf, 0xad, 0x0);

// {9C4C5FB6-C39C-4866-B0FB-E6ED03AC85CC}
DEFINE_GUID(IID_FsBox_Fs,
	0x9c4c5fb6, 0xc39c, 0x4866, 0xb0, 0xfb, 0xe6, 0xed, 0x3, 0xac, 0x85, 0xcc);
// {CBDFD670-C357-404E-A206-5D99521286B2}
DEFINE_GUID(IID_FsBox_Directory,
	0xcbdfd670, 0xc357, 0x404e, 0xa2, 0x6, 0x5d, 0x99, 0x52, 0x12, 0x86, 0xb2);
// {5150DBC3-E7DC-4311-BFBE-A33B0D7E0DA4}
DEFINE_GUID(IID_FsBox_DirectoryEnumerator,
	0x5150dbc3, 0xe7dc, 0x4311, 0xbf, 0xbe, 0xa3, 0x3b, 0xd, 0x7e, 0xd, 0xa4);
// {0B6572C4-3174-4B10-A6F1-504B9E9A5589}
DEFINE_GUID(IID_FsBox_File,
	0xb6572c4, 0x3174, 0x4b10, 0xa6, 0xf1, 0x50, 0x4b, 0x9e, 0x9a, 0x55, 0x89);


interface IFile;
interface IDirectory;

interface IFs : IUnknown
{
	virtual HRESULT __stdcall Open(BSTR fileName, IDirectory** root) = 0;
};

interface IDirectoryEnumerator : IUnknown
{
	typedef enum {File, Directory} DirectoryItemType;
	virtual HRESULT __stdcall Next(BSTR* name, DirectoryItemType* type) = 0;
};

interface IDirectory : IUnknown
{
	virtual HRESULT __stdcall GetParent(IDirectory** parent) = 0;
	virtual HRESULT __stdcall OpenDirectory(BSTR name, IDirectory** dir) = 0;
	virtual HRESULT __stdcall CreateDirectory(BSTR name, IDirectory** dir) = 0;
	virtual HRESULT __stdcall OpenFile(BSTR name, IFile** file) = 0;
	virtual HRESULT __stdcall CreateFile(BSTR name, IFile** file) = 0;
	virtual HRESULT __stdcall DeleteFile(BSTR name) = 0;
	virtual HRESULT __stdcall DeleteDirectory(BSTR name) = 0;
	virtual HRESULT __stdcall Enumerate(IDirectoryEnumerator** dirEnum) = 0;
};

interface IFile : IUnknown
{
	virtual HRESULT __stdcall Read(BYTE* buff, DWORD buffSize, DWORD* read) = 0;
	virtual HRESULT __stdcall Write(BYTE* buff, DWORD buffSize) = 0;
	virtual HRESULT __stdcall Seek(long long pos) = 0;
	virtual HRESULT __stdcall Tell(long long pos) = 0;
	virtual HRESULT __stdcall GetSize(long long* size) = 0;
	virtual HRESULT __stdcall Truncate(long long newSize) = 0;
};

#endif