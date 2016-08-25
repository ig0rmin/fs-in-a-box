#ifndef __FSBOX_DIRINTF_H__
#define __FSBOX_DIRINTF_H__

#include "BlockTypes.h"

#include <boost/noncopyable.hpp>

#include <string>
#include <functional>
#include <memory>

namespace FsBox
{

class Container;
class DirIntfImpl;

class DirIntf : public boost::noncopyable
{
public:
	DirIntf(Container& container);
	virtual ~DirIntf();

	BlockHandle GetRoot();
	// Enumerate
	using  EnumDirEntriesFn = bool(BlockTypes::FileType type, const std::string& name);
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
	std::unique_ptr<DirIntfImpl> _impl;
};

}//namespace FsBox
#endif