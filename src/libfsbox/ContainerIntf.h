#ifndef __FSBOX_CONTAINER_INTF_H__
#define __FSBOX_CONTAINER_INTF_H__

#include "Types.h"
#include "Container.h"
#include "FileIntf.h"
#include "DirIntf.h"

#include <unordered_map>


namespace FsBox
{

class ContainerIntf : public boost::noncopyable
{
public:
	ContainerIntf(Container& container);

	//DirIntf
	BlockHandle GetRoot();
	// Enumerate
	using  EnumDirEntriesFn = bool(BlockTypes::FileType type, const std::string& name);
	void EnumerateDir(BlockHandle dir, std::function<EnumDirEntriesFn> fn);
	// Get info
	BlockTypes::FileType GetDirEntryType(BlockHandle dir, const std::string& name);
	bool IsDirEmpty(BlockHandle dir);
	// Dir Open + Create + Delete
	BlockHandle OpenDir(BlockHandle parent, const std::string& name);
	BlockHandle CreateDir(BlockHandle parent, const std::string& name);
	bool DeleteDir(BlockHandle parent, const std::string& name);
	void CloseDir(BlockHandle dir);
	// File Open + Create + Delete
	BlockHandle OpenFile(BlockHandle dir, const std::string& name);
	BlockHandle CreateFile(BlockHandle dir, const std::string& name);
	bool DeleteFile(BlockHandle parent, const std::string& name);
	void CloseFile(BlockHandle file);

	//FileIntf
	stream_offset GetFileSize(BlockHandle file);
	bool IsFileEmpty(BlockHandle file);
	size_t ReadFile(BlockHandle file, char* buff, size_t buffSize, stream_offset offset);
	bool WriteFile(BlockHandle file, const char* buff, size_t buffSize, stream_offset offset);
	bool AppendFile(BlockHandle file, const char* buff, size_t buffSize);
	bool TruncateFile(BlockHandle file, stream_offset newSize);
private:
	std::recursive_mutex& _mutex;
	FileIntf _fileIntf;
	DirIntf _dirIntf;
	std::unordered_map<BlockHandle, size_t> _openFileMap;
	std::unordered_map<BlockHandle, size_t> _openDirMap;
};

} //namespace FsBox
#endif