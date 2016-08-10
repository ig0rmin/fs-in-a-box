#include "BlockAllocator.h"

#include "Logging.h"

using namespace std;
using namespace BlockTypes;

//TODO: Needs refactoring

BlockAllocator::BlockAllocator(Container& container)
:_container(container),
_blockReader(container)
{
}

uint32_t BlockAllocator::GetMinAllocationSize()
{
	return sizeof(FreeBlock);
}

BlockHandle BlockAllocator::NewFreeBlock(uint32_t size)
{
	MemoryMappedFile& mmf = _container.GetFileMapping();
	stream_offset oldSize = mmf.GetFileSize();
	mmf.Resize(oldSize + size);
	FreeBlock* pBlock = _blockReader.CastTo<FreeBlock>(oldSize);
	pBlock->size = size;
	return oldSize;
}

BlockHandle BlockAllocator::FindFreeBlock(uint32_t size)
{
	ContainerHeader* containerHeader = _blockReader.Get<ContainerHeader>(0);
	BlockHandle freeBlock = containerHeader->freeBlock;
	while (freeBlock)
	{
		FreeBlock* pFreeBlock = _blockReader.Get<FreeBlock>(freeBlock);
		if (!freeBlock)
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

void BlockAllocator::PadBlock(BlockHandle block, uint32_t size)
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

BlockHandle BlockAllocator::SplitBlock(BlockHandle block, uint32_t size)
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

BlockHandle BlockAllocator::FindInsertPoint(BlockHandle block)
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

void BlockAllocator::InsertFreeBlock(BlockHandle block)
{
	BlockHandle insertAfter = FindInsertPoint(block);
	if (!insertAfter)
	{
		ContainerHeader* containerHeader = _blockReader.Get<ContainerHeader>(0);
		BlockHandle oldListHead = containerHeader->freeBlock;
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

BlockHandle BlockAllocator::Allocate(uint32_t sizeRequested)
{
	if (sizeRequested < GetMinAllocationSize())
	{
		LOG_ERROR("%s", "Minimum allocation size is %d, requested %d", GetMinAllocationSize(), sizeRequested);
		return 0;
	}
	BlockHandle block = FindFreeBlock(sizeRequested);
	if (!block)
	{
		return NewFreeBlock(sizeRequested);
	}
	FreeBlock* pFoundBlock = _blockReader.Get<FreeBlock>(block);
	uint32_t sizeRemaining = pFoundBlock->size - sizeRequested;
	if (sizeRemaining < GetMinAllocationSize())
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

void BlockAllocator::UnpadBlock(BlockHandle block)
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
	}
	pFreeBlock->size += padSize;
}

uint32_t BlockAllocator::GetBlockSize(BlockHandle block)
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

bool BlockAllocator::Merge(BlockHandle base, BlockHandle supply)
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

void BlockAllocator::TryMergeRight(BlockHandle block)
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

BlockHandle BlockAllocator::TryMergeLeft(BlockHandle block)
{
	BlockHandle leftmostFreeBlock = FindInsertPoint(block);
	if (!leftmostFreeBlock)
	{
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

void BlockAllocator::Free(BlockHandle block)
{
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