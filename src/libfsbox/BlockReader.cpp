#include "BlockReader.hpp"

namespace FsBox
{
namespace BlockTypes
{
	template <>
	BlockType GetBlockType<ContainerHeader>()
	{
		return BlockType::ContainerHeader;
	}

	template <>
	BlockType GetBlockType<FreeBlock>()
	{
		return BlockType::FreeBlock;
	}

	template <>
	BlockType GetBlockType<DirHeader>()
	{
		return BlockType::DirHeader;
	}

	template <>
	BlockType GetBlockType<DirEntry>()
	{
		return BlockType::DirEntry;
	}

	template <>
	BlockType GetBlockType<FileHeader>()
	{
		return BlockType::FileHeader;
	}

	template <>
	BlockType GetBlockType<FileEntry>()
	{
		return BlockType::FileEntry;
	}

	template <>
	bool CheckBlockType<BlockTypes::TypedBlock>(const BlockTypes::TypedBlock* block)
	{
		return block->blockType >= BlockTypes::BlockType::FreeBlock &&
			block->blockType <= BlockTypes::BlockType::FileHeader;
	}

	uint32_t GetMaxBlockSize()
	{
		//TODO: May be we can guess better maximum size for block?
		// Note that we can't make this value dependend on some particular CPU, because .fsbox files shud be portable from
		// one machine to another
		// So why? 64*1024 is the allocation granularity on my system, 20 is just reasonable value
		// Also I like this number because it is close to 3.5 diskette size
		return 64*1024*20;
	}
}//namespace BlockTypes

}//namespace FsBox