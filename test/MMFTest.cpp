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
	return remove(fileName.c_str()) == 0;
}

TEST(MMF, MissingFile)
{
	MemoryMappedFile mmf;
	
	EXPECT_FALSE(mmf.Open("No such file"));
}

TEST(MMF, ReadSmall)
{
	const string fileName = "SMALL00.txt";
	const string inFileBody = "FILE BODY";
	string outFileBodyTest;
	ASSERT_TRUE(CreateSmallFile(fileName, inFileBody));
	ASSERT_TRUE(ReadSmallFile(fileName, outFileBodyTest));
	EXPECT_EQ(inFileBody, outFileBodyTest);

	{
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
	}

	DeleteFile(fileName);
}

TEST(MMF, ResizeSmall)
{
	const string fileName = "SMALL01.txt";
	ASSERT_TRUE(CreateSmallFile(fileName, " "));

	const string newFileBody = "Little brown fox";
	{
		MemoryMappedFile mmf;

		EXPECT_TRUE(mmf.Open(fileName));
		EXPECT_TRUE(mmf.IsOpened());
		EXPECT_TRUE(mmf.GetData());
		EXPECT_EQ(1u, mmf.GetFileSize());
		EXPECT_EQ(1u, mmf.GetDataSize());

		
		stream_offset newSize = newFileBody.size();

		EXPECT_TRUE(mmf.Resize(newSize));
		EXPECT_EQ(newSize, mmf.GetFileSize());
		EXPECT_EQ(newSize, mmf.GetDataSize());

		copy(newFileBody.begin(), newFileBody.end(), mmf.GetData());
	}

	string testFileBody;
	EXPECT_TRUE(ReadSmallFile(fileName, testFileBody));

	EXPECT_EQ(newFileBody, testFileBody);

	DeleteFile(fileName);
}

TEST(MMF, RemapLarge)
{
	const string fileName = "LARGE00.txt";
	ASSERT_TRUE(CreateSmallFile(fileName, " "));

	const size_t viewSize = 1024;
	const stream_offset fileSize = 10ll * 1024*1024*1024;
	const stream_offset messageOffset = 5ll * 1024*1024*1024;
	const string message = "Five gigabytes";

	// Write
	{
		MemoryMappedFile mmf(viewSize);
		
		EXPECT_TRUE(mmf.Open(fileName));

		EXPECT_TRUE(mmf.Resize(fileSize));
		EXPECT_TRUE(mmf.GetData());
		EXPECT_EQ(fileSize, mmf.GetFileSize());
		EXPECT_EQ(viewSize, mmf.GetDataSize());
		
		EXPECT_TRUE(mmf.Remap(messageOffset));
		EXPECT_TRUE(mmf.GetData());
		EXPECT_EQ(fileSize, mmf.GetFileSize());
		EXPECT_EQ(viewSize, mmf.GetDataSize());

		copy(message.begin(), message.end(), mmf.GetData());
	}

	// Read
	{
		MemoryMappedFile mmf(viewSize);

		EXPECT_TRUE(mmf.Open(fileName, messageOffset));
		EXPECT_TRUE(mmf.GetData());
		EXPECT_EQ(fileSize, mmf.GetFileSize());
		EXPECT_EQ(viewSize, mmf.GetDataSize());
		
		const char* b = mmf.GetData();
		const char* e = b + message.size();

		const string testMessage(b, e);

		EXPECT_EQ(message, testMessage);
	}

	DeleteFile(fileName);
}