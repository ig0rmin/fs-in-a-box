#ifndef __FSBOX_DIRINTF_H__
#define __FSBOX_DIRINTF_H__

#include "Types.h"
#include "BlockAllocator.h"
#include "BlockReader.hpp"
#include "FileIntf.h"

namespace FsBox
{

class DirIntf
{
public:
	DirIntf(Container& container);

	BlockHandle GetRoot();
	// Enumerate
	using  EnumDirEntriesFn = bool(BlockHandle entry, BlockTypes::DirEntry* pDirEntry, const std::string& name); //OK
	void EnumerateDir(BlockHandle dir, std::function<EnumDirEntriesFn> fn);
	// Get info
	BlockTypes::FileType GetDirEntryType(BlockHandle dir, const std::string& name);
	bool IsEmpty(BlockHandle dir);
	// Dir Open + Create + Delete
	BlockHandle OpenDir(BlockHandle parent, const std::string& name);
	BlockHandle CreateDir(BlockHandle parent, const std::string& name);
	bool DeleteDir(BlockHandle parent, const std::string& name);
	// File Open + Create + Delete
	BlockHandle OpenFile(BlockHandle dir, const std::string& name);
	BlockHandle CreateFile(BlockHandle dir, const std::string& name);
	bool DeleteFile(BlockHandle parent, const std::string& name);

	static size_t GetMaxFileName();
private:
	BlockReader _blockReader;
	BlockAllocator _blockAllocator;
	FileIntf _fileIntf;
private:
	// FileIntf wrappers
	std::string FileToString(BlockHandle file);
	BlockHandle StringToFile(const std::string& str);
	void FreeFile(BlockHandle file);
	// DirEntry helpers
	BlockHandle GetDirEntry(BlockHandle dir, const std::string& name);
	BlockTypes::FileType GetDirEntryType(BlockHandle dirEntry);
	BlockHandle GetDirEntryBody(BlockHandle dirEntry);
	BlockHandle GetDirEntryName(BlockHandle dirEntry);
	BlockHandle GetNextDirEntry(BlockHandle dirEntry);
	BlockHandle GetPrevDirEntry(BlockHandle dirEntry);
	void SetNextDirEntry(BlockHandle dirEntry, BlockHandle next);
	void SetPrevDirEntry(BlockHandle dirEntry, BlockHandle prev);
	// Linked list helpers
	void InsertDirEntry(BlockHandle dir, BlockHandle dirEntry);
	void DeleteDirEntry(BlockHandle dir, BlockHandle dirEntry);
	// Dir structures create/free helpers
	BlockHandle MakeDirHeader(BlockHandle parent);
	void FreeDirHeader(BlockHandle dir);
	BlockHandle MakeDirEntry(BlockTypes::FileType type, BlockHandle name, BlockHandle payload);
	void FreeDirEntry(BlockHandle dirEntry);
};

}//namespace FsBox
#endif