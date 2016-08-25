#include "FileIntf.h"

#include "BlockAllocator.h"
#include "BlockReader.hpp"
#include "Logging.h"

#include <functional>

using namespace FsBox::BlockTypes;

//TODO: This code needs to be reviewed and refactored

namespace FsBox
{

struct FilePos
{
	BlockHandle fileEntry;
	stream_offset offset;;
	size_t memOffset;
};

class FileIntfImpl : public boost::noncopyable
{
public:
	FileIntfImpl(Container & container);

	BlockHandle Create();
	stream_offset GetSize(BlockHandle fileHeader);
	bool IsEmpty(BlockHandle fileHeader);
	size_t Read(BlockHandle fileHeader, char* buff, size_t buffSize, stream_offset offset);
	bool Write(BlockHandle fileHeader, const char* buff, size_t buffSize, stream_offset offset);
	bool Append(BlockHandle fileHeader, const char* buff, size_t buffSize);
	bool Truncate(BlockHandle fileHeader, stream_offset newSize);
	void Delete(BlockHandle fileHeader);
private:
	using EnumFileEntriesFn = bool(BlockHandle fileEntry, BlockTypes::FileEntry* pFileEntry);
	void EnumerateFileEnties(BlockHandle fileEntry, std::function<EnumFileEntriesFn> callback);

	BlockHandle NextFileEntry(BlockHandle fileEntry);
	BlockHandle CreateFileEntry(uint32_t payloadSize);
	size_t WriteToFileEntry(BlockHandle fileEntry, const char* buff, size_t buffSize, size_t offset);
	bool Write(FilePos pos, const char* buff, size_t buffSize);
	bool AppendImpl(FilePos pos, const char* buff, size_t buffSize);
	bool AppendImpl(BlockHandle fileHeader, const char* buff, size_t buffSize);
	bool Seek(BlockHandle fileHeader, FilePos& pos, stream_offset offset);
	void SeekEnd(BlockHandle fileHeader, FilePos& pos);
	size_t Read(FilePos pos, char* buff, size_t buffSize);
	void DeleteFileEntryList(BlockHandle fileEntry);
private:
	BlockAllocator _blockAllocator;
	BlockReader	_blockReader;
};

FileIntfImpl::FileIntfImpl(Container & container):
_blockAllocator(container),
_blockReader(container)
{
}

void FileIntfImpl::EnumerateFileEnties(BlockHandle fileEntry, std::function<EnumFileEntriesFn> callback)
{
	while (fileEntry)
	{
		FileEntry* pFileEntry = _blockReader.Get<FileEntry>(fileEntry);
		if (!pFileEntry)
		{
			LOG_ERROR("%s", "Broken link in file entry list");
			break;
		}
		BlockHandle nextEntry = pFileEntry->next;
		if (!callback(fileEntry, pFileEntry))
		{
			break;
		}
		fileEntry = nextEntry;
	}
}

size_t FileIntfImpl::WriteToFileEntry(BlockHandle fileEntry, const char* buff, size_t buffSize, size_t offset)
{
	FileEntry* pFileEntry = _blockReader.Get<FileEntry>(fileEntry);
	if (!pFileEntry)
	{
		LOG_ERROR("%s", "Invalid file entry handle");
		return 0;
	}
	if (offset > pFileEntry->payloadSize)
	{
		LOG_ERROR("%s", "Offset is outside of entry");
		return 0;
	}
	size_t toWrite = std::min(pFileEntry->payloadSize - offset, buffSize);
	if (toWrite)
	{
		const char* dataBegin = buff;
		const char* dataEnd = dataBegin + toWrite;
		char* pOut = reinterpret_cast<char*>(pFileEntry) + sizeof(FileEntry) + offset;
		std::copy(dataBegin, dataEnd, pOut);
	}
	return toWrite;
}

BlockHandle FileIntfImpl::NextFileEntry(BlockHandle fileEntry)
{
	FileEntry* pFileEntry = _blockReader.Get<FileEntry>(fileEntry);
	if (!pFileEntry)
	{
		LOG_ERROR("%s", "Block handle is not a file entry handle");
		return 0;
	}
	return pFileEntry->next;
}

BlockHandle FileIntfImpl::CreateFileEntry(uint32_t payloadSize)
{
	const uint32_t entrySize = sizeof(FileEntry) + payloadSize;
	BlockHandle block = _blockAllocator.Allocate(entrySize);
	if (!block)
	{
		LOG_ERROR("%s", "Can't allocate new block");
		return 0;
	}
	FileEntry* pFileEntry = _blockReader.CastTo<FileEntry>(block);
	if (!pFileEntry)
	{
		LOG_ERROR("%s", "Allocated block handle is invalid");
		return 0;
	}
	pFileEntry->entrySize = entrySize;
	pFileEntry->payloadSize = payloadSize;
	pFileEntry->next = 0;
	pFileEntry->prev = 0;
	return block;
}

bool FileIntfImpl::AppendImpl(BlockHandle fileHeader, const char* buff, size_t buffSize)
{
	FileHeader* pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid file header");
		return false;
	}
	if (pFileHeader->body)
	{
		LOG_ERROR("%s", "File is not empty");
		return false;
	}
	uint32_t fileEntrySize = std::min(buffSize, FileIntf::GetMaxPayloadSize());
	BlockHandle newFileEntry = CreateFileEntry(fileEntrySize);
	size_t written = WriteToFileEntry(newFileEntry, buff, buffSize, 0);
	// this block is the new body in file header
	pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid file header");
		return false;
	}
	pFileHeader->body = newFileEntry;
	if (written == buffSize)
	{
		return true;
	}
	FilePos pos;
	pos.fileEntry = newFileEntry;
	pos.memOffset = 0;
	pos.offset = 0;
	return AppendImpl(pos, buff + written, buffSize - written);
}

