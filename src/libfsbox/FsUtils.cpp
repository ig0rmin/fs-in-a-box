#include "FsUtils.h"

#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#define _stat64 stat64
#endif

BOOST_STATIC_ASSERT(sizeof(_stat64::st_size) == sizeof(FsBox::stream_offset));

using namespace std;

namespace FsBox
{
namespace FsUtils
{

bool FileExists(const std::string& fileName)
{
	struct _stat64 dummy = {0};
	return !(_stat64(fileName.c_str(), &dummy) && 
		(errno == ENOENT));
}

stream_offset GetFileSize(const std::string& fileName)
{
	struct _stat64 s = {0};
	return !(_stat64(fileName.c_str(), &s)) ? s.st_size : 0;
}

bool WriteFile(const std::string& fileName, const void* buff, size_t buffSize)
{
	ofstream out(fileName, ios::binary);
	if (!out.good())
	{
		return false;
	}
	const char* begin = static_cast<const char*>(buff);
	const char* end = begin + buffSize;
	copy(begin, end, ostream_iterator<char>(out));
	return out.good();
}

}//namespace FsUtils
}//namespace FsBox