#include <libfsbox/Container.h>
#include <libfsbox/BlockAllocator.h>
#include <libfsbox/BlockReader.hpp>
#include <libfsbox/BlockTypes.h>

#include "gtest/gtest.h"
#include "TestUtils.h"

using namespace std;
using namespace FsBox;
using namespace FsBox::BlockTypes;

class BlockAllocatorTestsuite : public testing::Test
{
protected:
	virtual void SetUp()
	{
		ostringstream os;
		os << "BLKALLOC" << counter++ << ".fsbox";
		fileName = os.str();
		ASSERT_TRUE(container.Open(fileName));
		ASSERT_TRUE(container.IsOpened());
		ASSERT_GT(container.GetFileMapping().GetFileSize(), 0);
	}

	virtual void TearDown()
	{
		container.Close();
		TestUtils::DeleteFile(fileName);
	}

	Container container;
	string fileName;

	static size_t counter;
};

size_t BlockAllocatorTestsuite::counter = 0;

TEST_F(BlockAllocatorTestsuite, AllocAndFree)
{
	BlockAllocator allocator(container);
	const uint32_t sizeToAlloc = BlockAllocator::GetMinAllocationSize()*2;
	BlockHandle allocated = allocator.Allocate(sizeToAlloc);
	EXPECT_TRUE(allocated);
	stream_offset containerSize = container.GetFileMapping().GetFileSize();
	EXPECT_EQ(sizeof(BlockTypes::ContainerHeader) + sizeToAlloc, containerSize);
	allocator.Free(allocated);
}

TEST_F(BlockAllocatorTestsuite, AllocTooSmall)
{
	BlockAllocator allocator(container);
	EXPECT_FALSE(allocator.Allocate(BlockAllocator::GetMinAllocationSize() - 1));
}

TEST_F(BlockAllocatorTestsuite, AllocTooBig)
{
	BlockAllocator allocator(container);
	EXPECT_FALSE(allocator.Allocate(BlockAllocator::GetMaxAllocationSize() + 1));
}

TEST_F(BlockAllocatorTestsuite, ReuseFreeBlock)
{
	BlockAllocator allocator(container);
	const uint32_t sizeToAlloc = BlockAllocator::GetMinAllocationSize()*2;
	BlockHandle allocated = allocator.Allocate(sizeToAlloc);
	EXPECT_TRUE(allocated);
	allocator.Free(allocated);
	BlockHandle expectedAddress = allocated;
	allocated = allocator.Allocate(sizeToAlloc);
	EXPECT_EQ(expectedAddress, allocated);
}

TEST_F(BlockAllocatorTestsuite, SplitBlock)
{
	BlockAllocator allocator(container);
	const uint32_t sizeToAlloc = BlockAllocator::GetMinAllocationSize()*2;
	BlockHandle allocatedBig = allocator.Allocate(sizeToAlloc);
	EXPECT_TRUE(allocatedBig);
	stream_offset containerSize = container.GetFileMapping().GetFileSize();
	allocator.Free(allocatedBig);
	// allocate allocSmall01 + allocSmall02 = allcoatedBig
	BlockHandle allocSmall01 = allocator.Allocate(sizeToAlloc / 2);
	EXPECT_TRUE(allocSmall01);
	BlockHandle allocSmall02 = allocator.Allocate(sizeToAlloc / 2);
	EXPECT_TRUE(allocSmall02);
	// The size is same, we are reusing old block
	EXPECT_EQ(containerSize, container.GetFileMapping().GetFileSize());
}

TEST_F(BlockAllocatorTestsuite, MergeRight)
{
	BlockAllocator allocator(container);
	BlockHandle alloc01 = allocator.Allocate(BlockAllocator::GetMinAllocationSize());
	EXPECT_TRUE(alloc01);
	BlockHandle alloc02 = allocator.Allocate(BlockAllocator::GetMinAllocationSize());
	EXPECT_TRUE(alloc02);
	stream_offset containerSize = container.GetFileMapping().GetFileSize();
	allocator.Free(alloc02); // right
	allocator.Free(alloc01); // left, we should merge right block
	BlockHandle allocBig = allocator.Allocate(BlockAllocator::GetMinAllocationSize()*2);
	EXPECT_EQ(containerSize, container.GetFileMapping().GetFileSize());
	EXPECT_EQ(alloc01, allocBig);
}

