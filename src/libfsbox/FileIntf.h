#ifndef __FSBOX_FILE_INFT_H__
#define __FSBOX_FILE_INFT_H__

#include "Types.h"

#include <boost/noncopyable.hpp>

#include <memory>

namespace FsBox
{

class Container;

// This class by desgin has no syncronisation and no state
class FileIntf : public boost::noncopyable
{
public:
	FileIntf(Container& container);
	virtual ~FileIntf();

	BlockHandle Create();
	stream_offset GetSize(BlockHandle file);
	bool IsEmpty(BlockHandle file);
	size_t Read(BlockHandle file, char* buff, size_t buffSize, stream_offset offset);
	bool Write(BlockHandle file, const char* buff, size_t buffSize, stream_offset offset);
	bool Append(BlockHandle file, const char* buff, size_t buffSize);
	bool Truncate(BlockHandle file, stream_offset newSize);
	void Delete(BlockHandle file);

	// Use it to determine best caching strategy
	static size_t GetMaxPayloadSize();
private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

}//namespace FsBox
#endif