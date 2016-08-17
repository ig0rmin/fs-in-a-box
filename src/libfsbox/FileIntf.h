#ifndef __FSBOX_FILE_INFT_H__
#define __FSBOX_FILE_INFT_H__

#include "BlockAllocator.h"
#include "BlockReader.hpp"

#include <functional>

namespace FsBox
{

struct FStreamPos
{
	BlockHandle fileEntry;
	stream_offset offset;;
	size_t memOffset;
};

class FStreamIntf : public boost::noncopyable
{
public:
	FStreamIntf(Container& container);

	BlockHandle Create();
	stream_offset GetSize(BlockHandle fileHeader);
	bool IsEmpty(BlockHandle fileHeader);
	size_t Read(BlockHandle fileHeader, char* buff, size_t buffSize, stream_offset offset);
	bool Write(BlockHandle fileHeader, const char* buff, size_t buffSize, stream_offset offset);
	bool Append(BlockHandle fileHeader, const char* buff, size_t buffSize);
	bool Truncate(BlockHandle fileHeader, stream_offset newSize);
	void Delete(BlockHandle fileHeader);

	static uint32_t GetMaxPayloadSize();
private:
	using EnumFileEntriesFn = bool(BlockHandle fileEntry, BlockTypes::FileEntry* pFileEntry);
	void EnumerateFileEnties(BlockHandle fileEntry, std::function<EnumFileEntriesFn> callback);

	BlockHandle NextFileEntry(BlockHandle fileEntry);
	BlockHandle CreateFileEntry(uint32_t payloadSize);
	size_t WriteToFileEntry(BlockHandle fileEntry, const char* buff, size_t buffSize, size_t offset);

	size_t Read(FStreamPos pos, char* buff, size_t buffSize);
	bool Write(FStreamPos pos, const char* buff, size_t buffSize);
	bool Seek(BlockHandle fileHeader, FStreamPos& pos, stream_offset offset);
	void SeekEnd(BlockHandle fileHeader, FStreamPos& pos);
	

	bool AppendImpl(FStreamPos pos, const char* buff, size_t buffSize);
	bool AppendImpl(BlockHandle fileHeader, const char* buff, size_t buffSize);

	void DeleteFileEntryList(BlockHandle fileEntry);
private:
	BlockAllocator _blockAllocator;
	BlockReader	_blockReader;
};

}//namespace FsBox
#endif