TEST_F(BlockAllocatorTestsuite, MergeLeft)
{
	BlockAllocator allocator(container);
	BlockHandle alloc01 = allocator.Allocate(BlockAllocator::GetMinAllocationSize());
	EXPECT_TRUE(alloc01);
	BlockHandle alloc02 = allocator.Allocate(BlockAllocator::GetMinAllocationSize());
	EXPECT_TRUE(alloc02);
	stream_offset containerSize = container.GetFileMapping().GetFileSize();
	allocator.Free(alloc01); // left
	allocator.Free(alloc02); // right, we should merge left block
	BlockHandle allocBig = allocator.Allocate(BlockAllocator::GetMinAllocationSize()*2);
	EXPECT_EQ(containerSize, container.GetFileMapping().GetFileSize());
	EXPECT_EQ(alloc01, allocBig);
}

TEST_F(BlockAllocatorTestsuite, PadUnpad)
{
	BlockAllocator allocator(container);
	BlockHandle alloc01 = allocator.Allocate(BlockAllocator::GetMinAllocationSize()*2);
	EXPECT_TRUE(alloc01);
	allocator.Free(alloc01);
	// asking to allocate block just a bit smaller that we have in a reserve
	// we should reuse the same block, padding trailing bytes with 0
	BlockHandle alloc02 = allocator.Allocate(BlockAllocator::GetMinAllocationSize()*2 - 4);
	EXPECT_EQ(alloc01, alloc02);
	// free the allocated block. allocator should check for trailing padding
	// and unpad block, returning to the reserve block with the original size
	allocator.Free(alloc02);
	BlockHandle alloc03 = allocator.Allocate(BlockAllocator::GetMinAllocationSize()*2);
	EXPECT_EQ(alloc01, alloc03);
}

TEST_F(BlockAllocatorTestsuite, AllocLarge)
{
	const size_t maxMemViewSize = container.GetFileMapping().GetMaxViewSize();
	const uint32_t sizeToAlloc = BlockAllocator::GetMaxAllocationSize();
	
	BlockAllocator allocator(container);
	vector<BlockHandle> allocatedBlocks;
	stream_offset allocatedSize = 0;
	do
	{
		BlockHandle allocated = allocator.Allocate(sizeToAlloc);
		EXPECT_TRUE(allocated);
		allocatedBlocks.push_back(allocated);
		allocatedSize += sizeToAlloc;
	}
	while (allocatedSize < 2*maxMemViewSize);

	BlockReader blockReader(container);
	for (BlockHandle blockHandle : allocatedBlocks)
	{
		FreeBlock* pBlock = blockReader.Get<FreeBlock>(blockHandle);
		EXPECT_TRUE(pBlock);
	}
}

