#ifndef __FSBOX_BLOCK_TYPES_H__
#define __FSBOX_BLOCK_TYPES_H__

#include "Types.h"

#include <cstdint>

#include <boost/static_assert.hpp>

namespace BlockTypes
{

enum class FileType : std::uint32_t {File, Dir, Metainfo} ;
enum class BlockType : std::uint32_t {Unknown,
							FreeBlock,
							ContainerHeader,
							DirEntry,
							DirHeader,
							FileEntry,
							FileHeader,};

struct TypedBlock
{
	BlockType blockType;
};
							
struct ContainerHeader : TypedBlock
{
	uint32_t magic;
	stream_offset rootDir;
	stream_offset freeBlock;
};

struct DoubleLinkedNode : TypedBlock
{
	stream_offset prev;
	stream_offset next;
};

struct FreeBlock : DoubleLinkedNode
{
	uint32_t size; 
};

struct DirEntry : DoubleLinkedNode
{
	FileType type;
	stream_offset name;
	stream_offset body;
};

struct DirHeader : TypedBlock
{
	stream_offset parent;
	stream_offset entryList;
};

struct FileEntry : DoubleLinkedNode
{
	uint32_t entrySize;
};

struct FileHeader : TypedBlock
{
	stream_offset parent;
	stream_offset bodySize;
	stream_offset body;
};

BOOST_STATIC_ASSERT(sizeof(stream_offset) == 8);
BOOST_STATIC_ASSERT(sizeof(ContainerHeader) == 24);
BOOST_STATIC_ASSERT(sizeof(FreeBlock) == 32);
BOOST_STATIC_ASSERT(sizeof(DirEntry) == 48);
BOOST_STATIC_ASSERT(sizeof(DirHeader) == 24);
BOOST_STATIC_ASSERT(sizeof(FileEntry) == 32);
BOOST_STATIC_ASSERT(sizeof(FileHeader) == 32);

} // namespace BlockTypes

#endif