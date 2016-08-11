#ifndef __FSBOX_BLOCK_ALLOCATOR_H__
#define __FSBOX_BLOCK_ALLOCATOR_H__

#include "Types.h"

#include <boost/noncopyable.hpp>

#include <memory>

namespace FsBox
{

class BlockAllocatorImpl;
class Container;

class BlockAllocator : public boost::noncopyable
{
public:
	BlockAllocator(Container& container);
	virtual ~BlockAllocator();

	BlockHandle Allocate(uint32_t size);
	void Free(BlockHandle block);

	static uint32_t GetMinAllocationSize();
	static uint32_t GetMaxAllocationSize();
private:
	std::unique_ptr<BlockAllocatorImpl> _impl;	
};

}
#endif