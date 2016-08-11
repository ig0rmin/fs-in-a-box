#ifndef __FSBOX_CONTAINER_H__
#define __FSBOX_CONTAINER_H__

#include "MMF.h"

#include <boost/noncopyable.hpp>

#include <mutex>

namespace FsBox
{

class Container : public boost::noncopyable
{
public:
	bool Open(const std::string& fileName);
	bool IsOpened() const;
	void Close();

	std::recursive_mutex& GetLock();

	MemoryMappedFile& GetFileMapping();
private:
	bool Bootstrap(const std::string& fileName);
	bool CheckContainerHeader();
	static uint32_t GetMagic();
private:
	std::recursive_mutex _mutex;
	MemoryMappedFile _mmf;
};

}//namespace FsBox
#endif