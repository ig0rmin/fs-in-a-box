#ifndef __FSBOX_CONTAINER_H__
#define __FSBOX_CONTAINER_H__

#include "MMF.h"

#include <boost/noncopyable.hpp>

#include <mutex>

class Container : public boost::noncopyable
{
public:
	bool Open(const std::string& fileName);
	bool IsOpened() const;
	void Close();

	void Lock();
	void Unlock();

	MemoryMappedFile& GetFileMapping();
private:
	std::recursive_mutex _mutex;
	MemoryMappedFile _mmf;
private:
	bool Bootstrap(const std::string& fileName);
	bool CheckContainerHeader();
	static uint32_t GetMagic();
};

#endif