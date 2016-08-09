#ifndef __FSBOX_TEST_UTILS_H__
#define __FSBOX_TEST_UTILS_H__

#include <string>

namespace TestUtils
{

bool CreateSmallFile(const std::string& fileName, const std::string& fileContent);
bool ReadSmallFile(const std::string& fileName, std::string& fileContent);
bool DeleteFile(const std::string& fileName);
bool FileExists(const std::string& fileName);
}

#endif