bool FileIntfImpl::AppendImpl(FilePos pos, const char* buff, size_t buffSize)
{
	BlockHandle fileEntry = pos.fileEntry;
	FileEntry* pFileEntry = _blockReader.Get<FileEntry>(fileEntry);
	if (!pFileEntry)
	{
		LOG_ERROR("%s", "Block handle is not a file entry handle");
		return false;
	}
	if (pFileEntry->next)
	{
		LOG_ERROR("%s", "Position passed to AppendImpl is not EOF-like");
		return false;
	}
	while (buffSize)
	{
		BlockHandle newFileEntry = CreateFileEntry(std::min(buffSize, FileIntf::GetMaxPayloadSize()));
		size_t written = WriteToFileEntry(newFileEntry, buff, buffSize, 0);
		buff += written;
		buffSize -= written;
		pFileEntry = _blockReader.Get<FileEntry>(fileEntry);
		if (!pFileEntry)
		{
			LOG_ERROR("%s", "Invalid block");
		}
		pFileEntry->next = newFileEntry;
		fileEntry = newFileEntry;
	}
	return !buffSize;
}

bool FileIntfImpl::Write(FilePos pos, const char* buff, size_t buffSize)
{
	auto writeCallback = [&buff, &buffSize,this](BlockHandle fileEntry, BlockTypes::FileEntry* pFileEntry)
	{
		size_t written = WriteToFileEntry(fileEntry, buff, buffSize, 0);
		buff += written;
		buffSize -= written;
		return buffSize > 0;
	};
	if (pos.memOffset)
	{
		size_t written = WriteToFileEntry(pos.fileEntry, buff, buffSize, pos.memOffset);
		buff += written;
		buffSize -= written;
		BlockHandle nextEntry = NextFileEntry(pos.fileEntry);
		if (nextEntry)
		{
			pos.fileEntry = nextEntry;
			pos.memOffset = 0;
			EnumerateFileEnties(pos.fileEntry, writeCallback);
		}
		else
		{
			return AppendImpl(pos, buff, buffSize);
		}
	}
	if (buffSize)
	{
		return AppendImpl(pos, buff, buffSize);
	}
	return !buffSize;
}