TEST_F(BlockAllocatorTestsuite, CheckDataConsistency)
{
	const size_t maxMemViewSize = container.GetFileMapping().GetMaxViewSize();
	const uint32_t sizeToAlloc = BlockAllocator::GetMaxAllocationSize();

	BlockAllocator allocator(container);
	BlockReader blockReader(container);

	
	BlockHandle firstBlock = allocator.Allocate(sizeToAlloc);
	EXPECT_TRUE(firstBlock);
	// Fill the block
	{
		FreeBlock* pFirstBlock = blockReader.Get<FreeBlock>(firstBlock);
		EXPECT_TRUE(pFirstBlock);
		uint32_t firstBlockSize = pFirstBlock->size;
		EXPECT_EQ(sizeToAlloc, firstBlockSize);
		FileEntry* pFirstBlockFileEntry = blockReader.CastTo<FileEntry>(firstBlock);
		EXPECT_TRUE(pFirstBlockFileEntry);
		pFirstBlockFileEntry->entrySize = firstBlockSize;
		size_t payloadSize = firstBlockSize - sizeof(FileEntry);
		char* payloadBegin = reinterpret_cast<char*>(pFirstBlockFileEntry) + sizeof(FileEntry);
		char* payloadEnd = payloadBegin + payloadSize;
		fill(payloadBegin, payloadEnd, 'F');
		pFirstBlockFileEntry->payloadSize = payloadSize;
	}

	// Allocate a lot of blocks, forcing memory view to remap
	vector<BlockHandle> allocatedBlocks;
	stream_offset allocatedSize = 0;
	do
	{
		const uint32_t sizeToAlloc = BlockAllocator::GetMaxAllocationSize();
		BlockHandle allocated = allocator.Allocate(sizeToAlloc);
		EXPECT_TRUE(allocated);
		allocatedBlocks.push_back(allocated);
		allocatedSize += sizeToAlloc;
	}
	while (allocatedSize < 2*maxMemViewSize);

	BlockHandle lastBlock = allocator.Allocate(sizeToAlloc);
	EXPECT_TRUE(lastBlock);
	// Fill the block
	{
		//Fill the block letter L
		FreeBlock* pLastBlock = blockReader.Get<FreeBlock>(lastBlock);
		EXPECT_TRUE(pLastBlock);
		uint32_t lastBlockSize = pLastBlock->size;
		EXPECT_EQ(sizeToAlloc, lastBlockSize);
		size_t payloadSize = lastBlockSize - sizeof(FileEntry);
		FileEntry* pLastBlockFileEntry = blockReader.CastTo<FileEntry>(lastBlock);
		pLastBlockFileEntry->entrySize = lastBlockSize;
		char* payloadBegin = reinterpret_cast<char*>(pLastBlockFileEntry) + sizeof(FileEntry);
		char* payloadEnd = payloadBegin + payloadSize;
		fill(payloadBegin, payloadEnd, 'L');
		pLastBlockFileEntry->payloadSize = payloadSize;
	}

	// Check the first block
	{
		EXPECT_FALSE(blockReader.Get<FreeBlock>(firstBlock));
		FileEntry* pFirstBlockFileEntry = blockReader.Get<FileEntry>(firstBlock);
		EXPECT_TRUE(pFirstBlockFileEntry);
		uint32_t payloadSize = pFirstBlockFileEntry->payloadSize;
		const char* payloadBegin = reinterpret_cast<char*>(pFirstBlockFileEntry) + sizeof(FileEntry);
		const char* payloadEnd = payloadBegin + payloadSize;
		EXPECT_TRUE(all_of(payloadBegin, payloadEnd, [](char ch){return ch == 'F';}));
	}

	//Check the last block 
	{
		EXPECT_FALSE(blockReader.Get<FreeBlock>(lastBlock));
		FileEntry* pLastBlockEntry = blockReader.Get<FileEntry>(lastBlock);
		EXPECT_TRUE(pLastBlockEntry);
		uint32_t payloadSize = pLastBlockEntry->payloadSize;
		char* payloadBegin = reinterpret_cast<char*>(pLastBlockEntry) + sizeof(FileEntry);
		char* payloadEnd = payloadBegin + payloadSize;
		EXPECT_TRUE(all_of(payloadBegin, payloadEnd, [](char ch){return ch == 'L';}));
	}
}

TEST_F(BlockAllocatorTestsuite, FreeAlreadyFreed)
{
	BlockAllocator allocator(container);

	BlockHandle block = allocator.Allocate(BlockAllocator::GetMinAllocationSize());
	BlockHandle mergeGuard = allocator.Allocate(BlockAllocator::GetMinAllocationSize());
	BlockHandle block2 = allocator.Allocate(BlockAllocator::GetMinAllocationSize());
	
	allocator.Free(block);
	allocator.Free(block2);

	allocator.Free(block2); // Check that we are not hanging here
	
	SUCCEED();
}