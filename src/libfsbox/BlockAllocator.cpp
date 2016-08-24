#include "BlockAllocator.h"

#include "BlockReader.hpp"
#include "Logging.h"

using namespace std;
using namespace FsBox::BlockTypes;

namespace FsBox
{

//TODO: Needs review
class BlockAllocatorImpl : public boost::noncopyable
{
public:
	BlockAllocatorImpl(Container& container);

	BlockHandle Allocate(uint32_t size);
	void Free(BlockHandle block);
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
private:
	Container& _container;
	BlockReader _blockReader;
};

BlockAllocatorImpl::BlockAllocatorImpl(Container& container)
:_container(container),
_blockReader(container)
{
}

BlockHandle BlockAllocatorImpl::NewFreeBlock(uint32_t size)
{
	MemoryMappedFile& mmf = _container.GetFileMapping();
	stream_offset oldSize = mmf.GetFileSize();
	mmf.Resize(oldSize + size);
	FreeBlock* pBlock = _blockReader.CastTo<FreeBlock>(oldSize);
	pBlock->size = size;
	return oldSize;
}

BlockHandle BlockAllocatorImpl::FindFreeBlock(uint32_t size)
{
	ContainerHeader* containerHeader = _blockReader.Get<ContainerHeader>(0);
	BlockHandle freeBlock = containerHeader->freeBlock;
	while (freeBlock)
	{
		FreeBlock* pFreeBlock = _blockReader.Get<FreeBlock>(freeBlock);
		if (!pFreeBlock)
		{
			LOG_ERROR("%s", "Broken link in free blocks list");
			return 0;
		}
		if (pFreeBlock->size >= size)
		{
			break;
		}
		freeBlock = pFreeBlock->next;
	}
	return freeBlock;
}

void BlockAllocatorImpl::PadBlock(BlockHandle block, uint32_t size)
{
	if (size < sizeof(FreeBlock))
	{
		LOG_ERROR("%s", "Can't shrink block below sizeof(FreeBlock)");
		return;
	}
	FreeBlock* pFreeBlock = _blockReader.Get<FreeBlock>(block);
	if (!pFreeBlock)
	{
		LOG_ERROR("%s", "Invalid block");
		return;
	}
	if (size > pFreeBlock->size)
	{
		LOG_ERROR("Block size is %s, requested to shrink to %s", pFreeBlock->size, size);
		return;
	}
	char* padBegin = reinterpret_cast<char*>(pFreeBlock) + size;
	char* padEnd = padBegin + (pFreeBlock->size - size);
	fill(padBegin, padEnd, 0);
	pFreeBlock->size = size;
}

BlockHandle BlockAllocatorImpl::SplitBlock(BlockHandle block, uint32_t size)
{
	if (size < sizeof(FreeBlock))
	{
		LOG_ERROR("%s", "Can't shrink block below sizeof(FreeBlock)");
		return 0;
	}
	FreeBlock* pFreeBlock = _blockReader.Get<FreeBlock>(block);
	if (!pFreeBlock)
	{
		LOG_ERROR("%s", "Invalid block");
		return 0;
	}
	if (size > pFreeBlock->size)
	{
		LOG_ERROR("Block size is %s, requested to shrink to %s", pFreeBlock->size, size);
		return 0;
	}
	uint32_t tailSize = pFreeBlock->size - size;
	if (tailSize < sizeof(FreeBlock))
	{
		LOG_ERROR("Can't split block, tail is %d that is too small", tailSize);
		return 0;
	}
	// Ok, let's do it
	pFreeBlock->size = size;
	BlockHandle tail = block + size;
	FreeBlock* pTail = _blockReader.CastTo<FreeBlock>(tail);
	pTail->size = tailSize;
	return tail;
}

BlockHandle BlockAllocatorImpl::FindInsertPoint(BlockHandle block)
{
	ContainerHeader* containerHeader = _blockReader.Get<ContainerHeader>(0);
	BlockHandle freeBlock = containerHeader->freeBlock;
	BlockHandle insertAfter = 0;
	while(freeBlock && freeBlock < block)
	{
		insertAfter = freeBlock;
		FreeBlock* pFreeBlock = _blockReader.Get<FreeBlock>(freeBlock);
		if (!pFreeBlock)
		{
			LOG_ERROR("%s",  "Broken link in free blocks list");
			break;
		}
		freeBlock = pFreeBlock->next;
	}
	return insertAfter;
}

void BlockAllocatorImpl::InsertFreeBlock(BlockHandle block)
{
	BlockHandle insertAfter = FindInsertPoint(block);
	if (insertAfter == block)
	{
		LOG_ERROR("%s", "Trying to free already freed block");
		return;
	}
	if (!insertAfter)
	{
		ContainerHeader* containerHeader = _blockReader.Get<ContainerHeader>(0);
		BlockHandle oldListHead = containerHeader->freeBlock;
		if (oldListHead == block)
		{
			LOG_ERROR("%s", "Trying to free already freed block");
			return;
		}
		FreeBlock* pBlock = _blockReader.Get<FreeBlock>(block);
		if (!pBlock)
		{
			LOG_ERROR("%s", "Invalid block");
			return;
		}
		pBlock->prev = 0;
		pBlock->next = oldListHead;
		containerHeader = _blockReader.Get<ContainerHeader>(0);
		containerHeader->freeBlock = block;
	}
	else
	{
		FreeBlock* pInsertAfter = _blockReader.Get<FreeBlock>(insertAfter);
		if (!pInsertAfter)
		{
			LOG_ERROR("%s", "Invalid block");
			return;
		}
		BlockHandle next = pInsertAfter->next;
		FreeBlock* pBlock = _blockReader.Get<FreeBlock>(block);
		if (!pBlock)
		{
			LOG_ERROR("%s", "Invalid block");
			return;
		}
		pBlock->prev = insertAfter;
		pBlock->next = next;
		pInsertAfter = _blockReader.Get<FreeBlock>(insertAfter);
		pInsertAfter->next = block;
	}
}

BlockHandle BlockAllocatorImpl::Allocate(uint32_t sizeRequested)
{
	if (sizeRequested < BlockAllocator::GetMinAllocationSize())
	{
		LOG_ERROR("Minimum allocation size is %d, requested %d", BlockAllocator::GetMinAllocationSize(), sizeRequested);
		return 0;
	}
	if (sizeRequested > BlockAllocator::GetMaxAllocationSize())
	{
		LOG_ERROR("Maximum allocation size is %d, requested %d", BlockAllocator::GetMaxAllocationSize(), sizeRequested);
		return 0;
	}
	lock_guard<recursive_mutex> lock(_container.GetLock());
	BlockHandle block = FindFreeBlock(sizeRequested);
	if (!block)
	{
		return NewFreeBlock(sizeRequested);
	}
	FreeBlock* pFoundBlock = _blockReader.Get<FreeBlock>(block);
	uint32_t sizeRemaining = pFoundBlock->size - sizeRequested;
	if (sizeRemaining < BlockAllocator::GetMinAllocationSize())
	{
		PadBlock(block, sizeRequested);
	}
	else
	{
		BlockHandle tail = SplitBlock(block, sizeRequested);
		InsertFreeBlock(tail);
	}
	return block;
}

void BlockAllocatorImpl::UnpadBlock(BlockHandle block)
{
	FreeBlock* pFreeBlock = _blockReader.Get<FreeBlock>(block);
	if (!pFreeBlock)
	{
		LOG_ERROR("%s", "Invalid block");
		return;
	}
	BlockHandle outTheBlock = block + pFreeBlock->size;
	char* pOutTheBlock = reinterpret_cast<char*>(pFreeBlock) + pFreeBlock->size;
	MemoryMappedFile& mmf = _container.GetFileMapping();
	char* memLimit = mmf.GetData() + mmf.GetDataSize();
	BlockHandle blockLimit = mmf.GetFileSize();
	uint32_t padSize = 0;
	while (outTheBlock < blockLimit && 
		pOutTheBlock < memLimit &&
		*pOutTheBlock == 0)
	{
		++padSize;
		++outTheBlock;
		++pOutTheBlock;
		if (padSize > BlockAllocator::GetMinAllocationSize())
		{
			LOG_ERROR("%s", "Padding size can't be so big. Probably corrupted block");
			return;
		}
	}
	pFreeBlock->size += padSize;
}

uint32_t BlockAllocatorImpl::GetBlockSize(BlockHandle block)
{
	TypedBlock* pTypedBlock = _blockReader.Get<TypedBlock>(block);
	if (!pTypedBlock)
	{
		LOG_ERROR("%s", "Invalid block");
		return 0;
	}
	switch (pTypedBlock->blockType)
	{
	case BlockType::DirEntry:
		return sizeof(DirEntry);
	case BlockType::DirHeader:
		return sizeof(DirHeader);
	case BlockType::FileHeader:
		return sizeof(FileHeader);
	case BlockType::ContainerHeader:
		return sizeof(ContainerHeader);
	case BlockType::FreeBlock:
		return _blockReader.Get<FreeBlock>(block)->size;
	case BlockType::FileEntry:
		return _blockReader.Get<FileEntry>(block)->entrySize;
	default:
		LOG_ERROR("Unknown block type: %d", pTypedBlock->blockType);
		return 0;
	}
}

bool BlockAllocatorImpl::Merge(BlockHandle base, BlockHandle supply)
{
	FreeBlock* pSupply = _blockReader.Get<FreeBlock>(supply);
	if (!pSupply)
	{
		LOG_ERROR("%s", "Invalid block");
		return false;
	}
	BlockHandle supplyPrev = pSupply->prev;
	BlockHandle supplyNext = pSupply->next;
	uint32_t supplySize = pSupply->size;
	FreeBlock* pBase = _blockReader.Get<FreeBlock>(base);
	if (!pBase)
	{
		LOG_ERROR("%s", "Invalid block");
		return false;
	}
	if (supply != base + pBase->size)
	{
		LOG_ERROR("%s", "Base and Supply blocks are not adjoined");
		return false;
	}
	if (supplyPrev)
	{
		FreeBlock* pSupplyPrev = _blockReader.Get<FreeBlock>(supplyPrev);
		if (!pSupplyPrev)
		{
			LOG_ERROR("%s", "Invalid block");
			return false;
		}
		pSupplyPrev->next = base;
	}
	if (supplyNext)
	{
		FreeBlock* pSupplyNext = _blockReader.Get<FreeBlock>(supplyNext);
		if (!pSupplyNext)
		{
			LOG_ERROR("%s", "Invalid block");
			return false;
		}
		pSupplyNext->prev = base;
	}
	pBase = _blockReader.Get<FreeBlock>(base);
	pBase->next = supplyNext;
	pBase->prev = supplyPrev;
	pBase->size += supplySize;
	return true;
}

void BlockAllocatorImpl::TryMergeRight(BlockHandle block)
{
	FreeBlock* pFreeBlock = _blockReader.Get<FreeBlock>(block);
	if (!pFreeBlock)
	{
		LOG_ERROR("%s", "Invalid block");
		return;
	}
	BlockHandle nextBlock = block + pFreeBlock->size;
	MemoryMappedFile& mmf = _container.GetFileMapping();
	BlockHandle blockLimit = mmf.GetFileSize();
	if (nextBlock >= blockLimit)
	{
		return;
	}
	TypedBlock* pBlockRight = _blockReader.Get<TypedBlock>(nextBlock);
	if (!pBlockRight)
	{
		LOG_ERROR("%s", "Invalid right block");
		return;
	}
	if (pBlockRight->blockType == BlockType::FreeBlock)
	{
		Merge(block, nextBlock);
	}
}

BlockHandle BlockAllocatorImpl::TryMergeLeft(BlockHandle block)
{
	BlockHandle leftmostFreeBlock = FindInsertPoint(block);
	if (!leftmostFreeBlock)
	{
		return block;
	}
	if (leftmostFreeBlock == block)
	{
		LOG_ERROR("%s", "Trying to free already freed block");
		return block;
	}
	FreeBlock* pLeftmostFreeBlock = _blockReader.Get<FreeBlock>(leftmostFreeBlock);
	if (!pLeftmostFreeBlock)
	{
		LOG_ERROR("%s", "Invalid right block");
		return block;
	}
	if (leftmostFreeBlock + pLeftmostFreeBlock->size == block)
	{
		return Merge(leftmostFreeBlock, block) ? leftmostFreeBlock : block;
	}
	return block;
}

void BlockAllocatorImpl::Free(BlockHandle block)
{
	lock_guard<recursive_mutex> lock(_container.GetLock());
	uint32_t blockSize = GetBlockSize(block);
	if (!blockSize)
	{
		return;
	}
	FreeBlock* pBlock = _blockReader.CastTo<FreeBlock>(block);
	if (!pBlock)
	{
		LOG_ERROR("%s", "Invalid block");
		return;
	}
	pBlock->size = blockSize;
	UnpadBlock(block);
	TryMergeRight(block);
	block = TryMergeLeft(block);
	InsertFreeBlock(block);
}

// BlockAllocator

BlockAllocator::BlockAllocator(Container& container)
:_impl(new(nothrow) BlockAllocatorImpl(container))
{
}

BlockAllocator::~BlockAllocator()
{
}

uint32_t BlockAllocator::GetMinAllocationSize()
{
	return sizeof(FreeBlock);
}

uint32_t BlockAllocator::GetMaxAllocationSize()
{
	return GetMaxBlockSize();
}

BlockHandle BlockAllocator::Allocate(uint32_t size)
{
	return _impl->Allocate(size);
}

void BlockAllocator::Free(BlockHandle block)
{
	return _impl->Free(block);
}

}//namespace FsBox