void FileIntfImpl::SeekEnd(BlockHandle fileHeader, FilePos& pos)
{
	FileHeader* pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid file header handle");
		return;
	}
	pos.fileEntry = 0;
	pos.memOffset = 0;
	pos.offset = 0;
	auto seekEndCallback = [&pos](BlockHandle fileEntry, FileEntry* pFileEntry)
	{
		pos.offset += pFileEntry->payloadSize;
		pos.fileEntry = fileEntry;
		return true;
	};
	EnumerateFileEnties(pFileHeader->body, seekEndCallback);
}

bool FileIntfImpl::Seek(BlockHandle fileHeader, FilePos& pos, stream_offset requestedOffset)
{
	FileHeader* pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid file header handle");
		return false;
	}
	BlockHandle targetFileEntry = 0;
	stream_offset offset = 0;
	size_t memOffset = 0;
	auto seekCallback = [&offset, &requestedOffset, &memOffset, &targetFileEntry](BlockHandle fileEntry, FileEntry* pFileEntry)
	{
		if (requestedOffset >= offset &&
			requestedOffset < offset + pFileEntry->payloadSize)
		{
			memOffset = static_cast<size_t>(requestedOffset - offset);
			targetFileEntry = fileEntry;
			return false;
		}
		offset += pFileEntry->payloadSize;
		return true;
	};
	EnumerateFileEnties(pFileHeader->body, seekCallback);
	if (!targetFileEntry && requestedOffset)
	{
		LOG_ERROR("%s", "Requested offset is outside of file");
		return false;
	}
	pos.fileEntry = targetFileEntry;
	pos.offset = requestedOffset;
	pos.memOffset = memOffset;
	return true;
}

stream_offset FileIntfImpl::GetSize(BlockHandle fileHeader)
{
	FileHeader* pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid file header handle");
		return 0;
	}
	stream_offset fileSize = 0;
	auto sizeCallback = [&fileSize](BlockHandle fileEntry, BlockTypes::FileEntry* pFileEntry)
	{
		fileSize += pFileEntry->payloadSize;
		return true;
	};
	EnumerateFileEnties(pFileHeader->body, sizeCallback);
	return fileSize;
}

bool FileIntfImpl::IsEmpty(BlockHandle fileHeader)
{
	FileHeader* pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid file header handle");
		return true;
	}
	return pFileHeader->body == 0;
}

bool FileIntfImpl::Write(BlockHandle fileHeader, const char* buff, size_t buffSize, stream_offset offset)
{
	stream_offset fileSize = GetSize(fileHeader);
	if (!fileSize && offset)
	{
		LOG_ERROR("%s", "Non-zero offset on empty file");
		return false;
	}
	if (!fileSize)
	{
		return AppendImpl(fileHeader, buff, buffSize);
	}
	if (offset >= fileSize)
	{
		LOG_ERROR("%s", "Offset is outside of file");
		return false;
	}
	FilePos pos = {0};
	if (!Seek(fileHeader, pos, offset))
	{
		return false;
	}
	return Write(pos, buff, buffSize);
}

BlockHandle FileIntfImpl::Create()
{
	BlockHandle fileHeader = _blockAllocator.Allocate(sizeof(FileHeader));
	if (!fileHeader)
	{
		LOG_ERROR("%s", "Can't allocate file header");
		return 0;
	}
	FileHeader* pFileHeader = _blockReader.CastTo<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid handle");
	}
	pFileHeader->body = 0;
	return fileHeader;
}

bool FileIntfImpl::Append(BlockHandle fileHeader, const char* buff, size_t buffSize)
{
	if (IsEmpty(fileHeader))
	{
		return AppendImpl(fileHeader, buff, buffSize);
	}
	FilePos pos = {0};
	SeekEnd(fileHeader, pos);
	return AppendImpl(pos, buff, buffSize);
}

size_t FileIntfImpl::Read(FilePos pos, char* buff, size_t buffSize)
{
	size_t memOffset = pos.memOffset;
	size_t read = 0;
	auto readCallback = [&buff, &buffSize, &memOffset, &read](BlockHandle fileEntry, FileEntry* pFileEntry)
	{
		if (memOffset && memOffset >= pFileEntry->payloadSize)
		{
			LOG_ERROR("%s", "Invalid memory offset");
			return false;
		}
		size_t toRead = std::min(buffSize, pFileEntry->payloadSize - memOffset);
		const char* dataBegin = reinterpret_cast<char*>(pFileEntry) + sizeof(FileEntry) + memOffset;
		const char* dataEnd = dataBegin + toRead;
		std::copy(dataBegin, dataEnd, buff);
		read += toRead;
		buff += toRead;
		buffSize -= toRead;
		memOffset = 0;
		return buffSize > 0;
	};
	EnumerateFileEnties(pos.fileEntry, readCallback);
	return read;
}

