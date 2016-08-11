#include <libfsbox/Container.h>
#include <libfsbox/BlockAllocator.h>
#include <libfsbox/BlockTypes.h>

#include "gtest/gtest.h"
#include "TestUtils.h"

using namespace std;

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