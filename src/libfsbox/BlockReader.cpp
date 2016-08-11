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
}//namespace BlockTypes

template <>
bool CheckBlockType<BlockTypes::TypedBlock>(const BlockTypes::TypedBlock* block)
{
	return block->blockType >= BlockTypes::BlockType::FreeBlock &&
		block->blockType <= BlockTypes::BlockType::FileHeader;
}

}//namespace FsBox