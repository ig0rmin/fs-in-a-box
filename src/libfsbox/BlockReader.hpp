#ifndef __FSBOX_BLOCK_READER_H__
#define __FSBOX_BLOCK_READER_H__

#include "Container.h"
#include "BlockTypes.h"
#include "BlockAllocator.h"

namespace FsBox
{
namespace BlockTypes
{
	template <typename T>
	BlockType GetBlockType()
	{
		return BlockType::Unknown;
	}

	template <>
	BlockType GetBlockType<ContainerHeader>();

	template <>
	BlockType GetBlockType<FreeBlock>();

	template <>
	BlockType GetBlockType<DirHeader>();

	template <>
	BlockType GetBlockType<DirEntry>();

	template <>
	BlockType GetBlockType<FileHeader>();

	template <>
	BlockType GetBlockType<FileEntry>();

	template <typename T>
	bool CheckBlockType(const T* block)
	{
		return block->blockType == GetBlockType<T>();
	}

	template <>
	bool CheckBlockType<BlockTypes::TypedBlock>(const TypedBlock* block);

	uint32_t GetMaxBlockSize();
} // namespace BlockTypes

class BlockReader : public boost::noncopyable
{
public:
	BlockReader(Container& container) : _container(container), _mmf(container.GetFileMapping()) {}
	
	template <typename T>
	T* Get(BlockHandle block)
	{
		T* ptr = UncheckedGet<T>(block);
		if (!ptr)
		{
			return nullptr;
		}
		if (!BlockTypes::CheckBlockType(ptr))
		{
			return nullptr;
		}
		if (CheckBlockPayloadBorders(ptr))
		{
			return ptr;
		}
		// Remap view to fit block payload
		_mmf.Remap(block);
		// As remap invalidates all pointers, we should do the whole process again
		ptr = UncheckedGet<T>(block);
		if (!ptr)
		{
			return nullptr;
		}
		return CheckBlockPayloadBorders(ptr) ? ptr : nullptr;
	}
	
	template <typename T>
	T* CastTo(BlockHandle block)
	{
		T* ptr = UncheckedGet<T>(block);
		if (!ptr)
		{
			return nullptr;
		}
		ptr->blockType = BlockTypes::GetBlockType<T>();
		return ptr;
	}
private:
	template <typename T>
	T* UncheckedGet(BlockHandle block)
	{
		std::lock_guard<std::recursive_mutex> lock(_container.GetLock());
		if (!_mmf.IsOpened())
		{
			return nullptr;
		}	
		if (block > _mmf.GetFileSize() - sizeof(T))
		{
			return nullptr;
		}
		if (BlockNeedsRemap<T>(block))
		{
			if (!_mmf.Remap(block))
			{
				return nullptr;
			}
			if (BlockNeedsRemap<T>(block))
			{
				return nullptr;
			}
		}
		size_t fixup = static_cast<size_t>(block - _mmf.GetDataOffset());
		return reinterpret_cast<T*>(_mmf.GetData() + fixup);
	}

	template <typename T>
	bool CheckBlockPayloadBorders(const T* ptr)
	{
		return CheckDataInMemoryView(ptr, sizeof(T));
	}

	bool CheckBlockPayloadBorders(const BlockTypes::FreeBlock* ptr)
	{
		uint32_t fullSize = ptr->size;
		if (fullSize > BlockTypes::GetMaxBlockSize())
		{
			return false;
		}
		return CheckDataInMemoryView(ptr, fullSize);
	}

	bool CheckBlockPayloadBorders(const BlockTypes::FileEntry* ptr)
	{
		uint32_t fullSize = ptr->entrySize;
		if (fullSize > BlockTypes::GetMaxBlockSize())
		{
			return false;
		}
		return CheckDataInMemoryView(ptr, fullSize);
	}

	template <typename T>
	bool BlockNeedsRemap(BlockHandle block)
	{
		return block < _mmf.GetDataOffset() ||
			block > _mmf.GetDataOffset() + _mmf.GetDataSize() - sizeof(T);
	}

	bool CheckDataInMemoryView(const void* dataPtr, size_t dataSize)
	{
		return dataPtr >= _mmf.GetData() &&
			dataPtr <= _mmf.GetData() + _mmf.GetDataSize() - dataSize;
	}

	Container& _container;
	MemoryMappedFile& _mmf;
};

}//namespace FsBox
#endif