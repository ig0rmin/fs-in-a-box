#ifndef __FSBOX_BLOCK_ALLOCATOR_H__
#define __FSBOX_BLOCK_ALLOCATOR_H__

//TODO: Needs refactoring
//TODO: Think about border values

#include "BlockReader.hpp"

class BlockAllocator : public boost::noncopyable
{
public:
	BlockAllocator(Container& container);

	BlockHandle Allocate(uint32_t size);
	void Free(BlockHandle block);

	static uint32_t GetMinAllocationSize();
private:
	Container& _container;
	BlockReader _blockReader;
private:
	BlockHandle NewFreeBlock(uint32_t size);
	BlockHandle FindFreeBlock(uint32_t size);
	void PadBlock(BlockHandle block, uint32_t size);
	void UnpadBlock(BlockHandle block);
	BlockHandle SplitBlock(BlockHandle block, uint32_t size);
	BlockHandle FindInsertPoint(BlockHandle block);
	void InsertFreeBlock(BlockHandle block);
	uint32_t GetBlockSize(BlockHandle block);
	bool Merge(BlockHandle base, BlockHandle supply);
	void TryMergeRight(BlockHandle block);
	BlockHandle TryMergeLeft(BlockHandle block);
};

#endif