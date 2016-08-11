#ifndef __FSBOX_FS_UTILS_H__
#define __FSBOX_FS_UTILS_H__

#include "Types.h"

#include <string>

namespace FsBox
{
namespace FsUtils
{

bool FileExists(const std::string& fileName);
stream_offset GetFileSize(const std::string& fileName);
bool WriteFile(const std::string& fileName, const void* buff, size_t buffSize);

}//namespace FsUtils
}//namespace FsBox
#endif