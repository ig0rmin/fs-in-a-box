#ifndef __FSBOX_MMF_H__
#define __FSBOX_MMF_H__

#include "Types.h"

// This class workarounds bugs and limitations of boost::iostreams::mapped_file
// realted to large files support
class MemoryMappedFile
{
public:
	MemoryMappedFile(size_t memViewSize = 10*1024*1024);
	virtual ~MemoryMappedFile();

	bool Open(const std::string& fileName, stream_offset offset = 0);
	bool IsOpened() const;
	void Close();

	char* GetData();
	size_t GetDataSize() const;

	stream_offset GetFileSize() const;
	stream_offset GetDataOffset() const;
	bool Remap(stream_offset offset);
	bool Resize(stream_offset size);
private:
	boost::iostreams::mapped_file _mf;
	std::string _fileName;
	stream_offset _fileOffset;
	stream_offset _fileSize;
	size_t _alignment;
	size_t _memViewSize;
private:
	bool PeekFile(const std::string& fileName);
};

#endif