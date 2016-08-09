#include "TestUtils.h"

#include <fstream>
#include <iterator>
#include <cstdio>

using namespace std;

namespace TestUtils
{

bool CreateSmallFile(const string& fileName, const string& fileContent)
{
	ofstream out(fileName, ios::binary);
	if (!out.good())
	{
		return false;
	}
	copy(fileContent.begin(), fileContent.end(), ostream_iterator<char>(out));
	return out.good();
}

bool ReadSmallFile(const string& fileName, string& fileContent)
{
	ifstream in(fileName, ios::binary);
	if (!in.good())
	{
		return false;
	}
	in.unsetf(ios::skipws); // sic!
	copy(istream_iterator<char>(in), istream_iterator<char>(), back_inserter(fileContent));
	return in.eof();
}

bool DeleteFile(const string& fileName)
{
	return remove(fileName.c_str()) == 0;
}

bool FileExists(const std::string& fileName)
{
	struct _stat64 dummy = {0};
	return !(_stat64(fileName.c_str(), &dummy) && 
		(errno == ENOENT));
}

} // namespace TestUtils