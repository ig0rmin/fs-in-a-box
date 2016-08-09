#include "Container.h"

#include "BlockTypes.h"
#include "FsUtils.h"
#include "Logging.h"

using namespace std;
using namespace BlockTypes;

bool Container::Open(const std::string& fileName)
{
	if (_mmf.IsOpened())
	{
		LOG_ERROR("%s", "Already opened");
		return false;
	}
	if (!FsUtils::FileExists(fileName) || !FsUtils::GetFileSize(fileName))
	{
		return Bootstrap(fileName);
	}
	if (!_mmf.Open(fileName))
	{
		LOG_ERROR("Can't open file %s", fileName.c_str());
		return false;
	}
	if (!CheckContainerHeader())
	{
		LOG_ERROR("File %s is not valid container", fileName.c_str());
		_mmf.Close();
		return false;
	}
	return true;
}

bool Container::IsOpened() const
{
	return _mmf.IsOpened();
}

void Container::Close()
{
	_mmf.Close();
}

void Container::Lock()
{
	_mutex.lock();
}

void Container::Unlock()
{
	_mutex.unlock();
}

MemoryMappedFile& Container::GetFileMapping()
{
	return _mmf;
}

uint32_t Container::GetMagic()
{
	return 0x46534258; //FSBX
}

bool Container::Bootstrap(const std::string& fileName)
{
	ContainerHeader header;
	header.blockType = BlockType::ContainerHeader;
	header.magic = GetMagic();
	header.freeBlock = 0;
	header.rootDir = 0;
	if (!FsUtils::WriteFile(fileName, &header, sizeof(header)))
	{
		LOG_ERROR("Can't write file %s", fileName.c_str());
		return false;
	}
	if (!_mmf.Open(fileName))
	{
		LOG_ERROR("Can't open file %s", fileName.c_str());
		return false;
	}
	return true;
}

bool Container::CheckContainerHeader()
{
	if (_mmf.GetDataSize() < sizeof(ContainerHeader))
	{
		LOG_ERROR("%s", "File is too small to be a vaild container");
		return false;
	}
	ContainerHeader* header = reinterpret_cast<ContainerHeader*>(_mmf.GetData());
	if (header->blockType != BlockType::ContainerHeader)
	{
		LOG_ERROR("%s", "Invalid container header type");
		return false;
	}
	if (header->magic != GetMagic())
	{
		LOG_ERROR("%s", "Invalid magic");
		return false;
	}
	if (header->freeBlock >= _mmf.GetFileSize())
	{
		LOG_ERROR("%s", "Free block handle points outside of file");
		return false;
	}
	if (header->rootDir >= _mmf.GetFileSize())
	{
		LOG_ERROR("%s", "Root dir handle points outside of file");
		return false;
	}
	return true;
}