size_t FileIntfImpl::Read(BlockHandle fileHeader, char* buff, size_t buffSize, stream_offset offset)
{
	FilePos pos = {0};
	if (!Seek(fileHeader, pos, offset))
	{
		LOG_ERROR("%s", "Seek failed");
		return 0;
	}
	return Read(pos, buff, buffSize);
}

void FileIntfImpl::DeleteFileEntryList(BlockHandle fileEntry)
{
	BlockAllocator& blockAllocator = _blockAllocator;
	auto deleteCallback = [&blockAllocator](BlockHandle fileEntry, FileEntry* pFileEntry)
	{
		blockAllocator.Free(fileEntry);
		return true;
	};
	EnumerateFileEnties(fileEntry, deleteCallback);
}

bool FileIntfImpl::Truncate(BlockHandle fileHeader, stream_offset newSize)
{
	FilePos pos = {0};
	if (!Seek(fileHeader, pos, newSize))
	{
		LOG_ERROR("%s", "Seek failed");
		return false;
	}
	if (!pos.fileEntry)
	{
		return !!newSize;
	}
	if (!pos.memOffset)
	{
		DeleteFileEntryList(pos.fileEntry);
	}
	else
	{
		FileEntry* pFileEntry = _blockReader.Get<FileEntry>(pos.fileEntry);
		if (!pFileEntry)
		{
			LOG_ERROR("%s", "Invalid file entry");
			return false;
		}
		pFileEntry->payloadSize = pos.memOffset;
		BlockHandle oldNext = pFileEntry->next;
		DeleteFileEntryList(oldNext);
	}
	FileHeader* pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (pFileHeader &&
		pFileHeader->body == pos.fileEntry && 
		!pos.memOffset)
	{
		pFileHeader->body = 0;
	}
	return true;
}

void FileIntfImpl::Delete(BlockHandle fileHeader)
{
	FileHeader* pFileHeader = _blockReader.Get<FileHeader>(fileHeader);
	if (!pFileHeader)
	{
		LOG_ERROR("%s", "Invalid file header");
		return;
	}
	DeleteFileEntryList(pFileHeader->body);
	_blockAllocator.Free(fileHeader);
}

// FileIntf

FileIntf::FileIntf(Container& container):
_impl(new(std::nothrow) FileIntfImpl(container))
{
}

FileIntf::~FileIntf()
{
}

BlockHandle FileIntf::Create()
{
	return _impl->Create();
}

stream_offset FileIntf::GetSize(BlockHandle fileHeader)
{
	return _impl->GetSize(fileHeader);
}

bool FileIntf::IsEmpty(BlockHandle fileHeader)
{
	return _impl->IsEmpty(fileHeader);
}

size_t FileIntf::Read(BlockHandle fileHeader, char* buff, size_t buffSize, stream_offset offset)
{
	return _impl->Read(fileHeader, buff, buffSize, offset);
}

bool FileIntf::Write(BlockHandle fileHeader, const char* buff, size_t buffSize, stream_offset offset)
{
	return _impl->Write(fileHeader, buff, buffSize, offset);
}

bool FileIntf::Append(BlockHandle fileHeader, const char * buff, size_t buffSize)
{
	return _impl->Append(fileHeader, buff, buffSize);
}

bool FileIntf::Truncate(BlockHandle fileHeader, stream_offset newSize)
{
	return _impl->Truncate(fileHeader, newSize);
}

void FileIntf::Delete(BlockHandle fileHeader)
{
	return _impl->Delete(fileHeader);
}

size_t FileIntf::GetMaxPayloadSize()
{
	return BlockAllocator::GetMaxAllocationSize() - sizeof(FileEntry);
}


}//namespace FsBox