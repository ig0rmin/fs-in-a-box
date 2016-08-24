#include "ContainerIntf.h"

namespace FsBox
{

ContainerIntf::ContainerIntf(Container& container)
:_mutex(container.GetLock()),
_fileIntf(container),
_dirIntf(container)
{
}

BlockHandle ContainerIntf::GetRoot()
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	BlockHandle root = _dirIntf.GetRoot();
	if (root)
	{
		++_openDirMap[root];
	}
	return root;
}

void ContainerIntf::EnumerateDir(BlockHandle dir, std::function<EnumDirEntriesFn> fn)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _dirIntf.EnumerateDir(dir, fn);
}

BlockTypes::FileType ContainerIntf::GetDirEntryType(BlockHandle dir, const std::string& name)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _dirIntf.GetDirEntryType(dir, name);
}

bool ContainerIntf::IsDirEmpty(BlockHandle dir)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _dirIntf.IsEmpty(dir);
}

BlockHandle ContainerIntf::OpenDir(BlockHandle parent, const std::string& name)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	BlockHandle dir = _dirIntf.OpenDir(parent, name);
	if (dir)
	{
		++_openDirMap[dir];
	}
	return dir;
}

BlockHandle ContainerIntf::CreateDir(BlockHandle parent, const std::string& name)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	BlockHandle dir = _dirIntf.CreateDir(parent, name);
	if (dir)
	{
		++_openDirMap[dir];
	}
	return dir;
}

bool ContainerIntf::DeleteDir(BlockHandle parent, const std::string& name)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	BlockHandle dir = _dirIntf.OpenDir(parent, name);
	if (!dir || _openDirMap[dir])
	{
		return false;
	}
	return _dirIntf.DeleteDir(parent, name);
}

void ContainerIntf::CloseDir(BlockHandle dir)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	if (_openDirMap[dir])
	{
		--_openDirMap[dir];
	}
}

BlockHandle ContainerIntf::OpenFile(BlockHandle dir, const std::string & name)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	BlockHandle file = _dirIntf.OpenFile(dir, name);
	if (file)
	{
		++_openFileMap[file];
	}
	return file;
}

BlockHandle ContainerIntf::CreateFile(BlockHandle dir, const std::string& name)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	BlockHandle file = _dirIntf.CreateFile(dir, name);
	if (file)
	{
		++_openFileMap[file];
	}
	return file;
}

bool ContainerIntf::DeleteFile(BlockHandle dir, const std::string& name)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	BlockHandle file = _dirIntf.OpenFile(dir, name);
	if (!file || _openFileMap[file])
	{
		return false;
	}
	return _dirIntf.DeleteFile(dir, name);
}

void ContainerIntf::CloseFile(BlockHandle file)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	if (_openFileMap[file])
	{
		--_openFileMap[file];
	}
}

stream_offset ContainerIntf::GetFileSize(BlockHandle file)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _fileIntf.GetSize(file);
}

bool ContainerIntf::IsFileEmpty(BlockHandle file)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _fileIntf.IsEmpty(file);
}

size_t ContainerIntf::ReadFile(BlockHandle file, char* buff, size_t buffSize, stream_offset offset)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _fileIntf.Read(file, buff, buffSize, offset);
}

bool ContainerIntf::WriteFile(BlockHandle file, const char* buff, size_t buffSize, stream_offset offset)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _fileIntf.Write(file, buff, buffSize, offset);
}

bool ContainerIntf::AppendFile(BlockHandle file, const char* buff, size_t buffSize)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _fileIntf.Append(file, buff, buffSize);
}

bool ContainerIntf::TruncateFile(BlockHandle file, stream_offset newSize)
{
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _fileIntf.Truncate(file, newSize);
}

}//namespace FsBox