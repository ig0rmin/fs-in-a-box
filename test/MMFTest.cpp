#include <libfsbox/MMF.h>

#include "gtest/gtest.h"
#include "TestUtils.h"

using namespace std;
using namespace FsBox;

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
	ASSERT_TRUE(TestUtils::CreateSmallFile(fileName, inFileBody));
	ASSERT_TRUE(TestUtils::ReadSmallFile(fileName, outFileBodyTest));
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

	TestUtils::DeleteFile(fileName);
}

TEST(MMF, ResizeSmall)
{
	const string fileName = "SMALL01.txt";
	ASSERT_TRUE(TestUtils::CreateSmallFile(fileName, " "));

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
	EXPECT_TRUE(TestUtils::ReadSmallFile(fileName, testFileBody));

	EXPECT_EQ(newFileBody, testFileBody);

	TestUtils::DeleteFile(fileName);
}

TEST(MMF, RemapLarge)
{
	const string fileName = "LARGE00.txt";
	ASSERT_TRUE(TestUtils::CreateSmallFile(fileName, " "));

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

	TestUtils::DeleteFile(fileName);
}