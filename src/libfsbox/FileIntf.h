#ifndef __FSBOX_FILE_INFT_H__
#define __FSBOX_FILE_INFT_H__

#include "Types.h"

#include <boost/noncopyable.hpp>

#include <memory>

namespace FsBox
{

class Container;
class FileIntfImpl;

// This class by desgin has no syncronisation and no state
class FileIntf : public boost::noncopyable
{
public:
	FileIntf(Container& container);
	virtual ~FileIntf();

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
	std::unique_ptr<FileIntfImpl> _impl;
};

}//namespace FsBox
#endif