#ifndef __FSBOX_BLOCK_READER_H__
#define __FSBOX_BLOCK_READER_H__

#include "Container.h"
#include "BlockTypes.h"
#include "Logging.h"


template <typename T>
BlockTypes::BlockType GetBlockType()
{
	return BlockTypes::BlockType::Unknown;
}

template <>
BlockTypes::BlockType GetBlockType<BlockTypes::ContainerHeader>()
{
	return BlockTypes::BlockType::ContainerHeader;
}

template <>
BlockTypes::BlockType GetBlockType<BlockTypes::FreeBlock>()
{
	return BlockTypes::BlockType::FreeBlock;
}

template <>
BlockTypes::BlockType GetBlockType<BlockTypes::DirHeader>()
{
	return BlockTypes::BlockType::DirHeader;
}

template <>
BlockTypes::BlockType GetBlockType<BlockTypes::DirEntry>()
{
	return BlockTypes::BlockType::DirEntry;
}

template <>
BlockTypes::BlockType GetBlockType<BlockTypes::FileHeader>()
{
	return BlockTypes::BlockType::FileHeader;
}

template <>
BlockTypes::BlockType GetBlockType<BlockTypes::FileEntry>()
{
	return BlockTypes::BlockType::FileEntry;
}


template <typename T>
bool CheckBlockType(const T* block)
{
	return block->blockType == GetBlockType<T>();
}

class BlockReader : public boost::noncopyable
{
public:
	BlockReader(Container& container) : _container(container) {}
	
	template <typename T>
	T* Get(BlockHandle block)
	{
		T* ptr = UncheckedGet<T>(block);
		if (!ptr)
		{
			return nullptr;
		}
		return CheckBlockType(ptr) ? ptr : nullptr;
	}
	
	template <typename T>
	T* CastTo(BlockHandle block)
	{
		T* ptr = UncheckedGet<T>(block);
		if (!ptr)
		{
			return nullptr;
		}
		ptr->blockType = GetBlockType<T>();
		return ptr;
	}
private:
	template <typename T>
	T* UncheckedGet(BlockHandle block)
	{
		std::lock_guard<std::recursive_mutex> lock(_container.GetLock());
		MemoryMappedFile& mmf = _container.GetFileMapping();
		if (!mmf.IsOpened())
		{
			return nullptr;
		}	
		if (block > mmf.GetFileSize() - sizeof(T))
		{
			return nullptr;
		}
		if (block < mmf.GetDataOffset() ||
			block > mmf.GetDataOffset() + mmf.GetDataSize())
		{
			if (!mmf.Remap(block))
			{
				return nullptr;
			}
		}
		size_t fixup = static_cast<size_t>(block - mmf.GetDataOffset());
		return reinterpret_cast<T*>(mmf.GetData() + fixup);
	}

	Container& _container;
};

#endif