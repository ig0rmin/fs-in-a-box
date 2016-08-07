#include <libfsbox/MMF.h>
#include <libfsbox/Logging.h>

#include "gtest/gtest.h"

#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <cstdio>

using namespace std;

static bool CreateSmallFile(const string& fileName, const string& fileContent)
{
	ofstream out(fileName, ios::binary);
	if (!out.good())
	{
		return false;
	}
	copy(fileContent.begin(), fileContent.end(), ostream_iterator<char>(out));
	return out.good();
}

static bool ReadSmallFile(const string& fileName, string& fileContent)
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

static bool DeleteFile(const string& fileName)
{
	return remove(fileName.c_str());
}

TEST(MMF, MissingFile)
{
	MemoryMappedFile mmf;
	
	EXPECT_FALSE(mmf.Open("No such file"));
}

TEST(MMF, ReadSmallFile)
{
	const string fileName = "SMALL.txt";
	const string inFileBody = "FILE BODY";
	string outFileBodyTest;
	ASSERT_TRUE(CreateSmallFile(fileName, inFileBody));
	ASSERT_TRUE(ReadSmallFile(fileName, outFileBodyTest));
	EXPECT_EQ(inFileBody, outFileBodyTest);

	MemoryMappedFile mmf;

	EXPECT_TRUE(mmf.Open(fileName));
	EXPECT_TRUE(mmf.IsOpened());

	EXPECT_TRUE(mmf.GetData());
	EXPECT_EQ(inFileBody.size(), mmf.GetFileSize());
	EXPECT_EQ(inFileBody.size(), mmf.GetDataSize());
	
	const char* b = mmf.GetData();
	const char* e = b + mmf.GetDataSize();
	string outFileBody(b, e);
	EXPECT_EQ(inFileBody, outFileBody);

	DeleteFile(fileName);
}