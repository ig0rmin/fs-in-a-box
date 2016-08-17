#include <libfsbox/FileIntf.h>

#include "gtest/gtest.h"
#include "TestUtils.h"

#include <sstream>

using namespace FsBox;

class FileIntfTestsuite : public testing::Test
{
protected:
	virtual void SetUp()
	{
		std::ostringstream os;
		os << "FILETEST" << counter++ << ".fsbox";
		fileName = os.str();
		ASSERT_TRUE(container.Open(fileName));
		ASSERT_TRUE(container.IsOpened());
		ASSERT_GT(container.GetFileMapping().GetFileSize(), 0);
	}

	virtual void TearDown()
	{
		container.Close();
		TestUtils::DeleteFile(fileName);
	}

	Container container;
	std::string fileName;

	static size_t counter;
};

size_t FileIntfTestsuite::counter = 0;

TEST_F(FileIntfTestsuite, WriteSimple)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);
	EXPECT_TRUE(fileIntf.IsEmpty(file));
	EXPECT_EQ(0, fileIntf.GetSize(file));

	const std::string buff = "The quick brown fox jumps over the lazy dog";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	EXPECT_FALSE(fileIntf.IsEmpty(file));
	EXPECT_EQ(buff.size(), fileIntf.GetSize(file));

	// read 
	std::string outBuff(buff.size(), ' ');
	EXPECT_EQ(outBuff.size(), fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));

	EXPECT_STREQ(buff.c_str(), outBuff.c_str());
}

TEST_F(FileIntfTestsuite, AppendSimple)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string part1 = "Hello, ";
	EXPECT_TRUE(fileIntf.Write(file, &part1[0], part1.size(), 0));

	const std::string part2 = "world";
	EXPECT_TRUE(fileIntf.Append(file, &part2[0], part2.size()));

	EXPECT_EQ(part1.size() + part2.size(), fileIntf.GetSize(file));

	// read
	std::string outBuff(part1.size() + part2.size(), ' ');
	EXPECT_EQ(outBuff.size(), fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));

	EXPECT_STREQ("Hello, world", outBuff.c_str());
}

TEST_F(FileIntfTestsuite, Rewrite)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. John";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	const std::string newName = "Jeff";
	size_t oldNamePos = buff.find("John");
	EXPECT_NE(std::string::npos, oldNamePos);
	EXPECT_TRUE(fileIntf.Write(file, &newName[0], newName.size(), oldNamePos));

	EXPECT_EQ(buff.size(), fileIntf.GetSize(file));

	// read
	std::string outBuff(buff.size(), ' ');
	EXPECT_EQ(outBuff.size(), fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));

	EXPECT_STREQ("Good morning, mr. Jeff", outBuff.c_str());
}

TEST_F(FileIntfTestsuite, RewriteWithExpand)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. John";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	const std::string newName = "Jeff Lebowski";
	size_t oldNamePos = buff.find("John");
	EXPECT_NE(std::string::npos, oldNamePos);

	EXPECT_TRUE(fileIntf.Write(file, &newName[0], newName.size(), oldNamePos));
	EXPECT_EQ(buff.size() + std::string(" Lebowski").size(), fileIntf.GetSize(file));

	// read
	std::string outBuff(static_cast<size_t>(fileIntf.GetSize(file)), ' ');
	EXPECT_EQ(outBuff.size(), fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));

	EXPECT_STREQ("Good morning, mr. Jeff Lebowski", outBuff.c_str());
}

TEST_F(FileIntfTestsuite, ReadWithOffset)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. Jeff Lebowski";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	std::string outBuff(4, ' ');
	size_t namePos = buff.find("Jeff");
	EXPECT_NE(std::string::npos, namePos);
	EXPECT_EQ(outBuff.size(), fileIntf.Read(file, &outBuff[0], outBuff.size(), namePos));
	EXPECT_STREQ("Jeff", outBuff.c_str());
}

TEST_F(FileIntfTestsuite, WriteOutOfFile)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. Jeff Lebowski";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	EXPECT_FALSE(fileIntf.Write(file, &buff[0], buff.size(), 50));
}

TEST_F(FileIntfTestsuite, ReadTooMuch)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. Jeff Lebowski";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	std::string outBuff(50, ' ');
	size_t read = fileIntf.Read(file, &outBuff[0], outBuff.size(), 0);

	EXPECT_EQ(buff.size(), read);
	outBuff.resize(read);
	EXPECT_STREQ(buff.c_str(), outBuff.c_str());
}

TEST_F(FileIntfTestsuite, ReadOnEmpty)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);
	EXPECT_TRUE(fileIntf.IsEmpty(file));

	std::string outBuff(1, ' ');
	EXPECT_EQ(0, fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));
}

TEST_F(FileIntfTestsuite, TruncateSimple)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. Jeff Lebowski";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	EXPECT_TRUE(fileIntf.Truncate(file, 1));

	EXPECT_EQ(1, fileIntf.GetSize(file));
	
	// read 
	std::string outBuff(1, ' ');
	EXPECT_TRUE(fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));
	EXPECT_STREQ("G", outBuff.c_str());
}

TEST_F(FileIntfTestsuite, TruncateAll)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. Jeff Lebowski";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	EXPECT_TRUE(fileIntf.Truncate(file, 0));

	EXPECT_TRUE(fileIntf.IsEmpty(file));
}

TEST_F(FileIntfTestsuite, TruncateSameSize)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. Jeff Lebowski";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	EXPECT_EQ(buff.size(), fileIntf.GetSize(file));

	//read
	std::string outBuff(buff.size(), ' ');
	EXPECT_EQ(outBuff.size(), fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));

	EXPECT_STREQ(buff.c_str(), outBuff.c_str());
}

TEST_F(FileIntfTestsuite, AppendAndTruncate)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buffA = "A";
	EXPECT_TRUE(fileIntf.Append(file, &buffA[0], buffA.size()));

	const std::string buffB = "B";
	EXPECT_TRUE(fileIntf.Append(file, &buffB[0], buffB.size()));

	fileIntf.Truncate(file, 1);

	EXPECT_EQ(1, fileIntf.GetSize(file));
	//read
	std::string outBuff(1, ' ');
	EXPECT_EQ(1, fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));
	EXPECT_STREQ(buffA.c_str(), outBuff.c_str());
}

TEST_F(FileIntfTestsuite, WriteBigBlock)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff(FStreamIntf::GetMaxPayloadSize() * 2 + 4, 'P');

	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	std::string outBuff(buff.size(), ' ');
	EXPECT_TRUE(fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));

	EXPECT_STREQ(buff.c_str(), outBuff.c_str());
}

TEST_F(FileIntfTestsuite, DeleteFile)
{
	FStreamIntf fileIntf(container);

	BlockHandle file = fileIntf.Create();
	EXPECT_TRUE(file);

	const std::string buff = "Good morning, mr. Jeff Lebowski";
	EXPECT_TRUE(fileIntf.Write(file, &buff[0], buff.size(), 0));

	EXPECT_EQ(buff.size(), fileIntf.GetSize(file));

	fileIntf.Delete(file);

	EXPECT_TRUE(fileIntf.IsEmpty(file));
	EXPECT_EQ(0, fileIntf.GetSize(file));
	
	std::string outBuff(1, ' ');
	EXPECT_EQ(0, fileIntf.Read(file, &outBuff[0], outBuff.size(), 0));
	std::string inBuff(1, ' ');
	EXPECT_FALSE(fileIntf.Write(file, &inBuff[0], inBuff.size(), 0));
	EXPECT_FALSE(fileIntf.Truncate(file, 1));
}